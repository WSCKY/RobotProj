# IMU Attitude Estimator

#### MCU:
```STM32F042G6U6, Cortex-M0, Freq(48MHz), ROM(32K), RAM(6K), UFQFPN28```
#### IMU:
```MPU9250, 3-Axis gyroscope, 3-Axis accelerometer, 3-Axis magnetometer(AK8963), QFN3x3x1mm package```
#### Compiler:
```arm-none-eabi-gcc version 6.3.1 20170620 (release)```
#### CONNECTION:
```
     SPI11    -> MPU9250 IMU Sensor
     USART2   -> Communication Port
     USB(CDC) -> Communication Port
     I2C1     -> External EEPROM(AT24C64D-SSHM-T)
     TIM2_CH2 -> Heater circuit control
```

#### Hardware Resources:
```
    9-Dof IMU Sensor <MPU9250>:
        the MPU9250 is a 9-axis MotionTracking device that combines a 3-axis gyroscope, 3-axis accelerometer, 3-axis magnetometer and a Digital Motion Processor (DMP) all in a small 3x3x1mm package available as a pin-compatible upgrade from the MPU6515.
    EEPROM Mass Storage <AT24C64>:
        Stores the gyroscope stable zero offset data, temperature calibration data(gyroscope zero offset), accelerometer calibration data and compass calibration data.
    A Heater circuit:
        provides a constant temperature envrionment for the IMU sensor.
```

                                                               kyChu@2019/02/20

