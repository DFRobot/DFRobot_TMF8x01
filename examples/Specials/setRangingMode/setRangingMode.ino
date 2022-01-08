/*!
 * @file setRangingMode.ino(This demo is only suport TMF8701 sensor)
 * @brief Set Ranging mode to adjustment measurement distance in TMF8701
 * @n note: TMF8801 only suport one mode, PROXIMITY and DISTANCE hybrid mode.
 * *
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
 * ------------------------------------------
 * |  TMF8x01  |            MCU              |
 * |-----------------------------------------|
 * |    I2C    |       I2C Interface         |
 * |-----------------------------------------|
 * |    EN     |   not connected, floating   |
 * |-----------------------------------------|
 * |    INT    |   not connected, floating   |
 * |-----------------------------------------|
 * |    PIN0   |   not connected, floating   |
 * |-----------------------------------------|
 * |    PIN1   |    not connected, floating  |
 * |-----------------------------------------|
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @data   2021-04-02
 * @url https://github.com/DFRobot/DFRobot_TMF8x01
 */

#include "DFRobot_TMF8x01.h"

#define EN       -1                      //EN pin of of TMF8x01 module is floating, not used in this demo
#define INT      -1                      //INT pin of of TMF8x01 module is floating, not used in this demo

DFRobot_TMF8701 tof(/*enPin =*/EN,/*intPin=*/INT);

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

  Serial.print("Software Version: ");
  Serial.println(tof.getSoftwareVersion());
  Serial.print("Unique ID: ");
  Serial.println(tof.getUniqueID(),HEX);
  Serial.print("Model: ");
  Serial.println(tof.getSensorModel());

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
  tof.startMeasurement(/*cailbMode =*/tof.eModeCalib, /*disMode =*/tof.ePROXIMITY);
}

void loop() {
  if (tof.isDataReady()) {                                                                        //Is check measuring data vaild, if vaild that print measurement data to USB Serial COM.
      Serial.print("Distance = ");
      Serial.print(tof.getDistance_mm());                                                         //Print measurement data to USB Serial COM, unit mm, in eCOMBINE mode.
      Serial.println(" mm");
  }
}