#include "common.h"
#include "reset.h"
#include "gpreg.h"

reset_type_e reset_get_type(void)
{
	reset_type_e reset_type = RESET_POWER_ON;

	u32 reset_source = (RCC->CSR & 0xFE000000UL);

	if (GPREG_WDID == GPREG_WDID_REBOOT_RQ)
	{
		reset_type=RESET_USER_RQ;
	}
	else if (reset_source & (RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF))
	{
		reset_type=RESET_WDT;
	}	
	else if (reset_source & (RCC_CSR_LPWRRSTF))
	{
		reset_type=RESET_BOD;
	}

	return (reset_type);
}

void reset_clear(void)
{
	RCC->CSR |= RCC_CSR_RMVF;
	GPREG_WRITE(GPREG_WDID, 0);
}
