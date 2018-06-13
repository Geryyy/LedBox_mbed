/*
 * @brief LPCXpresso 1125 board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2015
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "retarget.h"
#include "lpc1125_fix.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define BUTTONS_BUTTON1_GPIO_PORT_NUM			1	/* WAKE button */
#define BUTTONS_BUTTON1_GPIO_BIT_NUM			4
#define BUTTONS_BUTTON2_GPIO_PORT_NUM			0	/* ISP button */
#define BUTTONS_BUTTON2_GPIO_BIT_NUM			1

/* Be careful that these pins are set to output before Board_LED_Init()
   is called. */
static const uint8_t ledPin[] = {1, 2, 4};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 10000000;
const uint32_t ExtRateIn = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	Chip_UART_SendBlocking(DEBUG_UART, &ch, 1);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	uint8_t data;

	if (Chip_UART_Read(DEBUG_UART, &data, 1) == 1) {
		return (int) data;
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(const char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* RXD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* TXD */

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(DEBUG_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Initializes board LED(s) */
static void Board_LED_Init(void)
{
#if 0
	uint32_t idx;
	for (idx = 0; idx < (sizeof(ledPin)/sizeof(ledPin[0])); ++idx) {
		/* Set the PIO_7 as output */
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, ledPin[idx]);
		Board_LED_Set(idx, false);
	}
#endif
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if(LEDNumber < (sizeof(ledPin)/sizeof(ledPin[0]))) {
		Chip_GPIO_SetPinState(LPC_GPIO, 1, ledPin[LEDNumber], !On);
	}
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	bool state = false;

	if(LEDNumber < (sizeof(ledPin)/sizeof(ledPin[0]))) {
		state = Chip_GPIO_GetPinState(LPC_GPIO, 1, ledPin[LEDNumber]);
	}

	return state;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	if(LEDNumber < (sizeof(ledPin)/sizeof(ledPin[0]))) {
		Chip_GPIO_SetPinToggle(LPC_GPIO, 1, ledPin[LEDNumber]);
	}
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	//Setup Clock

	if(Chip_Clock_GetMainClockSource() == SYSCON_MAINCLKSRC_PLLOUT)
		Chip_Clock_SetMainClockSource(SYSCON_MAINCLKSRC_IRC);
	Chip_SetupXtalClocking();
	SystemCoreClockUpdate();
	//Init gpio
	Chip_GPIO_Init(LPC_GPIO);
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, LORA_Reset_Port, LORA_Reset_Pin);
	Chip_GPIO_SetPortDIROutput(LPC_GPIO, LORA_CS_Port, LORA_CS_Pin);

	LPC_IOCON_LOC->REG[2] = 0;
	LPC_IOCON_LOC->REG[3] = 0;
	LPC_IOCON_LOC->REG[4] = 0;

	LPC_IOCON_LOC->REG[6] = 1;
}

void Board_SetLoraReset()
{
	Chip_GPIO_SetPortOutHigh(LPC_GPIO, LORA_Reset_Port, LORA_Reset_Pin);
}

void Board_Buttons_Init(void)
{
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM);
}

uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;

	if (Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON1_GPIO_PORT_NUM, BUTTONS_BUTTON1_GPIO_BIT_NUM) == 0) {
		ret |= BUTTONS_BUTTON1;
	}
	if (Chip_GPIO_GetPinState(LPC_GPIO, BUTTONS_BUTTON2_GPIO_PORT_NUM, BUTTONS_BUTTON2_GPIO_BIT_NUM) == 0) {
		ret |= BUTTONS_BUTTON2;
	}

	return ret;
}
