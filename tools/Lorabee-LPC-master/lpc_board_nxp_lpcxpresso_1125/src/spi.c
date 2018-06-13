/*
 * spi.c
 *
 *  Created on: 25 Apr 2018
 *      Author: Peter Kremsner
 */

#include "board.h"
#include "chip.h"

void spi_init()
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 3);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 2, 1);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 2, 2);

	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_1, (IOCON_FUNC2 | IOCON_MODE_INACT)); /* SCK1 */

	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_2, (IOCON_FUNC2 | IOCON_MODE_INACT)); /* MISO1 */

	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_3, (IOCON_FUNC2 | IOCON_MODE_INACT)); /* MOSI1 */

	Chip_SSP_Init(LORA_SSP);
	Chip_SSP_SetFormat(LORA_SSP, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE0);
	Chip_SSP_SetMaster(LORA_SSP, true);
	Chip_SSP_SetBitRate(LORA_SSP, 1000000);
	Chip_SSP_Enable(LORA_SSP);
}

uint8_t spi_transmit(uint8_t data)
{
	LORA_SSP->DR = data;
	while(Chip_SSP_GetStatus(LORA_SSP, SSP_STAT_BSY) == SET);
	return LORA_SSP->DR;
}

int spi_transmitBlock(void* recv, void* trans, int length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		if(recv != 0)
		{
			((uint8_t*)recv)[i] = spi_transmit(((uint8_t*)trans)[i]);
		}
		else
		{
			spi_transmit(((uint8_t*)trans)[i]);
		}
	}

	return i;
}
