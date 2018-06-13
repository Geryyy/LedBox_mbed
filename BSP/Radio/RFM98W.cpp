#include "RFM98W.h"


RFM98W::RFM98W() : Radio()
{
    /* todo: implement */
}


int RFM98W::serviceRadio(){
    /* todo: implement */
    return 0;
}

int RFM98W::sendBytes(unsigned char *data, int len){
    /* todo: implement */
    return 0;
}


//###########################PUBLIC FUNCTIONS##############################################

void RFM98W::lora_reset()
{
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_Reset_Port, LORA_Reset_PinNumber, false);
	// delay(10);
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_Reset_Port, LORA_Reset_PinNumber, true);
	// delay(10);
}

uint8_t RFM98W::lora_readVersion()
{
	uint8_t reg;
	reg = lora_readRegister(REG_VERSION);
	return reg;
}

void RFM98W::lora_init(loraSettings_t* settings)
{

	loraLocked = 0;

	// Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 9);

	spi_init();

	// set SS high
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_CS_Port, LORA_CS_PinNumber, true);
	lora_reset();

	// start SPI
	lora_setSleep();
	lora_setFrequency(settings->frequency);
	lora_writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
	lora_writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
	lora_writeRegister(REG_LNA, lora_readRegister(REG_LNA) | 0x03);
	lora_setTxPower(settings->txPower);
	lora_setSpreadingFactor(settings->spreadingFactor);
	lora_setCodingRate4(settings->codingRateDenominator);
	lora_setSignalBandwidth(settings->signalBandwith);
	lora_setPreambleLength(settings->preambleLength);
	lora_setSyncWord(settings->syncword);
	lora_setMessageSize(settings->messageSize);
	if (settings->crc)
	{
		lora_crc();
	}
	else
	{
		lora_noCrc();
	}
	if (settings->receivecallback)
		lora_onReceive(settings->receivecallback);
	else
		lora_onReceive(0);

	if (settings->transmitCompleteClb)
		_transmitCompleteClb = settings->transmitCompleteClb;
	else
		_transmitCompleteClb = 0;

	lora_setIdle();

	// Chip_GPIO_SetupPinInt(LPC_GPIO, 1, 9, GPIO_INT_RISING_EDGE);
}

void RFM98W::lora_deinit()
{
	// put in sleep mode
	lora_setSleep();
	// stop SPI
}

uint8_t RFM98W::lora_ready()
{
	loraStatus_e stat = lora_getStatus();
	return (stat != send) && (stat != receive) && (stat != disconnected);
}


//###########################PRIVATE FUNCTIONS##############################################

void RFM98W::spi_init(void){
	/* todo: implement */
}

uint8_t RFM98W::lora_singleTransfer(uint8_t address, uint8_t value)
{
	if (loraLocked) return 0;
	loraLocked = 1;
	uint8_t receive[2] =
	{ 0, 0 };
	uint8_t transmit[2] =
	{ address, value };
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_CS_Port, LORA_CS_PinNumber, false);
	// delayTicks(10);
	// Chip_GPIO_DisableInt(LPC_GPIO, 1, 0x200);
	// spi_transmitBlock(receive, transmit, sizeof(transmit));
	// Chip_GPIO_EnableInt(LPC_GPIO, 1, 0x200);
	// delayTicks(10);
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_CS_Port, LORA_CS_PinNumber, true);
	loraLocked = 0;
	return receive[1];
}

void RFM98W::lora_fifoTransfer(uint8_t address, const uint8_t* values, uint8_t length)
{

	if (loraLocked) return;
	loraLocked = 1;
	uint8_t data[256];
	data[0] = address | 0x80;
	memcpy(&data[1], values, length);
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_CS_Port, LORA_CS_PinNumber, false);
	// delayTicks(10);
	// Chip_GPIO_DisableInt(LPC_GPIO, 1, 0x200);
	// spi_transmitBlock(0, data, length + 1);
	// Chip_GPIO_EnableInt(LPC_GPIO, 1, 0x200);
	// delayTicks(10);
	// Chip_GPIO_SetPinState(LPC_GPIO, LORA_CS_Port, LORA_CS_PinNumber, true);
	loraLocked = 0;
}

