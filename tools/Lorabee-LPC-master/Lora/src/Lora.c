/*
 ===============================================================================
 Name        : Lora.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include "lora/loralib.h"
#include "lorainterface.h"
#include "UART/uart.h"
#include "globals.h"
#include "smp/libsmp.h"
#include "time.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

volatile int64_t maintimestamp = -1;
volatile int64_t lastTempTimestamp = 0;
int8_t loraTemp;

#ifdef USE_SMP
static smp_struct_t uart_smp;
static uint8_t smpBuffer[256];
static fifo_t uartSmpFifo;

static signed char uartReceived(fifo_t* fifo)
{
	uint32_t readyToSend;
	uint8_t buffer[sizeof(smpBuffer)/sizeof(uint8_t)];
	uint8_t* ptr;
	uint8_t port;
	if(fifo_read_byte(&port, fifo))
	{
		switch(port)
		{
			default:
			readyToSend = SMP_Send(&fifo->buffer[fifo->head], fifo_datasize(fifo), buffer, sizeof(buffer), &ptr); //Repack data
			while(readyToSend > 0)
			{
				readyToSend -= radio_send(buffer,readyToSend);
			}
			break;
		}
	}

	return 0;
}
#endif

int main(void) {
#if defined (__USE_LPCOPEN)
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	Board_Init();
#endif
#endif

	uint32_t available;
	uint8_t intermediateBuffer[100];
	uint8_t sleep = 1;

	time_init();

	uartsettings_t settings;
	settings.doubleBaudRate = 0;
	settings.baudrate = UART_BAUD_RATE;
	settings.databits = Eight_Bit;
	settings.parity = disabled;
	settings.stopbits = One_Bit;
#ifdef USE_SMP
	fifo_init(&uartSmpFifo,smpBuffer,sizeof(smpBuffer));

	memset(&uart_smp,0,sizeof(uart_smp));
	uart_smp.frameReadyCallback = uartReceived;
	SMP_Init(&uart_smp);
#endif

	UART_Init(&settings);
	radio_init(UART_Send);
	UART_SendString("Lora Version 1.0");

	lora_readVersion();
	lora_setReceive();
	/*
	 char msg[60];
	 sprintf(msg,"Lora Module Versionnumber: %d\n",lora_readVersion());
	 UART_SendString(msg);
	 */
	while (42) {
		sleep = 1;
		available = UART_Available();
#ifndef USE_SMP
		uint32_t bytesToWrite = radio_readyToSend();
		if (available > 0 && bytesToWrite > 0) {
			available = UART_Read(intermediateBuffer,
					MIN(bytesToWrite, sizeof(intermediateBuffer)));
			while (available > 0) {
				available -= radio_send(intermediateBuffer, available);
			}
		} else {
			maintimestamp = -1;
		}
#else
		if(available > 0)
		{
			available = UART_Read(intermediateBuffer, sizeof(intermediateBuffer));
			SMP_RecieveInBytes(intermediateBuffer, available, &uart_smp);
		}
#endif

		if (!radio_work())
			sleep = 0;

		if (sleep) {

		}
	}
	return 0;
}
