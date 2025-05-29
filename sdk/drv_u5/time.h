#ifndef _TIME_H_INCLUDED
#define _TIME_H_INCLUDED

#include	"hardware.h"

#define TIMER_MS	1000UL
#define TIMER_US	1UL

#define	TIMER2_ON 1
#define	TIMER3_ON 0
#define	TIMER5_ON 0

typedef u64 timer_time_t;

void timer_init(void);
timer_time_t timer_get_time(void);
timer_time_t timer_get_time_irq(void);

void timer3_init(void);
static inline void timer3_run(void) { TIM3->CR1 |= TIM_CR1_CEN; }
static inline void timer3_stop(void)  { TIM3->CR1 &= ~TIM_CR1_CEN; }
static inline void timer3_reset(void) { TIM3->CNT = 0; }
static inline u32 timer3_get_time(void) { return (TIM3->CNT); }

void timer5_free_run(void);
#define timer5_get_time() (TIM5->CNT)

void time_delay_ms(u32 delay);
void time_delay_us(u32 tm);

#endif/*_TIME_H_INCLUDED*/
