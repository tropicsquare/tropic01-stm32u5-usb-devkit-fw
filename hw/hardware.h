#ifndef HARDWARE_H
#define HARDWARE_H

#include "stm32u5xx.h"

#include "platform_setup.h"

#ifndef HW_NAME

  // default HW name
  #define	HW_NAME "TS1302"

#endif // not defined HW_NAME

#include "pcb_ts1302.h"

#define HW_SPI_SW_CPOL 0
#define HW_SPI_SW_CPHA 0

#define DEF_PRIO                7
#define ADC_ISR_PRIO            5
#define UART1_ISR_PRIO          DEF_PRIO
#define USB_ISR_PRIO            DEF_PRIO
#define SPI_ISR_PRIO            DEF_PRIO

#endif // ! HARDWARE_H

