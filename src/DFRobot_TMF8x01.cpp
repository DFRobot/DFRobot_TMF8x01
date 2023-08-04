/*!
 * @file DFRobot_TMF8x01.cpp
 * @brief Define the basic structure of class DFRobot_TMF8x01
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-03-16
 * @url https://github.com/DFRobot/DFRobot_TMF8x01
 */
#include <Arduino.h>
#include "DFRobot_TMF8x01.h"


#define REG_MTF8x01_ENABLE    0xE0
#define REG_MTF8x01_APPID     0x00
#define REG_MTF8x01_ID        0xE3
#define REG_MTF8x01_APPREQID  0x02
#define REG_MTF8x01_CMD_DATA9   0X06
#define REG_MTF8x01_CMD_DATA8   0X07
#define REG_MTF8x01_CMD_DATA7   0X08
#define REG_MTF8x01_CMD_DATA6   0X09
#define REG_MTF8x01_CMD_DATA5   0X0A
#define REG_MTF8x01_CMD_DATA4   0X0B
#define REG_MTF8x01_CMD_DATA3   0X0C
#define REG_MTF8x01_CMD_DATA2   0X0D
#define REG_MTF8x01_CMD_DATA1   0X0E
#define REG_MTF8x01_CMD_DATA0   0X0F
#define REG_MTF8x01_COMMAND     0X10
#define REG_MTF8x01_FACTORYCALIB   0X20
#define REG_MTF8x01_STATEDATAWR    0X2E
#define REG_MTF8x01_STATUS         0x1D
#define REG_MTF8x01_CONTENTS       0x1E
#define REG_MTF8x01_TJ             0x32
#define REG_MTF8x01_RESULT_NUMBER  0x20
#define REG_MTF8x01_INT_ENAB       0xE2
#define REG_MTF8x01_INT_STATUS       0xE1
#define REG_MTF8x01_VERSION_MAJOR   0X01  
#define REG_MTF8x01_VERSION_MINORANDPATCH   0x12 
#define REG_MTF8x01_VERSION_HW              0xE3
#define REG_MTF8x01_VERSION_SERIALNUM       0x28



DFRobot_TMF8x01::DFRobot_TMF8x01(int enPin, int intPin,TwoWire &pWire)
  :_en(enPin),_intPin(intPin),_initialize(false),_count(0), _config(0),_timestamp(0), _measureCmdFlag(false),_addr(0x41), _pWire(&pWire){
  memset(_hostTime, 0 ,sizeof(_hostTime));
  memset(_MoudleTime, 0 ,sizeof(_MoudleTime));
  memset(&_result, 0 ,sizeof(_result));
  memset(_measureCmdSet, 0 , sizeof(_measureCmdSet));
  memset(_calibData, 0 , sizeof(_calibData));
  memset(_algoStateData, 0 , sizeof(_algoStateData));
}

DFRobot_TMF8x01::~DFRobot_TMF8x01(){
  _pWire = NULL;
}

int DFRobot_TMF8x01::begin(){
  _initialize = false;
  gpioInit();
  if(_pWire == NULL){
      DBG("IIC bus pointer is NULL.");
      return -1;
  } 
  _pWire->begin();
  if(!isI2CAddress(_addr)){
      DBG("IIC addr is error.");
      return -1;
  }
  sleep();
  eEnableReg_t regValue;
  regValue.value = 1;
  writeReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));
  if(!waitForCpuReady()){
      DBG("waitForCpuReady is failed.")
      return false;
  }
  if(getAppId() == 0x80){//bootloader
      DBG("bootloader")
      if(!downloadRamPatch()){
          DBG("download ram patch is failed.");
          return false;
      } 
      if(!IS_APP0){
          DBG("APP0 is not running.");
          return false;
      } 
  }else{
      DBG("app0")
  }
  _initialize = true;
  return 0;
}

