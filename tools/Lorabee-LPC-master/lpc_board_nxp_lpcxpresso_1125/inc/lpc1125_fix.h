/*
 * lpc1125_fix.h
 *
 *  Created on: 28 Mar 2018
 *      Author: Peter Kremsner
 */

#ifndef LPC1125_FIX_H_
#define LPC1125_FIX_H_

#include "chip.h"

typedef struct
{
	__IO uint32_t REG [7];
}LPC_IOCON_LOC_T;

#define LPC_IOCON_LOC_BASE 0x400440CC
#define LPC_IOCON_LOC ((LPC_IOCON_LOC_T *) LPC_IOCON_LOC_BASE)


#endif /* LPC1125_FIX_H_ */