void RFM98W::PIOINT1_IRQHandler(void)
{
	// Chip_GPIO_ClearInts(LPC_GPIO, 1, 0x200);
	lora_handleDio0Rise();
}

void RFM98W::lora_poll()
{
// 	if (Chip_GPIO_GetPinState(LPC_GPIO, 1, 9))
// 	{
// 		lora_handleDio0Rise();
// 	}
// #ifdef Lora_Block
// #endif
}







uint8_t RFM98W::lora_readRegister(uint8_t address)
{
	return lora_singleTransfer(address & 0x7f, 0x00);
}

void RFM98W::lora_writeRegister(uint8_t address, uint8_t value)
{
	lora_singleTransfer(address | 0x80, value);
}

void RFM98W::lora_writeRegisterSafe(uint8_t address, uint8_t value)
{
	uint8_t mode = lora_getMode();
	if (mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP)) lora_setSleep();
	lora_writeRegister(address, value);
	if (mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP)) lora_writeRegister(REG_OP_MODE, mode);
}




















loraStatus_e RFM98W::lora_getStatus()
{
	uint8_t mode = lora_getMode();
	if(mode == (MODE_LONG_RANGE_MODE | MODE_TX))
	return send;
	if(mode == (MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS) || mode == (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE))
	return receive;
	if(mode == (MODE_LONG_RANGE_MODE | MODE_STDBY))
	return idle;
	if(mode == (MODE_LONG_RANGE_MODE | MODE_SLEEP))
	return send;
	return disconnected;
}



uint8_t RFM98W::lora_sendBytes(const uint8_t* buffer, uint8_t length)
{
	loraStatus_e stat = lora_getStatus();
	if((stat == send) || (stat == disconnected))
	return 0;
		lora_setIdle();
		lora_writeRegister(REG_DIO_MAPPING_1, 1 << 6);
		lora_writeRegister(REG_FIFO_ADDR_PTR, 0);
		lora_fifoTransfer(REG_FIFO,buffer,length);
		if(messageSize == 0)
		lora_writeRegister(REG_PAYLOAD_LENGTH, length);
		lora_writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
		while ((lora_readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0)
		;
		lora_writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
		stat = idle;
	return length;
}

void RFM98W::lora_setMessageSize(uint8_t size)
{
	if(size > 0)
	{
		lora_implicitHeaderMode();
			lora_writeRegister(REG_PAYLOAD_LENGTH,size);
	}
	else
	{
		lora_explicitHeaderMode();
	}
	messageSize = size;
}

uint8_t RFM98W::lora_getMessageSize()
{
	return messageSize;
}

void RFM98W::lora_setReceive()
{
	if(!lora_ready())
	return;
		lora_writeRegister(REG_DIO_MAPPING_1, 0);
		if(lora_getMode() != (MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS))
		{
			lora_writeRegister(REG_PAYLOAD_LENGTH, lora_getMessageSize());
			lora_writeRegister(REG_FIFO_ADDR_PTR, 0);
			lora_writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
		}
}

uint8_t RFM98W::lora_parsePacket() {
	uint8_t packetLength = 0;
	loraStatus_e stat = lora_getStatus();
	if(stat != send && stat != disconnected)
	{
			lora_writeRegister(REG_PAYLOAD_LENGTH, lora_getMessageSize());

			uint8_t irqFlags = lora_readRegister(REG_IRQ_FLAGS);
			// clear IRQ's
			lora_writeRegister(REG_IRQ_FLAGS, irqFlags);

			if ((irqFlags & IRQ_RX_DONE_MASK) && ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)){
				// received a packet
				_packetIndex = 0;

				// read packet length
				if (messageSize > 0) {
					packetLength = messageSize;
					} else {
					packetLength = lora_readRegister(REG_RX_NB_BYTES);
				}

				// set FIFO address to current RX address
				lora_writeRegister(REG_FIFO_ADDR_PTR,lora_readRegister(REG_FIFO_RX_CURRENT_ADDR));

				// put in standby mode
				lora_setIdle();

			} else if (lora_getMode()
			!= (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)) {
				// not currently in RX mode

				// reset FIFO address
				lora_writeRegister(REG_FIFO_ADDR_PTR, 0);

				// put in single RX mode
				lora_writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
			}
		return packetLength;
	}
	return 0;
}

