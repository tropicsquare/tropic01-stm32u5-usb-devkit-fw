#include "os.h"
#include "led.h"

bool led_init (led_t *led)
{
	bzero(led, sizeof(led_t));

	led->mask = 0;
	led->mask_run = 0 ;

	led->on  = NULL;
	led->off = NULL;

	return (true);
}

void led_cyclic_sequence(led_t *led, u32 new_mask, u8 length, u8 prio)
{
	if (led->mask == new_mask)
		return;
	
	if (led->mask)
	{	// 
		if (prio<led->prio)
			return; // signalizace neceho s vyssi prioritou

	}
	length--;
    if (length > 31)
		length = 31;

	led->length=length;
	led->mask = new_mask;
	led->prio = prio;
	led->count = 0;
}

void led_instant_sequence(led_t *led, u32 new_mask, u8 length)
{
	length--;
    if (length > 31)
		length = 31;

  	led->count=length;
  	led->mask_run=new_mask;
}

void led_tick (led_t *led)
{
	if (led->mask_run & 1)
	{
		if (led->on  != NULL)
		{
			led->on();
		}
	}
	else
	{
		if (led->off != NULL)
		{
			led->off();
		}
	}

	led->mask_run >>=1;
	if (led->count>0) led->count--;
	else
	{
		led->count=led->length;
		led->mask_run=led->mask;
	}
}

