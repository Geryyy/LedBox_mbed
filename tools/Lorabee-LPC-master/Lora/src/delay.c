#include "delay.h"
#include "board.h"

void delay(uint32_t ms)
{
	uint32_t tickRate = Chip_Clock_GetSystemPLLOutClockRate();
	uint32_t i = (tickRate*ms)/1000;

	delayTicks(i);
}

void delayTicks(uint32_t ticks)
{
	while(ticks > 0)
		ticks--;
}
