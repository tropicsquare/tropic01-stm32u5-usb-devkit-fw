#ifndef GPIO_H
#define	GPIO_H

#include "stm32u5xx_ll_gpio.h"

typedef GPIO_TypeDef gpio_port_t;

extern const u32 gpio_ll_map[];

u32 gpio_pin_code(u8 pin);

void gpio_pin_init (gpio_port_t *port, u8 pin, u32 mode);

#define	GPIO_PIN_INIT(port,pin,mode) gpio_pin_init(port,pin,mode)

#define	GPIO_MODE_INPUT            (LL_GPIO_MODE_INPUT)
#define	GPIO_MODE_OUTPUT           (LL_GPIO_MODE_OUTPUT)
#define	GPIO_MODE_ALT              (LL_GPIO_MODE_ALTERNATE)
// #define	GPIO_MODE_FLOATING         (LL_GPIO_MODE_FLOATING)

#define	GPIO_MODE(p,bit,mode) LL_GPIO_SetPinMode(p, gpio_ll_map[bit], mode)

#define	GPIO_PUPDN_UP              (LL_GPIO_PULL_UP)
#define	GPIO_PUPDN_DN              (LL_GPIO_PULL_DOWN)

#define	GPIO_PUPDN(p,bit,mode) LL_GPIO_SetPinPull(p, gpio_ll_map[bit], mode)

/*
#define GPIO_SPEED_LOW            (LL_GPIO_MODE_OUTPUT_2MHz)  // IO works at 2 MHz
#define GPIO_SPEED_MEDIUM         (LL_GPIO_MODE_OUTPUT_10MHz) // range 12,5 MHz to 50 MHz
#define GPIO_SPEED_HIGH           (LL_GPIO_MODE_OUTPUT_50MHz) // range 25 MHz to 100 MHz
#define GPIO_SPEED_VERY_HIGH      (LL_GPIO_MODE_OUTPUT_50MHz) // range 50 MHz to 200 MHz
*/

#define	GPIO_SPEED(p,bit,mode)  LL_GPIO_SetPinSpeed(p, gpio_ll_map[bit], mode)

#define	GPIO_OUTPUT_TYPE_PUSHPULL  (0)
#define	GPIO_OUTPUT_TYPE_OPENDRAIN (1)

#define	GPIO_OUTPUT_TYPE(p,bit,mode) LL_GPIO_SetPinOutputType(p, gpio_ll_map[bit], mode)


// GPIO control
#define	GPIO_BIT_CLR(p,bit)      p->BRR =  (1U<<(bit))
#define	GPIO_BIT_SET(p,bit)      p->BSRR = (1U<<(bit))

#define	GPIO_IN(p,bit)	(p->IDR & (1<<(bit)))

//	GPIOx->IDR == input data gerister
//	GPIOx->ODR == output data gerister

extern void gpio_init (void);

#endif // ~GPIO_H

