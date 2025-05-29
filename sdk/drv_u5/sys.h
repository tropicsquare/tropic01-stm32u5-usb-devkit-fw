#ifndef SYS_H
#define	SYS_H

#include "common.h"

#define SYS_HCLK_MIN 40000000
#define SYS_HCLK_MAX 80000000

void sys_init(void);
void sys_clock_config(void);
void sys_usb_clock_config(void);
u32  sys_get_hclk(void); // [Hz]
bool sys_set_hclk(u32 freq); // [Hz]
u32  sys_flash_size(void);


#endif // ! SYS_H