void DFRobot_TMF8x01::sleep(){
  //sensor reset which will enter bootloader.
  eEnableReg_t regValue;
  readReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));
  regValue.cpuReset = 1;
  writeReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));
  _measureCmdFlag = false;
   _count = 0;
  // waitForCpuReady();
  // waitForApplication();
  // _measureCmdFlag = false;
  // _count = 0;
}

bool DFRobot_TMF8x01::wakeup(){
  eEnableReg_t regValue;
  regValue.value = 1;
  writeReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));
  if(!waitForCpuReady()){
      DBG("waitForCpuReady is failed.")
      return false;
  }
  if(getAppId() == 0x80){//bootloader
      DBG("bootloader")
      if(!downloadRamPatch()){
          DBG("download ram patch is failed.");
          return false;
      } 
      if(!IS_APP0){
          DBG("APP0 is not running.");
          return false;
      } 
  }else{
      DBG("app0")
  }
  if(_measureCmdSet[CMDSET_INDEX_CMD6] & (1<< CMDSET_BIT_INT)){
      modifyCmdSet(CMDSET_INDEX_CMD6, CMDSET_BIT_INT, true);
  }
  if(!setCaibrationMode((eCalibModeConfig_t)getCalibrationMode())) return false;
  return true;
}

String DFRobot_TMF8x01::getSoftwareVersion(){
  String str = "";
  char testChr[10];
  uint8_t val = 0;
  readReg(0x01, &val, 1);
  sprintf(testChr,"%X.",val);
  str += String(testChr);
  //Serial.print("major=");Serial.println(val,HEX);
  readReg(0x12, &val, 1);
  sprintf(testChr,"%X.",val);
  str += String(testChr);
  //Serial.print("minor=");Serial.println(val,HEX);
  readReg(0x13, &val, 1);
  sprintf(testChr,"%X.",val);
  str += String(testChr);
  readReg(0xe4, &val, 1);
  sprintf(testChr,"%X",val);
  str += String(testChr);
  //Serial.print("patch=");Serial.println(val,HEX);
  //readReg(0xe3, &val, 1);
  //Serial.print("chip=");Serial.println(val,HEX);
  //readReg(0xe4, &val, 1);
  //Serial.print("chip id=");Serial.println(val,HEX);
  /*
  char testChr[10];
  uint8_t regValue = 0;
  uint8_t waitForTimeOutMs = 100;
  uint8_t waitForTimeoutIncMs = 5;
  uint8_t valBuf[3];
  readReg(REG_MTF8x01_VERSION_MAJOR, &regValue, sizeof(regValue));

  sprintf(testChr,"%X.",regValue);
  str += String(testChr);
  regValue = 0;
  readReg(REG_MTF8x01_VERSION_MINORANDPATCH, &regValue, sizeof(regValue));
  sprintf(testChr,"%X.",regValue);
  str += String(testChr);
  regValue = 0;
  readReg(REG_MTF8x01_VERSION_HW, &regValue, sizeof(regValue));

  sprintf(testChr,"%X",regValue);
  str += String(testChr);*/
  return str;
}

uint32_t DFRobot_TMF8x01::getUniqueID(){
  uint8_t regValue = 0x47;
  uint32_t rslt;
  uint8_t buf[4];
  uint8_t waitForTimeOutMs = 100;
  uint8_t waitForTimeoutIncMs = 5;
  uint8_t *pBuf = (uint8_t*)(&rslt);
  uint8_t data2[] = {0xff};
  regValue = 0x47;
  writeReg(REG_MTF8x01_COMMAND, &regValue, 1);
  regValue = 0;
  for(uint8_t t = 0; t < waitForTimeOutMs; t += waitForTimeoutIncMs){
      readReg(0x1e, &regValue, sizeof(regValue));
      if(regValue == 0x47){
          readReg(REG_MTF8x01_VERSION_SERIALNUM, buf, 4);
#if defined(__AVR__)
          pBuf[0] = buf[0];
          pBuf[1] = buf[1];
          pBuf[2] = buf[2];
          pBuf[3] = buf[3];
#else
          //rslt = ((buf[1] << 24) | (buf[3] << 16) | (buf[1] << 8) | buf[0]) ;
          memcpy(&rslt, buf, 4);
#endif
          //Serial.print("0=");Serial.println(buf[0],HEX);
          //Serial.print("1=");Serial.println(buf[1],HEX);
          //Serial.print("2=");Serial.println(buf[2],HEX);
          //Serial.print("3=");Serial.println(buf[3],HEX);
          
          writeReg(REG_MTF8x01_COMMAND, data2, sizeof(data2));
          delay(50);
          return rslt;
      }
  }
  return 0;
}

