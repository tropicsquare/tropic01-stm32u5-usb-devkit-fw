#ifndef _SPI_H
#define _SPI_H

#include "hardware.h"

//SPIx_ON should be set in pcb_xxxxxxx.h 

#ifndef SPI1_ON
  #define SPI1_ON 0
#endif

#ifndef SPI2_ON
  #define SPI2_ON 0
#endif

#define SPI_CS_ACTIVE true
#define SPI_CS_IDLE   false

#if SPI1_ON 

  void spi1_init (void);
  u32 spi1_get_prescaler(void);
  u32 spi1_get_frequency(void);
  bool spi1_set_frequency(u32 freq);
  bool spi1_set_prescaler(u32 value);
  void spi1_data_transfer(u8 *rx, u8 *tx, size_t len);
  void spi1_flush(void);
  u8 spi1_transfer(u8 c);
  bool spi1_cs_state(void);
  void spi1_cs(bool state);

#endif // SPI1_ON

#if SPI2_ON 

  void spi2_init (void);
  u8 spi2_transfer(u8 c);
  void spi2_cs(bool state);

#endif // SPI2_ON

#endif //_SPI_H

