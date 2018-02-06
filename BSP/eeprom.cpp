/* @file: eeprom.cpp
 * @author: Gerald Ebmer
 * @date: 06.02.2018
 * @brief: eeprom module STM32L152
 */


#include "mbed.h"
#include "eeprom.h"


HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t data)
 {
    HAL_StatusTypeDef  status;
    address = address + 0x08080000;
    // check adrress under-/overflow
    if(address < 0x08080000 || address > 0x08080FFF)
        return HAL_ERROR;
    // write eeprom
    HAL_FLASHEx_DATAEEPROM_Unlock();  //Unprotect the EEPROM to allow writing
    status = HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_BYTE, address, data);
    HAL_FLASHEx_DATAEEPROM_Lock();  // Reprotect the EEPROM
    return status;
}


uint8_t readEEPROMByte(uint32_t address){
    uint8_t tmp = 0;
    address = address + 0x08080000;
    // check adrress under-/overflow
    if(address < 0x08080000 || address > 0x08080FFF)
        return 0;
    // read eeprom
    tmp = *(__IO uint32_t*)address;
    return tmp;
}


int writeEEPROMBytes(uint32_t address, uint8_t *data, int len){
    for(int i = 0; i<len; i++){
        writeEEPROMByte(address+i,data[i]);
    }
    return SUCCESS;
}

int readEEPROMBytes(uint32_t address, uint8_t *data, int len){
    for(int i = 0; i<len; i++){
        data[i] = readEEPROMByte(address+i);
    }
    return SUCCESS;
}
