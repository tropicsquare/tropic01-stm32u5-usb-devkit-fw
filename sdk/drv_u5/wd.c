#include "common.h"
#include "gpreg.h"

#include "wd.h"

#if (MAIN_DEBUG == 1)
  #warning "WD disabled" 
  #define WD_ACTIVE 	0
#else
  #define WD_ACTIVE 	1
#endif // ~MAIN_DEBUG

#define IWDG_KEY_RELOAD                 0x0000AAAAU  // IWDG Reload Counter Enable
#define IWDG_KEY_ENABLE                 0x0000CCCCU  // IWDG Peripheral Enable
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555U  // IWDG KR Write Access Enable
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000U  // IWDG KR Write Access Disable

#define WD_TIMEOUT		300 // [8ms] LSI

static bool _wd_clr_enable = true;


static void _wd_start (void)
{
	IWDG->KR = IWDG_KEY_ENABLE;
	IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;
	IWDG->PR = 6; // LSI divide by 256
	IWDG->RLR = WD_TIMEOUT;

	while (IWDG->SR != 0)
		;

	IWDG->KR=IWDG_KEY_RELOAD;
}

void wd_init(void)
{
}

void wd_run(void)
{
#if (WD_ACTIVE == 1)
	_wd_start();
#endif // WD_ACTIVE
}

void wd_reset (u32 reason)
{
	GPREG_WRITE(GPREG_WDID, GPREG_WDID_REBOOT_RQ);
	GPREG_WRITE(GPREG_BOOT, reason);

#if (WD_ACTIVE != 1)
	// wd is not active, enable it 
	_wd_start();
#endif // WD_ACTIVE != 1

	IWDG->KR=IWDG_KEY_WRITE_ACCESS_ENABLE;
	IWDG->PR  = 1; // 
	IWDG->RLR = 1; // as fast as possible
	while (IWDG->SR != 0)
		;
	IWDG->KR=IWDG_KEY_RELOAD;
	while (1)
	{
		_wd_clr_enable=false;
	}
}

void wd_disable (void)
{
	_wd_clr_enable=false;
}


void wd_feed (void)
{
	if (_wd_clr_enable == true)
	{
		IWDG->KR=IWDG_KEY_RELOAD;
	}
}
