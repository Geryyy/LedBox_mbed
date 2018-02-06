/* @file: DeviceStats.cpp
 * @author: Gerald Ebmer
 * @date: 06.02.2018
 * @brief: device statistics module implementation
 */

#include "mbed.h"
#include "DeviceStats.h"

// #if !defined(POST_APPLICATION_ADDR)
// #error "target.restrict_size must be set for your target in mbed_app.json"
// #endif

FlashIAP flash;

ResetCounter_t resetCounter;

void procResetCounter(){
    // load flash content

    // inkrement reset counter
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET)     resetCounter.OBLRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)     resetCounter.NRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)     resetCounter.PORRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)     resetCounter.SWRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)    resetCounter.IWDRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)    resetCounter.WWDRST++;
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET)    resetCounter.LPRST++;

    // store reset counter

}

void clearResetCounter(){
    // set reset counter = 0

    // store reset counter in flash

}