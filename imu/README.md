# IMU Attitude Estimator

#### MCU: ```STM32F042G6U6, Cortex-M0, Freq(48MHz), ROM(32K), RAM(6K), UFQFPN28```
### IMU: ```MPU9250, 3-Axis gyroscope, 3-Axis accelerometer, 3-Axis magnetometer(AK8963), QFN3x3x1mm package```
#### GCC: ```arm-none-eabi-gcc version 6.3.1 20170620 (release)```
#### CONNECTION:
```
     SPI11  -> MPU9250 IMU Sensor
     USART2 -> Communication Port
     USB    -> Communication Port
     I2C1   -> External EEPROM(AT24C64D-SSHM-T)
```

                                                               kyChu@2019/02/20

