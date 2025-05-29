#include "platform_setup.h"
#include "hardware.h"
#include "time.h"

static u64 timer_ms = 0;


#if TIMER2_ON 

void TIM2_IRQHandler(void)
{
    // Check whether update interrupt is pending
    if (TIM2->SR & TIM_SR_UIF)
    {   // Clear the update interrupt flag
        TIM2->SR &= ~(TIM_SR_UIF);
        timer_ms++;
    }
}

void timer_init(void)
{
    // enable clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // reset
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM2RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM2RST;

    // Set the Autoreload value
    TIM2->ARR = 1000;

    // Set the Prescaler value
    TIM2->PSC = (TIM2CLK/1000000)-1; // [us]

    TIM2->CR1 = TIM_CR1_CEN; // Counter enable

    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    // Generate an update event to reload the Prescaler value immediatly
    TIM2->EGR = TIM_EGR_UG;    //1
}

static inline timer_time_t _get_timer_time(void)
{
    u32 t;

    t = TIM2->CNT;
    if (t < 100)
    {   // there is small probability there is pending IRQ
        if (TIM2->SR & TIM_SR_UIF)
            t += 1000; // fix missing IRQ, which will execute later
    }
    return (timer_ms * (u64)1000 + t);
}

timer_time_t timer_get_time(void)
{
    timer_time_t t;

    __disable_irq();
    t = _get_timer_time();
    __enable_irq();

    return (t);
}

timer_time_t timer_get_time_irq(void)
{   // may be called from IRQ (or when already disabled IRQ)
    // be sure no any timer IRQ appears here
    TIM2_IRQHandler(); // force update ms timer
    return (_get_timer_time());
}
#endif // TIMER2_ON 

void time_delay_ms(u32 tm)
{
    tm *= TIMER_MS;

    timer_time_t time = timer_get_time();
    
    while ((timer_time_t)(timer_get_time() - time) < tm)
        ;

/*  timer_time_t time = timer_get_time() + delay * TIMER_MS;

    while (timer_get_time() < time)
        ;*/
}

void time_delay_us (u32 tm)
{   // 
    tm *= TIMER_US;

    timer_time_t time = timer_get_time();
    
    while ((timer_time_t)(timer_get_time() - time) < tm)
        ;
}

void timer3_init(void)
{
    // enable clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

    // reset
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM3RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM3RST;

    // Set the Autoreload value
    TIM3->ARR = UINT16_MAX;

    // Set the Prescaler value
    TIM3->PSC = (TIM3CLK/1000000)-1; // [us]

    TIM3->CR1 = TIM_CR1_OPM; // one pulse mode
    // TIM3->CR1 |= TIM_CR1_CEN; // dont enable counter

    // Generate an update event to reload the Prescaler value immediatly
    TIM3->EGR = TIM_EGR_UG;
}

