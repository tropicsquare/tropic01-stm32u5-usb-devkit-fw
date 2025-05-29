#ifndef MAIN_H
#define MAIN_H

#include "common.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

extern bool main_spi_auto;
extern u8 main_spi_get_resp;
extern u8 main_spi_no_resp;

void Error_Handler(void);

#endif // MAIN_H