String DFRobot_TMF8x01::getSensorModel(){
  String str = "unknown";
  uint32_t rslt = getUniqueID();
  rslt = (rslt >> 16)&0xFFFF;
  //Serial.println(rslt,HEX);
  switch(rslt){
      case MODEL_TMF8801:
           str = "TMF8801";
           break;
      case MODEL_TMF8701:
           str = "TMF8701";
           break;
      default:
           break;
  }
  return str;
}

bool DFRobot_TMF8x01::getCalibrationData(uint8_t *data, uint8_t len){
  if(!_initialize) return false;
  if(data == NULL || len != SENSOR_MTF8x01_CALIBRATION_SIZE)  return false;
  uint8_t regValue = 0x0a;
  uint8_t data2[] = {0xff};
  if(!IS_APP0) return false;
  writeReg(REG_MTF8x01_COMMAND, &regValue, 1);
  if(checkStatusRegister(0x0a)){
      readReg(REG_MTF8x01_RESULT_NUMBER, data, len);
      writeReg(REG_MTF8x01_COMMAND, data2, sizeof(data2));
      delay(50);
      return true;
  }
  return false;
}

bool DFRobot_TMF8x01::setCalibrationData(uint8_t *data, uint8_t len){
  if(data == NULL || len != SENSOR_MTF8x01_CALIBRATION_SIZE)  return false;
  memcpy(_calibData, data, len);
  return true;
}

bool DFRobot_TMF8x01::setCaibrationMode(eCalibModeConfig_t mode){
  if((!_initialize) || _measureCmdFlag) return false;
  uint8_t CalibCmd[] = {0x0B};
  sResult_t result;
  switch(mode){
      case eModeCalib:
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_CALIB, true);
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_ALGO, false);
        writeReg(REG_MTF8x01_COMMAND, CalibCmd, sizeof(CalibCmd));
        writeReg(REG_MTF8x01_RESULT_NUMBER, _calibData, sizeof(_calibData));
        break;
      case eModeCalibAndAlgoState:
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_CALIB, true);
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_ALGO, true);
        writeReg(REG_MTF8x01_COMMAND, CalibCmd, sizeof(CalibCmd));
        writeReg(REG_MTF8x01_RESULT_NUMBER, _calibData, sizeof(_calibData));
        writeReg(REG_MTF8x01_STATEDATAWR, _algoStateData, sizeof(_algoStateData));
        break;
      default:
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_CALIB, false);
        modifyCmdSet(CMDSET_INDEX_CMD7,CMDSET_BIT_ALGO, false);
        break;

  }
  writeReg(REG_MTF8x01_CMD_DATA7, _measureCmdSet, sizeof(_measureCmdSet));
  
  // for(int i = 0; i < sizeof(_measureCmdSet); i++){
      // Serial.print(_measureCmdSet[i],HEX);
      // Serial.print(", ");
  // }
  // Serial.println();
  delay(600);
  
  if(!checkStatusRegister(0x55)){
      return false;
  }
  //readReg(REG_MTF8x01_STATUS, &result, sizeof(result));
  while(_count < 4){
      if(isDataReady()){
          getDistance_mm();
      }
      delay(2);
  }
  _measureCmdFlag = true;
  return true;
}

