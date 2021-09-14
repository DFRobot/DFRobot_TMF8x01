from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  # Demo_interrupt.py
  #
  # If you enable INT pin, MCU will capture a interrupt signal when the measure is completed.
  # @n You can attach the INT pin of TMF8x01 to MCU external interrupt pin.
  # *
  # Ranging mode configuration table: 
  # --------------------------------------------------------------------------------|
  # |  Type     |   suport ranging mode     |  ranging ranges |  Accuracy           |
  # |---------------------------------------|-----------------|---------------------|
  # |  TMF8801  | PROXIMITY and DISTANCE    |                 |  20~100mm: +/-15mm  |
  # |           |  hybrid mode(only one)    |    20~240cm     |  100~200mm: +/-10mm |
  # |           |                           |                 |   >=200: +/-%5      |
  # |---------------------------------------|-----------------|---------------------|
  # |           |     PROXIMITY mode        |    0~10cm       |                     |
  # |           |---------------------------|-----------------|   >=200: +/-%5      |
  # |  TMF8701  |     DISTANCE mode         |    10~60cm      |  100~200mm: +/-10mm |
  # |           |---------------------------|-----------------|                     | 
  # |           | PROXIMITY and DISTANCE    |    0~60cm       |                     |
  # |           |      hybrid mode          |                 |                     |
  # |---------------------------------------|-----------------|----------------------
  # *
  # @n hardware conneted table:
  # -------------------------------------------------------
  # |  TMF8x01  |            MCU                           |
  # |------------------------------------------------------|
  # |    I2C    |       I2C Interface                      |
  # |------------------------------------------------------|
  # |    EN     |   not connected, floating                |
  # |------------------------------------------------------|
  # |    INT    |   to the external interrupt pin of MCU   |
  # |------------------------------------------------------|
  # |    PIN0   |   not connected, floating                |
  # |------------------------------------------------------|
  # |    PIN1   |    not connected, floating               |
  # |------------------------------------------------------|
  #
  # test sensor: 
  #
  # Copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  # licence     The MIT License (MIT)
  # author [Arya](xue.peng@dfrobot.com)
  # version  V1.0
  # date  2021-04-06
  # get from https://www.dfrobot.com
  # url from https://github.com/DFRobot/DFRobot_TMF8x01
'''

import time
import RPi.GPIO as GPIO

from DFRobot_TMF8x01 import DFRobot_TMF8801 as tof
#from DFRobot_TMF8x01 import DFRobot_TMF8701 as tof
tof = tof(enPin = -1, intPin = 22, bus_id = 1)    # Select bus 1

irqFlag = False

def notifyFun(channel):
  global irqFlag
  irqFlag = True

if __name__ == "__main__":
  print("Initialization ranging sensor TMF8x01......", end = " ")
  while(tof.begin() != 0):
    print("Initialization failed")
    time.sleep(1)
  print("Initialization done.")

  print("Software Version: ", end=" ")
  print(tof.getSoftwareVersion())
  print("Unique ID: %X"%tof.getUniqueID())
  print("Model: ", end=" ")
  print(tof.getSensorModel())
  
  '''Enable INT pin to check measurement data. Sending a low signal to host if measurement distance completed.'''
  tof.enableIntPin()
  GPIO.add_event_detect(22, GPIO.FALLING, notifyFun) #22 is a external interrupt pin of raspberry in BCM 

  '''
  @brief Config measurement params to enable measurement. Need to call stopMeasurement to stop ranging action.
  @param cailbMode: Is an enumerated variable of eCalibModeConfig_t, which is to config measurement cailibration mode.
  @n     eModeNoCalib  :          Measuring without any calibration data.
  @n     eModeCalib    :          Measuring with calibration data.
  @n     eModeCalibAndAlgoState : Measuring with calibration and algorithm state.
  @param mode : the ranging mode of TMF8701 sensor.
  @n     ePROXIMITY: Raing in PROXIMITY mode,ranging range 0~10cm
  @n     eDISTANCE: Raing in distance mode,ranging range 10~60cm
  @n     eCOMBINE:  Raing in PROXIMITY and DISTANCE hybrid mode,ranging range 0~60cm
  @return status:
  @n      false:  enable measurement failed.
  @n      true:  enable measurement sucess.
  '''
  tof.startMeasurement(calibMode = tof.eModeCalib);
  #tof.startMeasurement(calibMode = tof.eModeCalib, mode = tof.ePROXIMITY);
  
  while True:
    #print(irqFlag)
    if irqFlag == True:
      irqFlag = False
      if(tof.isDataReady() == True):
        print("Distance = %d mm"%tof.getDistance_mm())
    #time.sleep(0.5)
  

