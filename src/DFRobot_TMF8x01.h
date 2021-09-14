/*!
 * @file DFRobot_TMF8x01.h
 * @brief Ranging distance sensor, suport TMF8701 and TMF8801.
 * @n Connected by I2C interface.
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-03-16
 * @https://github.com/DFRobot/DFRobot_TMF8x01
 */
#ifndef __DFROBOT_MTF8X01_H
#define __DFROBOT_MTF8X01_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>
#include<HardwareSerial.h>

//Define DBG, change 0 to 1 open the DBG, 1 to 0 to close.  
#if 0
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

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
#define REG_MTF8x01_RESULT_NUMBER  0x20
#define REG_MTF8x01_INT_ENAB       0xE2
#define REG_MTF8x01_INT_STATUS       0xE1
#define REG_MTF8x01_VERSION_MAJOR   0X01  
#define REG_MTF8x01_VERSION_MINORANDPATCH   0x12 
#define REG_MTF8x01_VERSION_HW              0xE3
#define REG_MTF8x01_VERSION_SERIALNUM       0x28

#define SENSOR_MTF8x01_CALIBRATION_SIZE   14


#define MODEL_TMF8801      0x4120
#define MODEL_TMF8701      0x5e10


class DFRobot_TMF8x01{
public:
  #define IS_CPU_READY (getCPUState() == 0x41)
  #define IS_STANDBY (getCPUState() == 0x00)
  #define IS_APP0 (getAppId() == 0xC0)
  #define IS_BOOTLOADER (getAppId() == 0x80)
  #define IS_DATA_READY (getRegContents() == 0x55)
  
  #define CMDSET_INDEX_CMD7       0
  #define CMDSET_BIT_CALIB        0
  #define CMDSET_BIT_ALGO         1
  
  #define CMDSET_INDEX_CMD6       1
  #define CMDSET_BIT_PROXIMITY    0
  #define CMDSET_BIT_DISTANCE     1
  #define CMDSET_BIT_INT          4
  #define CMDSET_BIT_COMBINE      5

  typedef enum{
      ePIN0 = 0,  /**< the PIN0 pin of sensor*/
      ePIN1,  /**< the PIN1 pin of sensor*/
      ePINTotal
  }ePin_t;

  typedef enum{
      ePinInput = 0,  /**< input mode*/
      ePinInputLow = 1,  /**< input high level*/
      ePinInputHigh = 2,
      ePinOutputVCSEL = 3,  /**< output VCSEL signal*/
      ePinOutputLow = 4,  /**< output low level signal*/
      ePinOutputHigh = 5,  /**< output high level signal*/
  }ePinControl_t;

  typedef enum{
      eModeNoCalib = 0,  /*!< without calibration.*/
      eModeCalib = 1,  /*!< load calibration data.*/
      eModeCalibAndAlgoState = 3 /*!< load calibration data and algorithm state.*/
  }eCalibModeConfig_t;
  

 
  /*
   MTF8x01‘s ENABLE register：
   * ---------------------------------------------------------------------
   * |    b7    |    b6    |  b5  |  b4  |  b3  |  b2  |  b1  |    b0    |
   * ---------------------------------------------------------------------
   * | cpu_reset| cpu_ready|            reserve               |    pon   |
   * ---------------------------------------------------------------------
   */
  typedef union{
    struct{
      uint8_t  pon: 1; /*!< Writing a '1' to this register will enable oscillator and Writing a '0' will set device into standby state. */
      uint8_t  reserve: 5; /*!< reserved bits */
      uint8_t  cpuReady: 1; /*!< ro bit, to determine whether the CPU of the TMF8x01 device is ready, if it is ready, the value is 1.*/
      uint8_t  cpuReset: 1; /*!< rw, write a '1' to reset cpu. all config of TMF8x01 will reset. */
    };
    uint8_t value;/*!< The value of enable register.*/
  } __attribute__ ((packed)) eEnableReg_t;

