#ifndef __PERIPHPRIORITYCONFIG_H
#define __PERIPHPRIORITYCONFIG_H

#include "misc.h"

#define SYSTEM_PRIORITY_GROUP_CONFIG             NVIC_PriorityGroup_4
#define SYSTEM_PRIORITY_MIN                      configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define SYSTEM_PRIORITY_MAX                      (0xF)

#define SYSTEM_PRIORITY_CONSTRAINT(x)            (((x) > SYSTEM_PRIORITY_MAX) ? SYSTEM_PRIORITY_MAX : (((x) < SYSTEM_PRIORITY_MIN) ? SYSTEM_PRIORITY_MIN : (x)))

/* IMU SPI DMA INT PRIORITY */
#define INT_PRIORITY_IMU_SPI_DMA_RX              SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0)
#define INT_PRIORITY_IMU_SPI_DMA_TX              SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#define INT_PRIORITY_IMU_INT_PIN                 SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2)

/* u-blox UART TX DMA & RX INT PRIORITY */
#define INT_PRIORITY_UBX_PORT_RX                 SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3)
#define INT_PRIORITY_UBX_PORT_TX_DMA             SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4)

/* USB OTG FS INT PRIORITY */
#define INT_PRIORITY_USB_OTG_FS                  SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 5)

/* COM_PORT UART TX DMA & RX INT PRIORITY */
#define INT_PRIORITY_COM_PORT_RX                 SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 6)
#define INT_PRIORITY_COM_PORT_RX_DMA             SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 6)
#define INT_PRIORITY_COM_PORT_TX_DMA             SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 7)

/* SYSTICK INT PRIORITY */
#define INT_PRIORITY_SYSTICK                     SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 8)

/* SPI FLASH DMA INT PRIORITY */
#define INT_PRIORITY_W25Q_SPI_DMA_TX             SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 9)
#define INT_PRIORITY_W25Q_SPI_DMA_RX             SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 10)

/* BUTTON INT PRIORITY */
#define INT_PRIORITY_BUTTON                      SYSTEM_PRIORITY_CONSTRAINT(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 11)

/* CHIP PERIPHERAL PRIORITY GLOBAL DEFINITIONS */
/* IMU INT1 & INT2 */
#define EXTI9_5_IRQn_Priority                    INT_PRIORITY_IMU_INT_PIN
/* IMU INT MAG & BUTTON */
#define EXTI15_10_IRQn_Priority                  INT_PRIORITY_IMU_INT_PIN
/* COM PORT IDLE INT */
#define USART2_IRQn_Priority                     INT_PRIORITY_COM_PORT_RX
/* COM PORT DMA RX */
#define DMA1_Stream5_IRQn_Priority               INT_PRIORITY_COM_PORT_RX_DMA
/* COM PORT DMA TX */
#define DMA1_Stream6_IRQn_Priority               INT_PRIORITY_COM_PORT_TX_DMA
/* IMU SPI DMA RX */
#define DMA1_Stream3_IRQn_Priority               INT_PRIORITY_IMU_SPI_DMA_RX
/* IMU SPI DMA TX */
#define DMA1_Stream4_IRQn_Priority               INT_PRIORITY_IMU_SPI_DMA_TX

#define DMA2_Stream3_IRQn_Priority               SYSTEM_PRIORITY_MAX

#define DMA2_Stream4_IRQn_Priority               SYSTEM_PRIORITY_MAX

#endif /* __PERIPHPRIORITYCONFIG_H */
