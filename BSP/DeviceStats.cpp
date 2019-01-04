/* @file: DeviceStats.cpp
 * @author: Gerald Ebmer
 * @date: 06.02.2018
 * @brief: device statistics module implementation
 */

#include "mbed.h"
#include "nvstore.h"
#include "DeviceStats.h"
#include "logprintf.h"
// #include "eeprom.h"

/* DO NOT USE readEEPROMBytes AND writeEEPROMBytes WITH MBED > 5.7: CAUSES SYSTEM CRASH
 * USE NVStore API PROVIDED BY MBED INSTEAD
 */

void print_return_code(int rc, int expected_rc)
{
    xprintf("Return code is %d ", rc);
    if (rc == expected_rc)
        xprintf("(as expected).\n");
    else
        xprintf("(expected %d!).\n", expected_rc);
}

ResetCounter_t resetCounter;

#define EEPROM_ADDR 0x0000

void procResetCounter(){
    NVStore &nvstore = NVStore::get_instance();
    const uint16_t key = 1;
    uint16_t actual_len_bytes = 0;
    int rc;

    // load flash content
    // readEEPROMBytes(EEPROM_ADDR, (uint8_t*)&resetCounter,sizeof(resetCounter));
    rc = nvstore.get(key, sizeof(resetCounter), &resetCounter, actual_len_bytes);
    print_return_code(rc, NVSTORE_SUCCESS);

    // inkrement reset counter
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)     resetCounter.OBLRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)     resetCounter.NRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)     resetCounter.PORRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)     resetCounter.SWRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)    resetCounter.IWDRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)    resetCounter.WWDRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)    resetCounter.LPRST++;
    // clear reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS();

    // store reset counter
    // writeEEPROMBytes(EEPROM_ADDR,(uint8_t*)&resetCounter,sizeof(resetCounter));
    rc = nvstore.set(key,sizeof(resetCounter),&resetCounter);
    print_return_code(rc, NVSTORE_SUCCESS);
}

void clearResetCounter(){
    NVStore &nvstore = NVStore::get_instance();
    const uint16_t key = 1;
    int rc;

    // set reset counter = 0
    resetCounter.OBLRST = 0;
    resetCounter.NRST = 0;
    resetCounter.PORRST = 0;
    resetCounter.SWRST = 0;
    resetCounter.IWDRST = 0;
    resetCounter.WWDRST = 0;
    resetCounter.LPRST = 0;
    // store reset counter in flash
    // writeEEPROMBytes(EEPROM_ADDR,(uint8_t*)&resetCounter,sizeof(resetCounter));
    rc = nvstore.set(key,sizeof(resetCounter),&resetCounter);
    print_return_code(rc, NVSTORE_SUCCESS);
}

void printDeviceStats(){
    printf("Reset State:\n");
    printf("\tOptions Byte Load reset: %ld\n",resetCounter.OBLRST);
    printf("\tReset Pin reset:         %ld\n",resetCounter.NRST);
    printf("\tPower On reset:          %ld\n",resetCounter.PORRST);
    printf("\tSoftware reset:          %ld\n",resetCounter.SWRST);
    printf("\tIndep. Watchdog reset:   %ld\n",resetCounter.IWDRST);
    printf("\tWindow Watchdog reset:   %ld\n",resetCounter.WWDRST);
    printf("\tLow Power reset:         %ld\n",resetCounter.LPRST);

}

int32_t getSoftwareReset(){
    return resetCounter.SWRST;
}

int32_t getPowerOnReset(){
    return resetCounter.PORRST;
}