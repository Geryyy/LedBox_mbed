/*
 * lorainterface.h
 *
 * Created: 16.03.2017 23:22:47
 *  Author: Local
 */ 


#ifndef LORAINTERFACE_H_
#define LORAINTERFACE_H_

#include <stdint.h>
#include "libfifo.h"

typedef uint32_t (*dataReady)(const uint8_t* data, uint32_t size);

typedef struct
{
	int sending :1;
	int powersave :1;
}lora_status_t;

void radio_init(dataReady dataReadyCallback);
uint8_t radio_work();
uint32_t radio_send(uint8_t* buffer, uint32_t length);
uint32_t radio_readyToSend();
uint8_t radio_sending();





#endif /* LORAINTERFACE_H_ */
