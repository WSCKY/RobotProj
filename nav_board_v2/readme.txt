NAVIGATION BOARD V2 INFORMATION:

MCU GPIO Pin Connection
- Device ----------- Pin -------------------------------------------------- Peripheral ------------
RGB_LED            R(PB4), G(PB5), B(PB3)                                 GPIOB_OUT
BUTTON             (PA15)                                                 GPIOA_IN
W25Q64             NSS(PA4), SCK(PA5), MISO(PA6), MOSI(PA7), WP(PB0)      SPI1
IMU(LSM9DS1)       CS_A/G(PB2), SCK(PB13), MISO(PB14), MOSI(PB15),        SPI2
                   CS_M(PB10), INT_M(PB12), INT1_A/G(PA8), INT2_A/G(PA9), 
                   DEN_A/G(PB9), DRDY_M(PB8)
ADC                (PB1)                                                  ADC1_IN9
HEATER             (PA0)                                                  GPIOA_OUT/TIM2_CH1
GPS(MAX-M8Q-0-10)  UART_RX(PB7), UART_TX(PB6)                             USART1
COMPORT            UART_RX(PA3), UART_TX(PA2)                             USART2
USB                DP(PA12), DM(PA11), ID(PA10)                           USB

MCU Peripheral DMA Allocation
- PeriphReq ------ Available(DMAxStreamy) ----- Selected ------------
USART1_RX        (2, 2) / (2, 5)              (2, 5) /Channel4
USART1_TX        (2, 7)                       (2, 7) /Channel4
USART2_RX        (1, 5)                       (1, 5) /Channel4
USART2_TX        (1, 6)                       (1, 6) /Channel4
SPI1_RX          (2, 0) / (2, 2)              (2, 2) /Channel3
SPI1_TX          (2, 3) / (2, 5)              (2, 3) /Channel3
SPI2_RX          (1, 3)                       (1, 3) /Channel0
SPI2_TX          (1, 4)                       (1, 4) /Channel0
ADC1             (2, 0) / (2, 4)              (2, 4) /Channel0

MCU Timer configuration
TIM2 as heater driver
TIM3 as timer counter

