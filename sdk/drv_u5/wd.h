#ifndef WD_H
#define WD_H

#include "type.h"

void wd_init (void);
void wd_run (void);
void wd_reset (u32 reason);
void wd_feed (void);
void wd_disable (void);

#endif // ! WD_H
