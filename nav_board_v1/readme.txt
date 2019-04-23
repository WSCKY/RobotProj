MCU GPIO Pin Connection
- Device ----------- Pin -------------------------------------------------- Peripheral ------------
RGB_LED            R(PB14), G(PB13), B(PB12)                              GPIOB_OUT
BUTTON             (PB15)                                                 GPIOB_IN
EEPROM(AT24C04)    SDA(PB9), SCL(PB8)                                     I2C1
IMU(MPU9250)       NSS(PA4), SCK(PA5), MISO(PA6), MOSI(PA7), INT(PB0)     SPI1
ADC                (PB1)                                                  ADC1_IN9
HEATER             (PB10)                                                 GPIOB_OUT/TIM2_CH3
GPS(MAX-M8Q-0-10)  UART_RX(PB7), UART_TX(PB6)                             USART1
COMPORT            UART_RX(PA3), UART_TX(PA2)                             USART2
PWR_3V3_CTRL       (PA9)                                                  GPIOA_OUT
PWR_IMU_CTRL       (PB2)                                                  GPIOB_OUT
USB                DP(PA12), DM(PA11), ID(PA10)                           USB

MCU Peripheral DMA Allocation
- PeriphReq ------ Available(DMAxStreamy) ----- Selected ------------
USART1_RX        (2, 2) / (2, 5)              (2, 5) /Channel4
USART1_TX        (2, 7)                       (2, 7) /Channel4
USART2_RX        (1, 5)                       (1, 5) /Channel4
USART2_TX        (1, 6)                       (1, 6) /Channel4
SPI1_RX          (2, 0) / (2, 2)              (2, 2) /Channel3
SPI1_TX          (2, 3) / (2, 5)              (2, 3) /Channel3
I2C1_RX          (1, 0) / (1, 5)              (1, 0) /Channel1
I2C1_TX          (1, 6) / (1, 7)              (1, 7) /Channel1
ADC1             (2, 0) / (2, 4)              (2, 4) /Channel0

MCU Timer configuration
TIM2 as heater driver
TIM3 as timer counter


ERROR: B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 C2 01 00 01 00 01 00 00 00 00 00 63 DD
RIGHT: B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 C2 01 00 01 00 01 00 00 00 00 00 B8 42
