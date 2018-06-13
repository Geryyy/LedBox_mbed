/*
 * uart.c
 *
 * Created: 17.02.2017 15:28:27
 *  Author: Peter Kremsner
 */ 

#include "uart.h"
//#include "libfifo.h"
#include "settings.h"
#include "chip.h"

//static volatile fifo_t transmitFifo;
//static volatile fifo_t receiveFifo;

//static volatile uint8_t transmitBuffer[GET_BUFFER_SIZE(UART_TRANSMITBUFFER)];
//static volatile uint8_t receiveBuffer[GET_BUFFER_SIZE(UART_RECEIVEBUFFER)];

extern volatile int64_t maintimestamp;

volatile uint32_t lastReceived;

static volatile uint8_t transmitBuffer[256];
static volatile uint8_t receiveBuffer[256];

static volatile RINGBUFF_T transmitFifo;
static volatile RINGBUFF_T receiveFifo;

/************************************************************************/
/* @brief Initialize the UART Interface                                 */
/* @return void															*/
/************************************************************************/
void UART_Init(uartsettings_t* settings)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_7, (IOCON_FUNC2 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_8, (IOCON_FUNC2 | IOCON_MODE_INACT));

	RingBuffer_Init((RINGBUFF_T*)&transmitFifo, (uint8_t*)transmitBuffer, sizeof(uint8_t), sizeof(transmitBuffer));
	RingBuffer_Init((RINGBUFF_T*)&receiveFifo, (uint8_t*)receiveBuffer, sizeof(uint8_t), sizeof(receiveBuffer));
	Chip_UART_Init(LPC_UART0);
	Chip_UART_SetBaudFDR(LPC_UART0,settings->baudrate);
	Chip_UART_ConfigData(LPC_UART0,(UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS));
	Chip_UART_SetupFIFOS(LPC_UART0, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART0);

	Chip_UART_IntEnable(LPC_UART0, (UART_IER_RBRINT | UART_IER_RLSINT));

	NVIC_SetPriority(UART0_IRQn, 1);
	NVIC_EnableIRQ(UART0_IRQn);

	//fifo_init((fifo_t*)&transmitFifo,(uint8_t*)transmitBuffer,sizeof(transmitBuffer));
	//fifo_init((fifo_t*)&receiveFifo,(uint8_t*)receiveBuffer,sizeof(receiveBuffer));
}

/*************************************************************************/
/* This function sends a string to the uart interface and returns		 */
/* the number of bytes that where actually written.                      */
/* @brief Send string to UART interface									 */
/* @return uint8_t                                                       */
/*************************************************************************/
uint32_t UART_SendString(const char* string)
{
	uint32_t i = 0;
	while(string[i] != '\0')
	{
		i++;
	}
	if(i > 0)
	{
		return UART_Send((const uint8_t*)string,i);
	}
	else
		return 0;
}

/*************************************************************************/
/* This function sends length bytes to the Uart interface and returns the*/
/* number of bytes that where actually written.                          */
/* @brief Send Bytes to UART interface									 */
/* @return uint8_t                                                       */
/*************************************************************************/
uint32_t UART_Send(const uint8_t* data, uint32_t length)
{
	uint32_t ret;
	//ret = fifo_write_bytes(data,(fifo_t*)&transmitFifo,length);
	//Interrupt aktivieren
	ret = Chip_UART_SendRB(LPC_UART0, (RINGBUFF_T*)&transmitFifo, data, length);
	return ret;
}

/************************************************************************/
/* @brief Returns the number of bytes that can be read from the uart    */
/* @return uint8_t number of bytes                                      */
/************************************************************************/
uint32_t UART_Available()
{
	uint32_t ret;
	//ret = fifo_datasize((fifo_t*)&receiveFifo);
	ret = RingBuffer_GetCount((RINGBUFF_T*)&receiveFifo);
	return ret;
}

/************************************************************************/
/* This function reads up to length bytes from the uart interface       */
/* and returns the number of bytes that where actually read             */
/* @brief Reads length bytes from the interface                         */
/************************************************************************/
uint32_t UART_Read(uint8_t* data, uint32_t length)
{
	uint32_t ret;
	//ret = fifo_read_bytes(data,(fifo_t*)&receiveFifo,length);
	ret = Chip_UART_ReadRB(LPC_UART0, (RINGBUFF_T*)&receiveFifo, data, length);
	return ret;
}

/**
uint8_t UART_Peak(uint8_t* data, uint8_t length)
{
	uint8_t ret;
	ret = fifo_peak_bytes(data,(fifo_t*)&receiveFifo,length);
	return ret;
}**/

void UART_delete(uint32_t length)
{
	//fifo_delete_bytes(length,(fifo_t*)&receiveFifo);
	RingBuffer_Flush((RINGBUFF_T*)&receiveFifo);
}

void UART_IRQHandler (void)
{
	Chip_UART_IRQRBHandler(LPC_UART0, (RINGBUFF_T*)&receiveFifo, (RINGBUFF_T*)&transmitFifo);
	/*
	if(LPC_UART0->IIR & UART_IIR_INTID_RDA)
	{
		Chip_UART_RXIntHandlerRB(LPC_UART0,(RINGBUFF_T*)&receiveFifo);
	}

	if(LPC_UART0->IIR & UART_IIR_INTID_THRE)
	{
		Chip_UART_TXIntHandlerRB(LPC_UART0, (RINGBUFF_T*)&transmitFifo);
	}*/
}

/*
ISR(USART_UDRE_vect)
{
	uint8_t data;
	if(fifo_read_byte(&data,(fifo_t*)&transmitFifo))
	{
		UDR0 = data;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
	}
}


ISR(USART_RX_vect)
{
	fifo_write_byte(UDR0,(fifo_t*)&receiveFifo);
}*/
