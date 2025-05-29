#include "platform_setup.h"
#include "gpio.h"

#include <stm32u5xx_ll_bus.h>

const u32 gpio_ll_map[16] = { 
    LL_GPIO_PIN_0, LL_GPIO_PIN_1, LL_GPIO_PIN_2, LL_GPIO_PIN_3,
    LL_GPIO_PIN_4, LL_GPIO_PIN_5, LL_GPIO_PIN_6, LL_GPIO_PIN_7,
    LL_GPIO_PIN_8, LL_GPIO_PIN_9, LL_GPIO_PIN_10, LL_GPIO_PIN_11,
    LL_GPIO_PIN_12, LL_GPIO_PIN_13, LL_GPIO_PIN_14, LL_GPIO_PIN_15
};

u32 gpio_pin_code(u8 pin)
{
    if (pin > 15)
        return (0);

    return (gpio_ll_map[pin]);
}

void gpio_pin_init (gpio_port_t *port, u8 pin, u32 mode)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (pin > 15)
        return;

    GPIO_InitStruct.Pin = gpio_ll_map[pin];
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;

    if (mode == GPIO_MODE_INPUT)
    {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    }
    else if (mode == GPIO_MODE_OUTPUT)
    {
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    }
    else
    {
        return; 
    }
    LL_GPIO_Init(port, &GPIO_InitStruct);
}

void gpio_init (void)
{
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
}