uint8_t RFM98W::lora_packetRssi() {
	uint8_t reg;
		reg = lora_readRegister(REG_PKT_RSSI_VALUE);
	return (reg - (_frequency < 868E6 ? 164 : 157));
}

float RFM98W::lora_packetSnr() {
	uint8_t reg;
		reg = lora_readRegister(REG_PKT_SNR_VALUE);
	return ((int8_t) reg) * 0.25;
}

uint8_t RFM98W::lora_available() {
	loraStatus_e stat = lora_getStatus();
	if(stat == send ||stat == disconnected)
	return 0;
	uint8_t reg;
		reg = lora_readRegister(REG_RX_NB_BYTES);
	return (reg - _packetIndex);
}

int16_t RFM98W::lora_read() {
	if (!lora_available()) {
		return -1;
	}

	_packetIndex++;

	uint8_t reg;
		reg = lora_readRegister(REG_FIFO);

	return reg;
}

uint16_t RFM98W::lora_readBytes(uint8_t* buffer, uint16_t length)
{
	uint16_t i = 0;
	uint8_t data;
	while((data = lora_read()) >= 0)
	{
		if(i == length)
		break;
		buffer[i] = data;
		i++;
	}
	return i;
}

int16_t RFM98W::lora_peek() {
	if (!lora_available()) {
		return -1;
	}
	uint8_t b;
		// store current FIFO address
		int currentAddress = lora_readRegister(REG_FIFO_ADDR_PTR);

		// read
		b = lora_readRegister(REG_FIFO);

		// restore FIFO address
		lora_writeRegister(REG_FIFO_ADDR_PTR, currentAddress);

	return b;
}

int8_t RFM98W::lora_getTemperature()
{
	int8_t temp;
	uint8_t regVal;
		regVal = lora_readRegister(REG_TEMP);

	temp = regVal & 0x7F;
	if((regVal & 0x80))
	{
		temp *= -1;
	}
	return temp;
}

void RFM98W::lora_onReceive(void (*callback)(void)) {
	_onReceive = callback;
}

