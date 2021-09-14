# DFRobot_TMF8x01

The TMF8801 is a dToF (direct time of flight) optical distance sensor module achieving up to 2500mm target detection distance. <br>
The TMF8701 is a time-of-flight (TOF) sensor in a single modular package with associated VCSEL. The TOF device is based on SPAD, TDC and histogram technology.<br>

这里需要显示拍照图片，可以一张图片，可以多张图片（不要用SVG图）

![正反面svg效果图](https://github.com/Arya11111/DFRobot_MCP23017/blob/master/resources/images/SEN0245svg1.png)


## Product Link（链接到英文商城）
    
   
## Table of Contents

* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary
### config table
  Type | suport ranging mode | ranging ranges | Accuracy |
---------------- | ---------------- | ---------------- | ---------------- |
TMF8801     | PROXIMITY and DISTANCE hybrid mode(only one)  |   20~240cm   | 20~100mm: +/-15mm <br> 100~200mm: +/-10mm |
TMF8701     | PROXIMITY mode |   0~10cm   |   >=200: +/-%5   |
TMF8701     | DISTANCE mode  |  10~60cm   |  100~200mm: +/-10mm |
TMF8701     | PROXIMITY and DISTANCE hybrid mode  |  0~60cm  |

### hardware conneted table
  TMF8x01 | MCU |
---------------- | ---------------- |
I2C | I2C Interface |
EN  | not connected, floating |
INT | not connected, floating |
PIN0 | not connected, floating |
PIN1 | not connected, floating |

### Installation

To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.

### Installation

### Calibrtion
14字节校准数据，获取精准校准数据的条件：<br>
1.40cm范围内没有目标物体；<br>
2.黑暗条件或弱光照<br>
满足上面2种条件，用calibrtion.ino示例demo获取的校准数据比较正常。<br>

```C++

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
 * @brief get device version.
 * @return return string of device version,format:
 * @n major_minor/patch_hw_serialnum
 */
  String getVersion();
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
```

## Compatibility

MCU                | Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Uno        |      √       |              |             | 
Mega2560           |      √       |              |             | 
Leonardo           |      X       |              |             | 
ESP32              |      √       |              |             | 
micro:bit          |      √       |              |             | 
FireBeetle M0      |      √       |              |             |
raspberry          |      √       |              |             |

## History

- Data 2021-04-06
- Version V1.0

## Credits

Written by(xue.peng@dfrobot.com), 2021. (Welcome to our [website](https://www.dfrobot.com/))





