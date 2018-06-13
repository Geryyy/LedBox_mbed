/*
 * settings.h
 *
 * Created: 17.02.2017 15:29:19
 *  Author: Local
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "globals.h"

/****************************Clock Settings***************************/
#ifndef F_CPU
//#define F_CPU 7372800UL
#define F_CPU   8000000UL
#endif

#define SYSTICK_FREQ 1000 //Ticks per Second

/****************************UART Settings***************************/
#define BAUD 9600

#define UART_TRANSMITBUFFER (200)
#define UART_RECEIVEBUFFER (200)

/**************************Uncomment to use smp interface***********/
#ifndef USE_SMP
#define USE_SMP
#endif

#ifdef USE_SMP
#define DEFAULT_HEARTBEAT 1
#endif





#endif /* SETTINGS_H_ */