uint8_t DFRobot_TMF8x01::getCalibrationMode(){
  uint8_t mode = 0;
  if(_measureCmdSet[CMDSET_INDEX_CMD7] & (1<< CMDSET_BIT_CALIB)){
      mode = 1;
  }
  if(_measureCmdSet[CMDSET_INDEX_CMD7] & (1<< CMDSET_BIT_ALGO)){
      mode |= (1<<CMDSET_BIT_ALGO);
  }
  return mode;
}

void DFRobot_TMF8x01::stopMeasurement(){
  uint8_t data[] = {0xff};
  writeReg(REG_MTF8x01_COMMAND, data, sizeof(data));
  delay(50);
  _measureCmdFlag = false;
  _count = 0;
  _timestamp = 1;
  memset(&_hostTime, 0, sizeof(_hostTime));
  memset(&_MoudleTime, 0, sizeof(_MoudleTime));
  memset(&_result, 0, sizeof(_result));
}

bool DFRobot_TMF8x01::isDataReady(){
  sResult_t result;
  uint32_t t,sysT;
  double t1, t2;

  memset(&result, 0, sizeof(_result));
  t = millis();
  readReg(REG_MTF8x01_STATUS, &result, sizeof(result));
  //Serial.println(result.regContents,HEX);
  if(result.regContents == 0x55){
      DBG(result.regContents,HEX);
      DBG(result.tid);
      if(result.tid != _result.tid){
          _result = result;
#if defined(__AVR__)
          memcpy(&sysT, &_result.sysclock0, 4);
#else
          sysT = ((_result.sysclock3 << 24) | (_result.sysclock2 << 16) | (_result.sysclock1 << 8) | _result.sysclock0) ;
#endif
          DBG(sysT,HEX);
          DBG(t);
          DBG(_result.sysclock3,HEX);
          DBG(_result.sysclock2,HEX);
          DBG(_result.sysclock1,HEX);
          DBG(_result.sysclock0,HEX);
          if(_count < 4){
              _hostTime[_count] = t;
              _MoudleTime[_count++] = sysT;
          }else if(_count == 4){
              _hostTime[_count] = t;
              _MoudleTime[_count] = sysT;
              if(_MoudleTime[4] > _MoudleTime[0] && (_hostTime[4] >= _hostTime[0])){
                  t1 = (_hostTime[4] - _hostTime[0])*10;
                  t2 = (_MoudleTime[4] - _MoudleTime[0])*0.2/100;
                  if((double(t1/t2) >= 0.7) && (double(t1/t2) <= 1.3)){
                      _timestamp = t1/t2;
                  }
              }
              
              for(int i = 0; i < 4; i++){
                _hostTime[i] = _hostTime[i+1];
                _MoudleTime[i] = _MoudleTime[i+1];
              }
          }else _count = 0;
          return true;
      }
  }
  if(_measureCmdSet[CMDSET_INDEX_CMD6] & (1<<CMDSET_BIT_INT)){
      uint8_t val = 0;
      readReg(REG_MTF8x01_INT_STATUS, &val, 1);
      DBG(val,HEX);
      if(val & 0x01){
          val |= 0x01;
          writeReg(REG_MTF8x01_INT_STATUS, &val, 1);
      }
  }
  return false;
}

uint16_t DFRobot_TMF8x01::getDistance_mm(){
  uint16_t rslt = (_result.disH << 8) | _result.disL;
  DBG(rslt);
  DBG(_timestamp);
  //Serial.print("rslt: ");
  //Serial.print(rslt);
  //Serial.print(", _timestamp: ");
  //Serial.println(_timestamp);
  rslt *= _timestamp;
  
  if(_measureCmdSet[CMDSET_INDEX_CMD6] & (1<<CMDSET_BIT_INT)){
      uint8_t val = 0;
      readReg(REG_MTF8x01_INT_STATUS, &val, 1);
      //Serial.println(val,HEX);
      //delay(1000);
      DBG(val,HEX);
      val |= 0x01;
      writeReg(REG_MTF8x01_INT_STATUS, &val, 1);
  }
  
  return rslt;
}