  typedef struct{
      uint8_t status;  /*!< Current status or current general operation.*/
      uint8_t regContents;  /*!< Current contents of the I²C RAM from 0x20 to 0xEF, the coding is as follows: 0x0A(Calibration data) 0x47(Serial number) 0x55(Results for commands 0x02/0x03 and 0x04*/
      uint8_t tid;  /*!< Unique transaction ID, changes with every update of register map by TOF.*/
      uint8_t resultNumber;  /*!< Result number, incremented every time there is a unique answer.*/
      struct{
          uint8_t reliability:6;  /*!< Reliability of object - valid range 0..63 where 63 is best.*/
          uint8_t meastatus:2;  /*!< Will indicate the status of the measurement.*/
      }resultInfo;
      uint8_t disL;  /*!< Distance to the peak in [mm] of the object, least significant byte.*/
      uint8_t disH;  /*!< Distance to the peak in [mm] of the object, most significant byte.*/
      uint8_t sysclock0;  /*!< System clock/time stamp in units of 0.2 µs.*/
      uint8_t sysclock1;  /*!< System clock/time stamp in units of 0.2 µs.*/
      uint8_t sysclock2;  /*!< System clock/time stamp in units of 0.2 µs.*/
      uint8_t sysclock3;  /*!< System clock/time stamp in units of 0.2 µs.*/
  }sResult_t;
/**
 * @brief DFRobot_TMF8x01 abstract class constructor.
 * @param enPin: The EN pin of sensor is connected to digital IO port of the MCU.
 * @param intPin: The INT pin of sensor is connected to the exteral interrupt IO port of the MCU.
 * @param pWire : Objects of the TwoWire class. 
 */
  DFRobot_TMF8x01(int enPin, int intPin, TwoWire &pWire);
  ~DFRobot_TMF8x01();
/**
 * @brief initialization sensor's interface, addr, ram config to running APP0 application.
 * @return initialization sucess return 0, fail return -1
 */
  int begin();

/**
 * @brief sleep sensor by software, the sensor enter sleep mode(bootloader). Need to call wakeup function to wakeup sensor to enter APP0
 */
  void sleep();
/**
 * @brief wakeup device from sleep mode, it will running app0
 * @return enter app0 return true, or return false.
 */
  bool wakeup();

/**
 * @brief get a unique number of sensor .Each sensor has a unique identifier.
 * @return return 4bytes unique number:
 * @n  the byte0 of return: serial_number_0
 * @n  the byte1 of return: serial_number_1
 * @n  the byte2 of return: identification_number_1
 * @n  the byte2 of return: identification_number_0
 */
  uint32_t getUniqueID();
/**
 * @brief get sensor's model.
 * @return return a String:
 * @n  TMF8801: the sensor is TMF8801
 * @n  TMF8701: the sensor is TMF8701
 * @n  unknown : unknown device
 */
  String getSensorModel();
/**
 * @brief get software version of patch.
 * @return return string of device software version,format:
 * @n major.minor.patch numbers.chip id version
 */
  String getSoftwareVersion();
/**
 * @brief Get 14 bytes of calibration data.
 * @param data Cache for storing calibration data
 * @param len The bytes of calibration data,its value can only be 14 bytes
 * @return Vail data return true, or return false.
 */
  bool getCalibrationData(uint8_t *data, uint8_t len = SENSOR_MTF8x01_CALIBRATION_SIZE);
/**
 * @brief set 14 bytes of calibration data.
 * @param data Pointer to calibration data.
 * @param len The bytes of calibration data,its value can only be 14 bytes
 * @return set sucess return true, or return false.
 */
  bool setCalibrationData(uint8_t *data, uint8_t len = SENSOR_MTF8x01_CALIBRATION_SIZE);

/**
 * @brief disable measurement config. Need to call startMeasurement before using this function. 
 */
  void stopMeasurement();
/**
 * @brief Waiting for data ready.
 * @return if data is valid, return true, or return false.
 */
  bool isDataReady();
/**
 * @brief get distance, unit mm. Before using this function, you need to call isDataReady().
 * @return return distance value, unit mm.
 */
  uint16_t getDistance_mm();
/**
 * @brief enable INT pin. If you call this function,which will report a interrupt
 * @n signal to host by INT pin when measure data is ready.
 */
  void enableIntPin();
/**
 * @brief disable INT pin.
 */
  void disableIntPin();
/**
 * @brief power on sensor when power down sensor by EN pin.
 * @return sucess return True, or return False
 */
  bool powerOn();
/**
 * @brief power down sensor by EN pin.
 * @return sucess return True, or return False
 */
  bool powerDown();

/**
 * @brief get I2C address.
 * @return return 7 bits I2C address
 */
  uint8_t getI2CAddress();
/**
 * @brief Config the pin of sensor.
 * @param pin: The pin of sensor, example PIN0 and PIN1,which is an enumerated variable of ePin_t.
 * @n     ePIN0:  The PIN0 of sensor config.
 * @n     ePIN1:  The PIN1 of sensor.
 * @n     eGPIOTotal:  both of PIN0 and PIN1.
 * @param config:  The config of pin, which is an enumerated variable of ePinControl_t.
 */
  void pinConfig(ePin_t pin, ePinControl_t config);
  
protected:
  virtual bool downloadRamPatch() = 0;
  uint8_t  getCalibrationMode();
  bool loadApplication();
  bool loadBootloader();
  bool waitForApplication();
  bool waitForBootloader();
  bool waitForCpuReady();
  void modifyCmdSet(uint8_t index, uint8_t bit, bool val);
  uint8_t getCPUState();
  uint8_t getAppId();
  uint8_t getRegContents();
  bool checkStatusRegister(uint8_t status);
  bool isI2CAddress(uint8_t addr);
  void modifyConfig(uint8_t index, uint8_t bit, bool val);
  uint8_t *getConfigAddr(uint8_t *len = NULL);
  uint8_t calChecksum(uint8_t *data, uint8_t len);
  void conversion(String str, uint8_t *buf, uint8_t &len, uint8_t checksumbit = 1);
  uint8_t getCharHex(char c);
  bool readStatusACK();
  void writeReg(uint8_t reg, const void* pBuf, size_t size);
  uint8_t readReg(uint8_t reg, void* pBuf, size_t size);
  void gpioInit();
  bool setCaibrationMode(eCalibModeConfig_t cailbMode);
  uint8_t _measureCmdSet[9];
  uint8_t _calibData[14];
  uint8_t _algoStateData[11];
  
private:
  int _en;
  int _intPin;
  bool _initialize;
  uint8_t _count;
  uint8_t _config;
  double _timestamp;
  bool _measureCmdFlag;
  uint8_t _addr;
  TwoWire *_pWire;
  uint32_t _hostTime[5];
  uint32_t _MoudleTime[5];
  sResult_t _result;
};

