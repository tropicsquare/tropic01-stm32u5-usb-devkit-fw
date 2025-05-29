#ifndef HARDWARE_H
#define HARDWARE_H

#include "stm32u5xx.h"

#include "platform_setup.h"

#ifndef HW_NAME

  // default HW name
  #define	HW_NAME "TS1302"

#endif // not defined HW_NAME

// #include "pcb_nucleo.h"
// #include "pcb_stlinkv2.h"
// #include "pcb_stm32_devel_board.h"
// #include "pcb_ts0101.h"
#include "pcb_ts1302.h"

#define HW_SPI_SW_CPOL 0
#define HW_SPI_SW_CPHA 0

#define SYSCLK                  72000000
#define HCLK                    SYSCLK
#define PCLK1                   (HCLK/2)
#define APB1CLK                 (PCLK1/1)
#define TIM2CLK                 HCLK
#define TIM3CLK                 HCLK

#define DEF_PRIO                7
#define ADC_ISR_PRIO            5
#define UART1_ISR_PRIO          DEF_PRIO
#define UART2_ISR_PRIO          DEF_PRIO
#define UART3_ISR_PRIO          DEF_PRIO
#define USB_ISR_PRIO            DEF_PRIO
#define SPI_ISR_PRIO            DEF_PRIO

#define UART1CLK                PCLK1
#define UART2CLK                PCLK1
#define UART3CLK                PCLK1

#endif // ! HARDWARE_H