void DFRobot_TMF8x01::enableIntPin(){
  uint8_t val = 0x1;
  writeReg(REG_MTF8x01_INT_ENAB, &val, 1);
  modifyCmdSet(CMDSET_INDEX_CMD6, CMDSET_BIT_INT, true);
}

void DFRobot_TMF8x01::disableIntPin(){
  uint8_t val = 0;
  writeReg(REG_MTF8x01_INT_ENAB, &val, 1);
  modifyCmdSet(CMDSET_INDEX_CMD6, CMDSET_BIT_INT, false);
}

bool DFRobot_TMF8x01::powerOn(){
  if(!_initialize) return false;
  if(_en < 0) return false;
  delay(1000);
  digitalWrite(_en, HIGH);
  delay(1000);

  eEnableReg_t regValue;
  regValue.value = 1;
  writeReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));

  if(!waitForCpuReady()) return false;
  if(getAppId() == 0x80){//bootloader
      DBG("bootloader");
      loadApplication();
      if(!waitForApplication()) return false;
  }else{
      DBG("app0")
  }
  return true;
}

bool DFRobot_TMF8x01::powerDown(){
  if(!_initialize) return false;
  if(_en < 0) return false;
  delay(1000);
  digitalWrite(_en, LOW);
  delay(1000);
  return true;
}

bool DFRobot_TMF8x01::loadApplication(){
  uint8_t regValue = 0xC0;
  writeReg(REG_MTF8x01_APPREQID, &regValue, 1);
  if(waitForApplication()) return true;
  else return false;
}

bool DFRobot_TMF8x01::loadBootloader(){
  uint8_t regValue = 0x80;
  writeReg(REG_MTF8x01_APPREQID, &regValue, 1);
  if(waitForBootloader()) return true;
  else return false;
}

bool DFRobot_TMF8x01::waitForApplication(){
  /*100ms中，每过5ms查询一次0x00寄存器，appid超时*/
  uint8_t waitForTimeOutMs = 100;
  uint8_t waitForTimeoutIncMs = 5;
  for(uint8_t t = 0; t < waitForTimeOutMs; t += waitForTimeoutIncMs){
      delay(waitForTimeoutIncMs);
      if(IS_APP0) return true;
  }
  return false;
}

bool DFRobot_TMF8x01::waitForBootloader(){
  /*100ms中，每过5ms查询一次0x00寄存器，appid超时*/
  uint8_t waitForTimeOutMs = 100;
  uint8_t waitForTimeoutIncMs = 5;
  for(uint8_t t = 0; t < waitForTimeOutMs; t += waitForTimeoutIncMs){
      delay(waitForTimeoutIncMs);
      if(IS_BOOTLOADER) return true;
  }
  return false;
}

bool DFRobot_TMF8x01::waitForCpuReady(){
  /*100ms中，每过5ms查询一次0xE0寄存器，CPU准备就绪超时*/
  uint8_t waitForTimeOutMs = 100;
  uint8_t waitForTimeoutIncMs = 5;
  for(uint8_t t = 0; t < waitForTimeOutMs; t += waitForTimeoutIncMs){
      delay(waitForTimeoutIncMs);
      if(IS_CPU_READY) return true;
  }
  return false;
}

void DFRobot_TMF8x01::modifyCmdSet(uint8_t index, uint8_t bit, bool val){
  if(index > (sizeof(_measureCmdSet) - 1) || bit > 7) return ;
  if(val) _measureCmdSet[index] |= (1 << bit);
  else _measureCmdSet[index] = ~((~_measureCmdSet[index]) | (1 << bit));
}

