from __future__ import print_function
# -*- coding:utf-8 -*-

'''
  @file demo_set_ranging_mode.py(This demo is only suport TMF8701 sensor)
  @brief Set Ranging mode to adjustment measurement distance in TMF8701
  @note TMF8801 only suport one mode, PROXIMITY and DISTANCE hybrid mode.
  @n Ranging mode configuration table: 
  @n --------------------------------------------------------------------------------|
  @n |  Type     |   suport ranging mode     |  ranging ranges |  Accuracy           |
  @n |---------------------------------------|-----------------|---------------------|
  @n |  TMF8801  | PROXIMITY and DISTANCE    |                 |  20~100mm: +/-15mm  |
  @n |           |  hybrid mode(only one)    |    20~240cm     |  100~200mm: +/-10mm |
  @n |           |                           |                 |   >=200: +/-%5      |
  @n |---------------------------------------|-----------------|---------------------|
  @n |           |     PROXIMITY mode        |    0~10cm       |                     |
  @n |           |---------------------------|-----------------|   >=200: +/-%5      |
  @n |  TMF8701  |     DISTANCE mode         |    10~60cm      |  100~200mm: +/-10mm |
  @n |           |---------------------------|-----------------|                     | 
  @n |           | PROXIMITY and DISTANCE    |    0~60cm       |                     |
  @n |           |      hybrid mode          |                 |                     |
  @n |---------------------------------------|-----------------|----------------------
  @n hardware conneted table:
  @n ------------------------------------------
  @n |  TMF8x01  |            MCU              |
  @n |-----------------------------------------|
  @n |    I2C    |       I2C Interface         |
  @n |-----------------------------------------|
  @n |    EN     |   not connected, floating   |
  @n |-----------------------------------------|
  @n |    INT    |   not connected, floating   |
  @n |-----------------------------------------|
  @n |    PIN0   |   not connected, floating   |
  @n |-----------------------------------------|
  @n |    PIN1   |    not connected, floating  |
  @n |-----------------------------------------|
  @n
  @n
  @Copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license     The MIT License (MIT)
  @author [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2021-04-06
  @url https://github.com/DFRobot/DFRobot_TMF8x01
'''


import sys
import os
import time

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
from DFRobot_TMF8x01 import DFRobot_TMF8701 as tof
tof = tof(enPin = -1, intPin = -1, bus_id = 1)

if __name__ == "__main__":
  print("Initialization ranging sensor TMF8x01......", end = " ")
  while(tof.begin() != 0):
    print("Initialization failed")
    time.sleep(1)
  print("Initialization done.")
  
  print("Software Version: ", end=" ")
  print(tof.get_software_version())
  print("Unique ID: %X"%tof.get_unique_id())
  print("Model: ", end=" ")
  print(tof.get_sensor_model())
  
  '''
  @brief Config measurement params to enable measurement. Need to call stop_measurement to stop ranging action.
  @param cailbMode: Is an enumerated variable , which is to config measurement cailibration mode.
  @n     eMODE_NO_CALIB  :          Measuring without any calibration data.
  @n     eMODE_CALIB    :          Measuring with calibration data.
  @n     eMODE_CALIB_AND_ALGOSTATE : Measuring with calibration and algorithm state.
  @param mode : the ranging mode of TMF8701 sensor.
  @n     ePROXIMITY: Raing in PROXIMITY mode,ranging range 0~10cm
  @n     eDISTANCE: Raing in distance mode,ranging range 10~60cm
  @n     eCOMBINE:  Raing in PROXIMITY and DISTANCE hybrid mode,ranging range 0~60cm
  @return status:
  @n      false:  enable measurement failed.
  @n      true:  enable measurement sucess.
  '''
  tof.start_measurement(calib_m = tof.eMODE_CALIB, mode = tof.ePROXIMITY)
  
  while True:
    if(tof.is_data_ready() == True):
      print("Distance = %d mm"%tof.get_distance_mm())
    #time.sleep(2)

