# DFRobot_TMF8x01

* [中文版](./README_CN.md)

The TMF8801 is a dToF (direct time of flight) optical distance sensor module achieving up to 2500mm target detection distance. <br>
The TMF8701 is a time-of-flight (TOF) sensor in a single modular package with associated VCSEL. The TOF device is based on SPAD, TDC and histogram technology.<br>
Both sensors support calibration, and the conditions for obtaining valid 14-byte quasi-data are as follows: <br>

1. There is no target object within 1.40cm;<br>
2. Dark conditions or weak light; <br>

If the above two conditions are met, the calibration data obtained with the calibrtion.ino example demo is relatively normal. <br>

![产品效果图](../../resources/images/SEN0430.png)![产品效果图](../../resources/images/SEN0429.png)


## Product Link（[https://www.dfrobot.com](https://www.dfrobot.com)）
    SKU: SEN0430
    SKU: SEN0429
   
## Table of Contents

* [Summary](#summary)
* [Connected](#connected)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary

It supports TMF8801 and TMF8701 TOF ranging sensors. The characteristics of these two sensors are as follows:
  Type | suport ranging mode | ranging ranges | Accuracy |
---------------- | ---------------- | ---------------- | ---------------- |
TMF8801     | PROXIMITY and DISTANCE hybrid mode(only one)  |   20~240cm   | 20~100mm: +/-15mm <br> 100~200mm: +/-10mm |
TMF8701     | PROXIMITY mode |   0~10cm   |   >=200: +/-%5   |
TMF8701     | DISTANCE mode  |  10~60cm   |  100~200mm: +/-10mm |
TMF8701     | PROXIMITY and DISTANCE hybrid mode  |  0~60cm  |

## Connected
  TMF8x01 |  raspberry pi |
---------------- | ---------------- |
I2C  | I2C Interface |
EN   | not connected, floating  or connected to the io pin of raspberry pi|
INT  | not connected, floating  or connected to the external interruput pin of raspberry pi|
PIN0 | not connected, floating  or connected to the io pin of raspberry pi or other digital device|
PIN1 | not connected, floating  or connected to the io pin of raspberry pi or other digital device|


## Installation
1. To use this library, first download the library file<br>
```python
sudo git clone https://github.com/DFRobot/DFRobot_TMF8x01
```
2. Open and run the routine. To execute a routine demo_x.py, enter python demo_x.py in the command line. For example, to execute the demo_calibration.py routine, you need to enter :<br>

```python
python demo_calibration.py 
或 
python2 demo_calibration.py 
或 
python3 demo_calibration.py
```


## Methods

```python
  '''！
    @brief Config measurement params to enable measurement. Need to call stop_measurement to stop ranging action.
    @param calib_m: Is an enumerated variable, which is to config measurement cailibration mode.
    @n     eMODE_NO_CALIB  :          Measuring without any calibration data.
    @n     eMODE_CALIB    :          Measuring with calibration data.
    @n     eMODE_CALIB_AND_ALGOSTATE : Measuring with calibration and algorithm state.
    @return status:
    @n      false:  enable measurement failed.
    @n      true:  enable measurement sucess.
  '''
  def start_measurement(self, calib_m):

  '''
    @brief Config measurement params to enable measurement. Need to call stop_measurement to stop ranging action.
    @param calib_m: Is an enumerated variable , which is to config measurement cailibration mode.
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
  def start_measurement(self, calib_m, mode):

  '''!
    @brief    initialization sensor's interface, addr, ram config to running APP0 application.
    @return   initialization sucess return 0, fail return -1
  '''
  def begin(self):

  '''!
    @brief  sleep sensor by software, the sensor enter sleep mode(bootloader). Need to call wakeup function to wakeup sensor to enter APP0
  '''
  def sleep(self):
  
  '''!
    @brief  wakeup device from sleep mode, it will running app0.
    @return enter app0 r67eturn true, or return false.
  '''
  def wakeup(self):
    
  '''!
    @brief get a unique number of sensor .Each sensor has a unique identifier.
    @return return 4bytes unique number:
    @n  the byte0 of return: serial_number_0
    @n  the byte1 of return: serial_number_1
    @n  the byte2 of return: identification_number_1
    @n  the byte2 of return: identification_number_0
  '''
  def get_unique_id(self): 

  '''!
    @brief get sensor's model.
    @return return a String:
    @n  TMF8801: the sensor is TMF8801
    @n  TMF8701: the sensor is TMF8701
    @n  unknown : unknown device
  '''
  def get_sensor_model(self): 

  '''!
    @brief get software version of patch.
    @return return string of device software version,format:
    @n major.minor.patch numbers.chip id version
  '''
  def get_software_version(self): 

  '''!
    @brief  Get 14 bytes of calibration data.
    @return return 14 bytes of calibration data.
  '''
  def get_calibration_data(self): 
    
  '''!
    @brief  set 14 bytes of calibration data.
    @param l The list of 14 bytes calibration data.
    @return set sucess return true, or return false.
  '''
  def set_calibration_data(self, l): 

  '''!
    @brief  disable measurement config.
  '''  
  def stop_measurement(self):

  '''!
    @brief  Waiting for data ready.
    @return if data is valid, return true, or return false.
  '''
  def is_data_ready(self):
    
  '''!
    @brief  get distance, unit mm. Before using this function, you need to call is_data_ready.
    @return return distance value, unit mm.
  '''  
  def get_distance_mm(self):

  '''!
    @brief  enable INT pin. If you call this function,which will report a interrupt.
    @n signal to host by INT pin when measure data is ready.
  '''
  def enable_int_pin(self):

  '''!
    @brief disable INT pin.
  '''
  def disable_int_pin(self):

  '''!
    @brief power on sensor when power down sensor by EN pin.
    @return sucess return True, or return False
  '''
  def power_on(self):
    
  '''!
    @brief power down sensor by EN pin.
    @return sucess return True, or return False
  '''
  def power_down(self):
  
  '''!
    @brief get I2C address.
    @return return 7 bits I2C address
  '''
  def get_i2c_address(self):
    
  '''!
    @brief get junction temperature of sensor.
    @return Junction temperature of sensor, unit, Celsius.
  '''
  def get_junction_temperature_C(self):
    
```

## Compatibility

| 主板         | 通过 | 未通过 | 未测试 | 备注 |
| ------------ | :--: | :----: | :----: | :--: |
| RaspberryPi2 |      |        |   √    |      |
| RaspberryPi3 |      |        |   √    |      |
| RaspberryPi4 |  √   |        |        |      |

* Python 版本

| Python  | 通过 | 未通过 | 未测试 | 备注 |
| ------- | :--: | :----: | :----: | ---- |
| Python2 |  √   |        |        |      |
| Python3 |  √   |        |        |      |

## History

- 2021/04/06 - Version 1.0.0 released.

## Credits

Written by Arya(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