uint8_t DFRobot_TMF8x01::getI2CAddress(){
  return _addr;
}

void DFRobot_TMF8x01::pinConfig(ePin_t pin, ePinControl_t config){
  uint8_t data[] = {0x0f, 0, 0x0f};
  if((pin > ePINTotal) || (config > ePinOutputHigh)) return;
  switch(pin){
      case ePIN0:
           _config |= (((uint8_t)config) << 4);
           break;
      case ePIN1:
           _config |= ((uint8_t)config);
           break;
      case ePINTotal:
           _config = ((uint8_t)config) << 4 | ((uint8_t)config);
           break;
  }
  data[1] = _config;
  writeReg(data[0], data+1, sizeof(data) - 1);
}

uint8_t DFRobot_TMF8x01::getCPUState(){
  eEnableReg_t regValue;
  readReg(REG_MTF8x01_ENABLE, &regValue, sizeof(regValue));
  return regValue.value;
}

int8_t DFRobot_TMF8x01::getJunctionTemperature_C(){
  int8_t temp = 0;
  readReg(REG_MTF8x01_TJ, &temp, 1);
  return temp;
}


uint8_t DFRobot_TMF8x01::getAppId(){
  uint8_t regValue;
  readReg(REG_MTF8x01_APPID, &regValue, sizeof(regValue));
  return regValue;
}


uint8_t DFRobot_TMF8x01::getRegContents(){
  uint8_t regValue;
  readReg(REG_MTF8x01_CONTENTS, &regValue, 1);
  return regValue;
}

bool DFRobot_TMF8x01::checkStatusRegister(uint8_t status){
  uint8_t regValue;
  
  uint8_t waitForTimeOutMs = 1000;
  uint8_t waitForTimeoutIncMs = 5;
  for(uint8_t t = 0; t < waitForTimeOutMs; t += waitForTimeoutIncMs){
      delay(waitForTimeoutIncMs);
      if(getRegContents() == status) return true;
  }
  return false;
}

bool DFRobot_TMF8x01::isI2CAddress(uint8_t addr){
  byte error, address;
  if((addr < 1) || (addr > 127)) return false;
  _pWire->beginTransmission(addr);
  error = _pWire->endTransmission();
  if(error == 0) return true;
  else return false;
}

uint8_t DFRobot_TMF8x01::calChecksum(uint8_t *data, uint8_t len){
  if(data == NULL) return 0;
  uint8_t sum = 0;
  for(uint8_t i = 0; i < len; i++){
      sum += data[i];
  }
  sum = sum ^ 0xff;
  return sum;
}
bool DFRobot_TMF8x01::readStatusACK(){
  uint32_t value = 0;
  readReg(0x08, &value, 3);
  if(value = 0xFF0000) return true;
  else return false;
}

void DFRobot_TMF8x01::conversion(String str, uint8_t *buf, uint8_t &len, uint8_t checksumbit){
  uint8_t count = 0,sum = 0;
  int i;
  bool checksumFlag = false;
  String subStr;
  str.toUpperCase();
  str.trim();
  str += ',';
  while((i = str.indexOf(',')) > -1){
      if( i != 0){
          subStr = str.substring(0, i);
          subStr.trim();
          if((subStr.length()==4) && (subStr[0]=='0') && (subStr[1]=='X')){
              buf[count++] = getCharHex(subStr[2])*16 + getCharHex(subStr[3]);
              
              if((count-1) >= checksumbit) sum += buf[count - 1];
          }
      }
      if (str.length() <= (i + 1)) break;
      str = str.substring(i+1);
      str.trim();
  }
  
  if(count > checksumbit) buf[count++] = sum ^ 0xff;
  len = count;
}

