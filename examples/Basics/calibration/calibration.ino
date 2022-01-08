/*!
 * @file calibration.ino
 * @brief This demo tells how to get 14 bytes calibration data.
 * @n If you want to obtain reliable calibration data, you need to calibrate under the following conditions: 
 * @ 1. no target within 40cm of the sensor;
 * @ 2. in dark conditions.
 * 
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
 * @data  2021-03-26
 * @url https://github.com/DFRobot/DFRobot_TMF8x01
 */

#include "DFRobot_TMF8x01.h"

#define EN       -1                      //EN pin of of TMF8x01 module is floating, not used in this demo
#define INT      -1                      //INT pin of of TMF8x01 module is floating, not used in this demo

DFRobot_TMF8801 tof(/*enPin =*/EN,/*intPin=*/INT);
//DFRobot_TMF8701 tof(/*enPin =*/EN,/*intPin=*/INT);

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

  /* If you want to obtain reliable calibration data, you need to calibrate under the following conditions: 
     1. no target within 40cm of the sensor;
     2. in dark conditions.*/

  // while(!Serial.available());
  // while(!Serial.available()){
      // Serial.read();
  // }

  uint8_t caliDataBuf[14] = {0};                                                                   //Store 14 bytes of calibration data.
  Serial.print("Calibration ");
  while(tof.getCalibrationData(caliDataBuf, sizeof(caliDataBuf)) != true){                         //get calibration data to update caliDataBuf buffer. You needs to be obtained for every sensor by call the function getCalibrationData.
      Serial.print(".");
      delay(1000);
  }
  
  Serial.println("Calibration complete.");
  Serial.print("caliDataBuf[0-13] = {0x");
  for(int i = 0; i < sizeof(caliDataBuf); i++){
       if(caliDataBuf[i] < 16) Serial.print("0");
       Serial.print(caliDataBuf[i],HEX);
       if(i < 13) Serial.print(", 0x");
  }
  Serial.println("}");
  
   tof.setCalibrationData(caliDataBuf, sizeof(caliDataBuf));                //Set calibration data.

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
  if(!tof.startMeasurement(/*cailbMode =*/tof.eModeCalib)){                 //Enable measuring with Calibration data.
  //if(!tof.startMeasurement(/*cailbMode =*/tof.eModeCalib, /*disMode =*/tof.eCOMBINE)){      //only support TMF8701
      Serial.println("Enable measurement faild.\nPlease check the calibration data and recalibrate!!!");
      return;
  }
}

void loop() {
if (tof.isDataReady()) {                                                 //Is check measuring data vaild, if vaild that print measurement data to USB Serial COM.
      Serial.print("Distance = ");
      Serial.print(tof.getDistance_mm());                                  //Print measurement data to USB Serial COM, unit mm, in eCOMBINE mode.
      Serial.println(" mm");
  }
}
