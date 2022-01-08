/*!
 * @file interrupt.ino
 * @brief If you enable INT pin, MCU will capture a interrupt signal when the measure is completed.
 * @n You can attach the INT pin of TMF8x01 to MCU external interrupt pin.
 * @n When the sensor has data, the INT pin will generate a low level signal, and the MCU can determine whether there is data coming by detecting the low level
 * *
 * Ranging mode configuration table: 
 * --------------------------------------------------------------------------------|
 * |  Type     |   suport ranging mode     |  ranging ranges |  Accuracy           |
 * |---------------------------------------|-----------------|---------------------|
 * |  TMF8801  | PROXIMITY and DISTANCE    |                 |  20~100mm: +/-15mm  |
 * |           |  hybrid mode(only one)    |    20~240cm     |  100~200mm: +/-10mm |
 * |           |                           |                 |   >=200: +/-%5      |
 * |---------------------------------------|-----------------|---------------------|
 * |           |     PROXIMITY mode        |    0~10cm       |                     |
 * |           |---------------------------|-----------------|   >=200: +/-%5      |
 * |  TMF8701  |     DISTANCE mode         |    10~60cm      |  100~200mm: +/-10mm |
 * |           |---------------------------|-----------------|                     | 
 * |           | PROXIMITY and DISTANCE    |    0~60cm       |                     |
 * |           |      hybrid mode          |                 |                     |
 * |---------------------------------------|-----------------|----------------------
 * *
 * @n hardware conneted table:
 * -------------------------------------------------------
 * |  TMF8x01  |            MCU                           |
 * |------------------------------------------------------|
 * |    I2C    |       I2C Interface                      |
 * |------------------------------------------------------|
 * |    EN     |   not connected, floating                |
 * |------------------------------------------------------|
 * |    INT    |   to the external interrupt pin of MCU   |
 * |------------------------------------------------------|
 * |    PIN0   |   not connected, floating                |
 * |------------------------------------------------------|
 * |    PIN1   |    not connected, floating               |
 * |------------------------------------------------------|
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @data  2021-03-26
 * @url https://github.com/DFRobot/DFRobot_TMF8x01
 */

#include "DFRobot_TMF8x01.h"

#define EN       -1                                                      //EN pin of of TMF8x01 module is floating, not used in this demo
#define INT      2                                                       //connected INT pin of module1 to the external interrupt pin of MCU

DFRobot_TMF8801 tof(/*enPin =*/EN,/*intPin=*/INT);
//DFRobot_TMF8701 tof(/*enPin =*/EN,/*intPin=*/INT);

bool irqFlag = false;
void notifyFun(){
  irqFlag = true;
}

void setup() {
  Serial.begin(115200);                                                                               //Serial Initialization
  while(!Serial){                                                                                     //Wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initialization ranging sensor TMF8x01......");
  while(tof.begin() != 0){                                                                        //Initialization sensor,sucess return 0, fail return -1
      Serial.println("failed.");
      delay(1000);
  }
  Serial.println("done.");

  Serial.print("Software Version: ");  Serial.println(tof.getSoftwareVersion());
  Serial.print("Unique ID: ");         Serial.println(tof.getUniqueID(),HEX);
  Serial.print("Model: ");             Serial.println(tof.getSensorModel());

  tof.enableIntPin();                                                                             //Enable INT pin to check measurement data. Sending a low signal to host if measurement distance completed.
  
  #if defined(ESP32)||defined(ESP8266)
  /* mPython Interrupt Pin vs Interrupt NO
   * --------------------------------------------------------------------------------------------------
   * |                    |  DigitalPin  |             can be used as external interrupt              |
   * |    ESP32           |---------------------------------------------------------------------------|
   * |    ESP8266         | Interrupt No |  use digitalPinToInterrupt(pin numbers)                    |
   * |------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*Interrupt NO=*/digitalPinToInterrupt(INT),notifyFun,FALLING);            //Enable the external interrupt of ESP32'D9 or ESP8266's D5; rising edge trigger; connect INT to D9 in ESP32, connected INT to D5 in ESP8266
  #else
  /* Main-board of AVR series    Interrupt Pin vs Interrupt NO
   * ---------------------------------------------------------------------------------------
   * |                                        |  DigitalPin  | 2  | 3  |                   |
   * |    Uno, Nano, Mini, other 328-based    |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  |                   |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 2  | 3  | 21 | 20 | 19 | 18 |
   * |               Mega2560                 |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  | 5  |
   * |-------------------------------------------------------------------------------------|
   * |                                        |    Pin       | 3  | 2  | 0  | 1  | 7  |    |
   * |    Leonardo, other 32u4-based          |--------------------------------------------|
   * |                                        | Interrupt No | 0  | 1  | 2  | 3  | 4  |    |
   * |--------------------------------------------------------------------------------------
   */
  /* microbit  Interrupt Pin vs Interrupt NO
   * ---------------------------------------------------------------------------------------------------------------
   * |                                                   |  DigitalPin  | P0~P20 can be used as external interrupt |
   * |                  microbit                         |---------------------------------------------------------|
   * |(when used as external interrupt, do not need to   | Interrupt No | Interrupt NO is pin value, for instance, |
   * | set it to input mode via pinMode)                 |              | the Interrupt NO of P0 is 0, P1 is 1.    |
   * |-------------------------------------------------------------------------------------------------------------|
   */
  attachInterrupt(/*Interrupt NO=*/0,notifyFun,FALLING);                                                 //Enable external interrupt 0, connect INTA to the main-controller's digital pin: UNO(2),Mega2560(2),Leonardo(3),microbit(P0)
  #endif
  
/**
 * @brief Config measurement params to enable measurement. Need to call stopMeasurement to stop ranging action.
 * @param cailbMode: Is an enumerated variable of eCalibModeConfig_t, which is to config measurement cailibration mode.
 * @n     eModeNoCalib  :          Measuring without any calibration data.
 * @n     eModeCalib    :          Measuring with calibration data.
 * @n     eModeCalibAndAlgoState : Measuring with calibration and algorithm state.
 * @param disMode : the ranging mode of TMF8701 sensor.(this mode only TMF8701 support)
 * @n     ePROXIMITY: Raing in PROXIMITY mode,ranging range 0~10cm
 * @n     eDISTANCE: Raing in distance mode,ranging range 10~60cm
 * @n     eCOMBINE:  Raing in PROXIMITY and DISTANCE hybrid mode,ranging range 0~60cm
 */
  tof.startMeasurement(/*cailbMode =*/tof.eModeCalib);
  //tof.startMeasurement(/*cailbMode =*/tof.eModeCalib, /*disMode =*/tof.ePROXIMITY);      //only support TMF8701
}

void loop() {
  if(irqFlag){
      irqFlag = false;
      if (tof.isDataReady()) {                                                                        //Is check measuring data vaild, if vaild that print measurement data to USB Serial COM.
          Serial.print("Distance = ");
          Serial.print(tof.getDistance_mm());                                                         //Print measurement data to USB Serial COM, unit mm, in eCOMBINE mode.
          Serial.println(" mm");
      }
  }
}
