/*
 * uart.h
 *
 * Created: 17.02.2017 15:32:34
 *  Author: Local
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "board.h"

#define UART_THRE_INTERRUPT (1<<1)

typedef enum
{
	disabled = 0,
	even = 2,
	odd	
}parity_e;

typedef enum
{
	One_Bit = 0,
	Two_Bit
}stopbits_e;

typedef enum
{
	Five_Bit = 0,
	Six_Bit,
	Seven_Bit,
	Eight_Bit,
	Nine_Bit = 7
}databits_e;

typedef struct  
{
	uint8_t doubleBaudRate;
	uint32_t baudrate;
	parity_e parity;
	stopbits_e stopbits;
	databits_e databits;
}uartsettings_t;

void UART_Init(uartsettings_t* settings);
uint32_t UART_SendString(const char* string);
uint32_t UART_Send(const uint8_t* data, uint32_t length);
uint32_t UART_Available();
uint32_t UART_Read(uint8_t* data, uint32_t length);
/**
uint8_t UART_Peak(uint8_t* data, uint8_t length);
**/
void UART_delete(uint32_t length);

extern volatile uint32_t lastReceived;


#endif /* UART_H_ */
