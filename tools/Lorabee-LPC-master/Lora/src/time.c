
#include "time.h"
#include "chip.h"
#include "settings.h"

static uint32_t currentTime;

void time_init()
{
	SysTick_Config(Chip_Clock_GetSystemClockRate()/SYSTICK_FREQ);
	currentTime = 0;
	NVIC_EnableIRQ(SysTick_IRQn);
}

uint32_t time_now()
{
	return currentTime;
}

void SysTick_Handler(void)
{
	currentTime++;
}