uint8_t DFRobot_TMF8x01::getCharHex(char c){
  uint8_t index = 0;
  switch(c){
      case 'A':
            index = 0x0A;
            break;
      case 'B':
            index = 0x0B;
            break;
      case 'C':
            index = 0x0C;
            break;
      case 'D':
            index = 0x0D;
            break;
      case 'E':
            index = 0x0E;
            break;
      case 'F':
            index = 0x0F;
            break;
      default:
            index = String(c).toInt();
  }
  return index;
}

void DFRobot_TMF8x01::gpioInit(){
  if(_en > -1) {
     pinMode(_en, OUTPUT);
     digitalWrite(_en, LOW);
     delay(1000);
     digitalWrite(_en, HIGH);
     delay(1000);
  }
  if(_intPin > -1){
      pinMode(_intPin, INPUT);
  }
}

void DFRobot_TMF8x01::writeReg(uint8_t reg, const void* pBuf, size_t size){
  if(pBuf == NULL){
      DBG("pBuf ERROR!! : null pointer");
  }
  
  uint8_t * _pBuf = (uint8_t *)pBuf;

  _pWire->beginTransmission(_addr);
  _pWire->write(&reg,1);

  for(uint16_t i = 0; i < size; i++){
    _pWire->write(_pBuf[i]);
  }
  _pWire->endTransmission();
}

uint8_t DFRobot_TMF8x01::readReg(uint8_t reg, void* pBuf, size_t size){
  if(pBuf == NULL){
    DBG("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->beginTransmission(_addr);
  _pWire->write(&reg, 1);
  if( _pWire->endTransmission() != 0){
      return 0;
  }
  _pWire->requestFrom(_addr, (uint8_t) size);
  for(uint16_t i = 0; i < size; i++){
    _pBuf[i] = _pWire->read();
  }
  return size;
}


//////////////////////TMF8801/////////////////////

DFRobot_TMF8801::DFRobot_TMF8801(int enPin,int intPin,TwoWire &pWire)
  :DFRobot_TMF8x01(enPin,intPin,pWire){
  String str = "0x01,0xA3,0x00,0x00,0x00,0x64,0x03,0x84,0x02";
  uint8_t len = 0;
  conversion(str,_measureCmdSet, len, sizeof(_measureCmdSet));
  str = "0x41,0x57,0x01,0xFD,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04";
  conversion(str,_calibData, len, sizeof(_calibData));
  str = "0xB1,0xA9,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00";
  conversion(str,_algoStateData, len, sizeof(_algoStateData));
}

bool DFRobot_TMF8801::startMeasurement(eCalibModeConfig_t cailbMode){
  return setCaibrationMode(cailbMode);
}

DFRobot_TMF8701::DFRobot_TMF8701(int enPin,int intPin,TwoWire &pWire)
  :DFRobot_TMF8x01(enPin,intPin,pWire){
  String str = "0x03,0x23,0x00,0x00,0x00,0x64,0xff,0xff,0x02";
  uint8_t len = 0;
  conversion(str,_measureCmdSet, len, sizeof(_measureCmdSet));
  //conversion(str,_measureCmdSet, len, sizeof(_measureCmdSet));
//0x41,0x25,0x00,0xF6,0x04,0x40,0x3D,0xFD,0x00,0x00,0x7A,0x60,0x00,0xFC
  str = "0x41,0x57,0x01,0xFD,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04";
  conversion(str,_calibData, len, sizeof(_calibData));
  str = "0xB1,0xA9,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00";
  conversion(str,_algoStateData, len, sizeof(_algoStateData));
}

bool DFRobot_TMF8701::startMeasurement(eCalibModeConfig_t cailbMode, eDistaceMode_t disMode){
  uint8_t mode = (uint8_t)disMode;
  switch(mode){
      case ePROXIMITY: //0~10cm
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_PROXIMITY,true);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_DISTANCE,false);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_COMBINE,false);
           break;
      case eDISTANCE:  //10~60cm
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_PROXIMITY,false);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_DISTANCE,true);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_COMBINE,false);
           break;
      case eCOMBINE:  //0~60cm
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_PROXIMITY,true);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_DISTANCE,true);
           modifyCmdSet(CMDSET_INDEX_CMD6,CMDSET_BIT_COMBINE,true);
           break;
  }
  return setCaibrationMode(cailbMode);
}

