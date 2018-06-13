/*
 * lorainterface.c
 *
 * Created: 16.03.2017 23:22:35
 *  Author: Local
 */

#include "lorainterface.h"
#include "lora/loralib.h"
#include "fifo/libfifo.h"
#include "globals.h"
#include "board.h"
#include "time.h"
#include "string.h"

#define BLOCK_SEND_TIMEOUT 200

static dataReady callback;

volatile lora_status_t lora_status;

static uint8_t sendBuffer[GET_BUFFER_SIZE(LORA_PACKET_LENGTH * 10)];
static fifo_t sendFifo;

static uint32_t beginnToWaitTimestamp;

static void lora_dataReadyCallback(void) {
	int dataSize;
	uint8_t data[LORA_PACKET_LENGTH];
	dataSize = lora_readBytes(data, LORA_PACKET_LENGTH);
	callback(data, dataSize);
}

static void lora_transmitComplete(void)
{
	if(fifo_datasize(&sendFifo) < LORA_PACKET_LENGTH)
		lora_setReceive(); //Goto receive after transmition
}

void radio_init(dataReady dataReadyCallback) {
	loraSettings_t lora_settings;

	memset(&lora_settings, 0, sizeof(loraSettings_t));

	lora_settings.codingRateDenominator = LORA_CODING_RATE;
	lora_settings.crc = LORA_CRC;
	lora_settings.frequency = LORA_FREQUENCY;
	lora_settings.preambleLength = LORA_PREAMBLE_LENGTH;
	lora_settings.receivecallback = lora_dataReadyCallback;
	lora_settings.signalBandwith = LORA_SIGNAL_BANDWIDTH;
	lora_settings.spreadingFactor = LORA_SPREADING_FACTOR;
	lora_settings.syncword = LORA_SYNC_WORD;
	lora_settings.txPower = LORA_TX_POWER;
	lora_settings.messageSize = LORA_PACKET_LENGTH;
	lora_settings.transmitCompleteClb = lora_transmitComplete;
	lora_init(&lora_settings);

	fifo_init(&sendFifo, sendBuffer, sizeof(sendBuffer));

	callback = dataReadyCallback;
}

uint8_t radio_work() {
	uint8_t loraPacket[LORA_PACKET_LENGTH];
	uint8_t ret = 1;

	lora_poll();

	if (lora_getStatus() == send)
		return 1;

	uint8_t i;
	uint32_t dataAvailable = fifo_datasize(&sendFifo);
	uint32_t dataRead;
	if (dataAvailable > 0) {
		if (dataAvailable >= LORA_PACKET_LENGTH) {
			ret = 0;
			dataRead = fifo_read_bytes(loraPacket, &sendFifo, LORA_PACKET_LENGTH);
			lora_sendBytes(loraPacket, LORA_PACKET_LENGTH);
		}
		else
		{
			if(beginnToWaitTimestamp == 0)
			{
				beginnToWaitTimestamp = time_now();
			}
			else if(time_elapsedOffset(beginnToWaitTimestamp, BLOCK_SEND_TIMEOUT))
			{
				beginnToWaitTimestamp = 0;
				dataRead = fifo_read_bytes(loraPacket, &sendFifo, LORA_PACKET_LENGTH);
				for (i = dataRead; i < sizeof(loraPacket); i++) {
					loraPacket[i] = 0;
				}
				lora_sendBytes(loraPacket, LORA_PACKET_LENGTH);
			}
		}
	}
	return ret;
}

uint32_t radio_send(uint8_t* buffer, uint32_t length) {
	uint8_t ret = 0;
	ret = fifo_write_bytes(buffer, &sendFifo, length);
	return ret;
}

uint32_t radio_readyToSend()
{
	uint32_t ret;
	ret = fifo_free_space(&sendFifo);
	return ret;
}
