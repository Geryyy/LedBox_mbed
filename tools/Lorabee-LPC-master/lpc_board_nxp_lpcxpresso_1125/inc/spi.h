#ifndef SPI_H__
#define SPI_H__

#include "chip.h"

void spi_init();
uint8_t spi_transmit(uint8_t data);
int spi_transmitBlock(void* recv, void* trans, int length);

#endif
