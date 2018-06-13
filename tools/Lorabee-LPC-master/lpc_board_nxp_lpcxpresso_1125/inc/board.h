/*
 * @brief NXP LPCXpresso LPC1125 board file
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

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_NXP_LPCXPRESSO_1125 LPCXpresso LPC1125 board support functions
 * @ingroup LPCOPEN_11XX_BOARD_NXP_LPCXPRESSO_1125
 * The board support software API functions provide simple abstracted
 * functions used across multiple LPCOpen board examples. See @ref BOARD_COMMON_API
 * for the functions defined by this board support layer.<br>
 * @{
 */

/** @defgroup BOARD_LPCXPRESSO_1125_OPTIONS BOARD: LPCXpresso LPC1125 board build options
 * This board has options that configure its operation at build-time.<br>
 */

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
	out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
	semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
	Board_UARTPutSTR functions.
 */

/**
 * @}
 */

/* Board name */
#define BOARD_SPACETEAM_LORA

//PIN Defines
#define LORA_Reset_Port			1
#define LORA_Reset_PinNumber 	11
#define LORA_Reset_Pin 			(1<<LORA_Reset_PinNumber)
#define LORA_CS_Port			2
#define LORA_CS_PinNumber		0
#define LORA_CS_Pin				(1<<LORA_CS_PinNumber)

//Interfaces
#define LORA_SSP LPC_SSP1

/**
 * LED defines
 */
#define	LED_RED				0x00
#define	LED_GREEN			0x01
#define	LED_BLUE			0x02

/**
 * BUTTON defines
 */
#define	BUTTONS_BUTTON1		0x01	/* WAKE button */
#define	BUTTONS_BUTTON2		0x02	/* ISP button */

#define	NO_BUTTON_PRESSED	0x00

/**
 * @brief	Initialize buttons on the board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint32_t Buttons_GetStatus(void);

/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