class DFRobot_TMF8801: public DFRobot_TMF8x01{
public:
  DFRobot_TMF8801(int enPin = -1, int intPin = -1, TwoWire &pWire = Wire);
/**
 * @brief Config measurement params to enable measurement. Need to call stopMeasurement to stop ranging action.
 * @param cailbMode: Is an enumerated variable of eCalibModeConfig_t, which is to config measurement cailibration mode.
 * @n     eModeNoCalib  :          Measuring without any calibration data.
 * @n     eModeCalib    :          Measuring with calibration data.
 * @n     eModeCalibAndAlgoState : Measuring with calibration and algorithm state.
 * @return status:
 * @n      false:  enable measurement failed.
 * @n      true:  enable measurement sucess.
 */
  bool startMeasurement(eCalibModeConfig_t cailbMode = eModeCalib);
protected:
  bool downloadRamPatch();
private:
  
};

class DFRobot_TMF8701: public DFRobot_TMF8x01{
public:
  typedef enum{
      ePROXIMITY = 0,  /*!< Raing in PROXIMITY mode,ranging range 0~10cm.*/
      eDISTANCE = 1,  /*!< Raing in distance mode,ranging range 10~60cm.*/
      eCOMBINE = 2 /*!< Raing in PROXIMITY and DISTANCE hybrid mode,ranging range 0~60cm.*/
  }eDistaceMode_t;

  DFRobot_TMF8701(int enPin = -1, int intPin = -1, TwoWire &pWire = Wire);
/**
 * @brief Config measurement params to enable measurement. Need to call stopMeasurement to stop ranging action.
 * @param cailbMode: Is an enumerated variable of eCalibModeConfig_t, which is to config measurement cailibration mode.
 * @n     eModeNoCalib  :          Measuring without any calibration data.
 * @n     eModeCalib    :          Measuring with calibration data.
 * @n     eModeCalibAndAlgoState : Measuring with calibration and algorithm state.
 * @param disMode : the ranging mode of TMF8701 sensor.
 * @n     ePROXIMITY: Raing in PROXIMITY mode,ranging range 0~10cm
 * @n     eDISTANCE: Raing in distance mode,ranging range 10~60cm
 * @n     eCOMBINE:  Raing in PROXIMITY and DISTANCE hybrid mode,ranging range 0~60cm
 * @return status:
 * @n      false:  enable measurement failed.
 * @n      true:  enable measurement sucess.
 */
  bool startMeasurement(eCalibModeConfig_t cailbMode = eModeCalib, eDistaceMode_t disMode = eCOMBINE);
protected:
  bool downloadRamPatch();
private:
};
#endif
