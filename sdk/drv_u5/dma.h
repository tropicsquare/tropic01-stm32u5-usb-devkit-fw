#ifndef DMA_H
#define DMA_H

#include "type.h"

void Error_Handler(void);

bool dma_init(void);
void dma_init_spi_rx(void);
void dma_init_spi_tx(void);

void dma_spi_rx_start(char *dest, size_t len);
void dma_spi_tx_start(char *src, size_t len);
bool dma_spi_rx_done(void);
bool dma_spi_tx_done(void);

#endif // ! DMA_H

