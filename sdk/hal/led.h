#ifndef LED_H
#define LED_H

#include	"type.h"

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

typedef void (*pfunc_led_action) (void);

typedef struct {
	u32 mask_run;
	u32 mask;
	pfunc_led_action on;
	pfunc_led_action off;
	u8  count;
	u8  prio;
	u8  length; // max 32
} led_t;

bool led_init(led_t *led);
void led_instant_sequence (led_t *led, u32 new_mask, u8 length );
void led_cyclic_sequence  (led_t *led, u32 new_mask, u8 length , u8 prio);
void led_on (led_t *led);
void led_off (led_t *led);
void led_tick (led_t *led);

#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// LED_H

