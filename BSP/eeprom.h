/* @file: eeprom.h
 * @author: Gerald Ebmer
 * @date: 06.02.2018
 * @brief: eeprom module STM32L152
 */

#ifndef EEPROM_H
#define EEPROM_H

#include "stm32l1xx_hal.h"

HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t data);
uint8_t readEEPROMByte(uint32_t address);

int readEEPROMBytes(uint32_t address, uint8_t *data, int len);
int writeEEPROMBytes(uint32_t address, uint8_t *data, int len);



#endif // EEPROM_H