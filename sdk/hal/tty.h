#ifndef TTY_H
#define	TTY_H

#include "type.h"
#include "hardware.h"
#include "uart.h"

#define	TTY_BUF_SIZE (1024)

#ifdef HW_CONSOLE_ON_UART
 	#define TTY_ON_UART
 	#define	TTY_UART_GETCHAR()   HW_TTY_UART_GETCHAR()
 	#define	TTY_UART_PUTCHAR(ch) HW_TTY_UART_PUTCHAR(ch)
 	#define	TTY_UART_INIT(baud)  HW_TTY_UART_INIT(baud)
#endif

#ifdef TTY_ON_UART
	int putChar (int c);
	extern volatile unsigned char last_sent_char;
#else
 	#define	TTY_UART_GETCHAR() (-1)
 	__inline bool TTY_UART_PUTCHAR(char ch){return (true);}
 	#define	TTY_UART_INIT(baud)
#endif

typedef void (*tty_parse_callback_t) (char *data);

bool tty_init(tty_parse_callback_t callback);
void tty_put_binary(u8 *data, size_t len);
void tty_put_text(char *text);
void tty_rx_task(void);

#endif // ! TTY_H