//#if (defined(__AVR__) || defined(ESP8266))
#include "drv/TMF8801_2.h"
#include "drv/TMF8701_2.h"
bool DFRobot_TMF8801::downloadRamPatch(){
  uint8_t buf[20], len = 0;
  uint8_t *addr = (uint8_t *)DFRobot_TMF8801_initBuf;
  String str = "";
  int flag = 0;
  memset(buf,0, 20);
  if(getAppId() != 0x80){
      if(!loadBootloader()){
          DBG("load Bootloader failed");
          return false;
      }
  }
  str = "0x08,0x14,0x01,0x29";
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  if(!readStatusACK()) return false;
  
  str = "0x08,0x43,0x02,0x00,0x00";
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  
  buf[0] = 0x08;
  buf[1] = 0x41;
  while((flag = pgm_read_byte(addr++)) > 0){
      buf[2] = flag;
      memcpy_P(buf+3, addr, flag);
      addr +=  flag;
      buf[3 + flag] = calChecksum(buf+1, flag+2);
      writeReg(buf[0], buf+1, 3+flag);
      if(!readStatusACK()) return false;
  }

  str = "0x08,0x11,0x00";//reset
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  if(waitForCpuReady()) return true;
  else return false;
}
bool DFRobot_TMF8701::downloadRamPatch(){
  uint8_t buf[20], len = 0;
  uint8_t *addr = (uint8_t *)DFRobot_TMF8701_initBuf;
  String str = "";
  int flag = 0;
  memset(buf,0, 20);
  if(getAppId() != 0x80){
      if(!loadBootloader()){
          DBG("load Bootloader failed");
          return false;
      }
  }
  str = "0x08,0x14,0x01,0x29";
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  if(!readStatusACK()) return false;
  
  str = "0x08,0x43,0x02,0x00,0x00";
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  
  buf[0] = 0x08;
  buf[1] = 0x41;
  while((flag = pgm_read_byte(addr++)) > 0){
      buf[2] = flag;
      memcpy_P(buf+3, addr, flag);
      addr +=  flag;
      buf[3 + flag] = calChecksum(buf+1, flag+2);
      writeReg(buf[0], buf+1, 3+flag);
      if(!readStatusACK()) return false;
  }
  
  str = "0x08,0x11,0x00";//reset
  conversion(str, buf, len);
  writeReg(buf[0], buf+1, len - 1);
  if(waitForCpuReady()) return true;
  else 
      return false;
}

// #else
// bool DFRobot_TMF8801::downloadRamPatch(){

  // uint8_t buf[20], len = 0;
  // String str = "";
  // int flag = 0;
  // memset(buf,0, 20);
  // str = "0x08,0x14,0x01,0x29";
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
  // if(!readStatusACK()) return false;
  
  // str = "0x08,0x43,0x02,0x00,0x00";
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
 // #include "drv/TMF8801_1.h"

  
  // str = "0x08,0x11,0x00";//reset
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
  // if(waitForCpuReady()) return true;
  // else return false;
// }

// bool DFRobot_TMF8701::downloadRamPatch(){

  // uint8_t buf[20], len = 0;
  // String str = "";
  // int flag = 0;
  // memset(buf,0, 20);
  // str = "0x08,0x14,0x01,0x29";
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
  // if(!readStatusACK()) return false;
  
  // str = "0x08,0x43,0x02,0x00,0x00";
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
  // #include "drv/TMF8701_1.h"
  
  // str = "0x08,0x11,0x00";//reset
  // conversion(str, buf, len);
  // writeReg(buf[0], buf+1, len - 1);
  // if(waitForCpuReady()) return true;
  // else return false;
// }
// #endif