void RFM98W::lora_setIdle() {
		lora_writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void RFM98W::lora_setSleep() {
		lora_writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void RFM98W::lora_setTxPower(uint8_t level) {
	if (level < 2) {
		level = 2;
		} else if (level > 17) {
		level = 17;
	}
		lora_writeRegisterSafe(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

void RFM98W::lora_setFrequency(uint32_t frequency) {
	_frequency = frequency;

	uint64_t frf = ((uint64_t) frequency << 19) / 32000000;
	uint8_t mode = lora_getMode();
	if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
	lora_setSleep();
		lora_writeRegister(REG_FRF_MSB, (uint8_t) (frf >> 16));
		lora_writeRegister(REG_FRF_MID, (uint8_t) (frf >> 8));
		lora_writeRegister(REG_FRF_LSB, (uint8_t) (frf >> 0));
		if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
		lora_writeRegister(REG_OP_MODE, mode);
}

void RFM98W::lora_setSpreadingFactor(uint8_t sf) {
	if (sf < 6) {
		sf = 6;
		} else if (sf > 12) {
		sf = 12;
	}
	uint8_t mode = lora_getMode();
	if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
	lora_setSleep();
		if (sf == 6) {
			lora_writeRegister(REG_DETECTION_OPTIMIZE, 0xc5);
			lora_writeRegister(REG_DETECTION_THRESHOLD, 0x0c);
			} else {
			lora_writeRegister(REG_DETECTION_OPTIMIZE, 0xc3);
			lora_writeRegister(REG_DETECTION_THRESHOLD, 0x0a);
		}

		lora_writeRegister(REG_MODEM_CONFIG_2,
		(lora_readRegister(REG_MODEM_CONFIG_2) & 0x0f)
		| ((sf << 4) & 0xf0));
		if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
		lora_writeRegister(REG_OP_MODE, mode);
}

void RFM98W::lora_setSignalBandwidth(uint32_t sbw) {
	uint8_t bw;

	if (sbw <= 7.8E3) {
		bw = 0;
		} else if (sbw <= 10.4E3) {
		bw = 1;
		} else if (sbw <= 15.6E3) {
		bw = 2;
		} else if (sbw <= 20.8E3) {
		bw = 3;
		} else if (sbw <= 31.25E3) {
		bw = 4;
		} else if (sbw <= 41.7E3) {
		bw = 5;
		} else if (sbw <= 62.5E3) {
		bw = 6;
		} else if (sbw <= 125E3) {
		bw = 7;
		} else if (sbw <= 250E3) {
		bw = 8;
		} else if (sbw <= 500E3){
		bw = 9;
	}
		lora_writeRegisterSafe(REG_MODEM_CONFIG_1,
		(lora_readRegister(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
}

void RFM98W::lora_setCodingRate4(uint8_t denominator) {
	if (denominator < 5) {
		denominator = 5;
		} else if (denominator > 8) {
		denominator = 8;
	}

	uint8_t cr = denominator - 4;
		lora_writeRegisterSafe(REG_MODEM_CONFIG_1,
		(lora_readRegister(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void RFM98W::lora_setPreambleLength(uint16_t length) {
	uint8_t mode = lora_getMode();
	if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
	lora_setSleep();
		lora_writeRegister(REG_PREAMBLE_MSB, (uint8_t) (length >> 8));
		lora_writeRegister(REG_PREAMBLE_LSB, (uint8_t) (length >> 0));
		if(mode != (MODE_LONG_RANGE_MODE | MODE_SLEEP))
		lora_writeRegister(REG_OP_MODE, mode);
}

void RFM98W::lora_setSyncWord(uint8_t sw) {
		lora_writeRegisterSafe(REG_SYNC_WORD, sw);
}

uint8_t RFM98W::lora_getMode()
{
	uint8_t mode;
		mode = lora_readRegister(REG_OP_MODE);
	return mode;
}

void RFM98W::lora_crc() {
		lora_writeRegisterSafe(REG_MODEM_CONFIG_2,
		lora_readRegister(REG_MODEM_CONFIG_2) | 0x04);
}

void RFM98W::lora_noCrc() {
		lora_writeRegisterSafe(REG_MODEM_CONFIG_2,
		lora_readRegister(REG_MODEM_CONFIG_2) & 0xfb);
}

uint8_t RFM98W::lora_random() {
	uint8_t reg;
		reg = lora_readRegister(REG_RSSI_WIDEBAND);
	return reg;
}

void RFM98W::lora_explicitHeaderMode()
{
		lora_writeRegister(REG_MODEM_CONFIG_1,
		lora_readRegister(REG_MODEM_CONFIG_1) & 0xfe);
}

void RFM98W::lora_implicitHeaderMode() {
		lora_writeRegister(REG_MODEM_CONFIG_1,
		lora_readRegister(REG_MODEM_CONFIG_1) | 0x01);
}

void RFM98W::lora_handleDio0Rise()
{
	uint8_t irqFlags;
	lora_writeRegister(REG_PAYLOAD_LENGTH, lora_getMessageSize());
	irqFlags = lora_readRegister(REG_IRQ_FLAGS);
	lora_writeRegister(REG_IRQ_FLAGS, irqFlags);
	if (irqFlags & IRQ_RX_DONE_MASK)
	{
		if ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
		{
			_packetIndex = 0;
			if (_onReceive) _onReceive();
		}
	}
	else if (irqFlags & IRQ_TX_DONE_MASK)
	{
		if (_transmitCompleteClb) _transmitCompleteClb();
	}
}

