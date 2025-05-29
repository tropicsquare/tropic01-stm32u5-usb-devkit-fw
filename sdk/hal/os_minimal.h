#ifndef OS_MAIN_H
#define OS_MAIN_H

// no RTOS minimal system setup (i.e. bootloader)

#include "platform_setup.h"
#include "time.h"

#define OS_ASSERT               ASSERT
#define OS_PLATFORM_NAME        PLATFORM_NAME

void tty_put_text(char *text);
#define OS_PUTTEXT(x) tty_put_text(x)
#define OS_PRINTF(...)  printf(__VA_ARGS__)
#define OS_ERROR(msg)  OS_PUTTEXT("ERROR: " msg NL)
#define OS_FATAL(msg)  { OS_PUTTEXT("FATAL ERROR: " msg NL); while(1) ;}

extern void time_delay_ms(u32 ms);
#define OS_DELAY(x) time_delay_ms(x)
extern void time_delay_us(u32 us);
#define OS_DELAY_US(x) time_delay_us(x)

#define OS_TIMER(x) timer_get_time(x)
#define OS_TIMER_MS (TIMER_MS)
#define OS_TIMER_SECOND (1000*TIMER_MS)

#define os_timer_t  timer_time_t 
#define os_timer_get_time timer_get_time 

__inline static bool ret_true(void) {return (true);}

#define OS_SEMAPHORE(x)
#define OS_SEMAPHORE_INIT(x)
#define OS_SEMAPHORE_TAKE(x) ret_true()
#define OS_SEMAPHORE_GIVE(x)
#define OS_TASK_YIELD()
#define OS_FLUSH() fflush(stdout);

#endif // ! OS_MAIN_H

