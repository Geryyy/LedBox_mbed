#ifndef LORALIB_H
#define LORALIB_H

//Entspricht Ausg�ngen am Arduinon Ausgang
#define LORA_DEFAULT_SS_PIN    10
#define LORA_DEFAULT_RESET_PIN 9 
#define LORA_DEFAULT_DIO0_PIN  8 //ursprgl pin 2 am arduino ausgang 

#include <stdint.h>
#include "board.h"



typedef struct
{
	void (*receivecallback)(void);
	void (*transmitCompleteClb)(void);
	uint32_t frequency;
	uint8_t txPower;
	uint8_t spreadingFactor;
	uint32_t signalBandwith;
	uint8_t codingRateDenominator;
	uint16_t preambleLength;
	uint8_t syncword;
	uint8_t crc;
	uint8_t messageSize;
}loraSettings_t;

typedef enum
{
		idle,
		send,
		receive,
		sleep,
		disconnected
}loraStatus_e;

void lora_init(loraSettings_t* settings);
void lora_reset();
void lora_deinit();
uint8_t lora_readVersion();
uint8_t lora_ready();

loraStatus_e lora_getStatus();
void lora_setMessageSize(uint8_t size);
uint8_t lora_getMessageSize();
void lora_onReceive(void (*callback)(void));
void lora_setReceive();
void lora_setIdle();
void lora_setSleep();
void lora_setTxPower(uint8_t level);
void lora_setFrequency(uint32_t frequency);
void lora_setSpreadingFactor(uint8_t sf);
void lora_setSignalBandwidth(uint32_t sbw);
void lora_setCodingRate4(uint8_t denominator);
void lora_setPreambleLength(uint16_t length);
void lora_setSyncWord(uint8_t sw);
void lora_crc();
void lora_noCrc();
void lora_implicitHeaderMode();
void lora_explicitHeaderMode();
uint8_t lora_getMode();

uint8_t lora_parsePacket();
uint8_t lora_packetRssi();
float lora_packetSnr();
uint8_t lora_available();
int16_t lora_read();
uint16_t lora_readBytes(uint8_t* buffer, uint16_t length);
uint8_t lora_sendBytes(const uint8_t* buffer, uint8_t length);
int16_t lora_peek();
uint8_t lora_random();
int8_t lora_getTemperature();
uint8_t lora_locked();

void lora_poll();


#endif
