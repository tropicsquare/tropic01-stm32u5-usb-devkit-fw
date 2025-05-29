#ifndef UART_H
#define UART_H

#include "type.h"
#include "hardware.h"

#if HW_UART1_ON
	#define UART1_RX_BIT  HW_UART1_RX_BIT
	#define UART1_RX_PORT HW_UART1_RX_PORT
	#define UART1_TX_BIT  HW_UART1_TX_BIT
	#define UART1_TX_PORT HW_UART1_TX_PORT
	#define UART1_GPIO_EN HW_UART1_GPIO_EN

	#define UART1_IRQ_USED 			1

	#define UART1_TX_BUF_DEPTH	1024
	int uart1_putchar(const char c);

	#define UART1_RX_BUF_DEPTH	512
	int uart1_getchar(void);

	void uart1_init(u32 baudrate);	
#endif // UART1_ON

#if HW_UART2_ON
	#define UART2_RX_BIT  HW_UART2_RX_BIT
	#define UART2_RX_PORT HW_UART2_RX_PORT
	#define UART2_TX_BIT  HW_UART2_TX_BIT
	#define UART2_TX_PORT HW_UART2_TX_PORT
	#define UART2_GPIO_EN HW_UART2_GPIO_EN

	#define UART2_IRQ_USED 			1

	#define UART2_TX_BUF_DEPTH	1024
	int uart2_putchar(const char c);

	#define UART2_RX_BUF_DEPTH	512
	int uart2_getchar(void);

	void uart2_init(u32 baudrate);	

	void uart2_set_state(bool enabled);
#endif // UART2_ON

#if UART3_ON
	#define UART3_RX_BIT  HW_UART3_RX_BIT
	#define UART3_RX_PORT HW_UART3_RX_PORT
	#define UART3_TX_BIT  HW_UART3_TX_BIT
	#define UART3_TX_PORT HW_UART3_TX_PORT
	#define UART3_GPIO_EN HW_UART3_GPIO_EN

	#define UART3_IRQ_USED 			1

	#define UART3_TX_BUF_DEPTH	1024
	int uart3_putchar(const char c);

	#define UART3_RX_BUF_DEPTH	512
	int uart3_getchar(void);

	void uart3_init(u32 baudrate);	
#endif // UART3_ON

#endif // UART_H
