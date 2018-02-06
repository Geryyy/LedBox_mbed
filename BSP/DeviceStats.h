/* @file: DeviceStats.h
 * @author: Gerald Ebmer
 * @date: 06.02.2018
 * @brief: device statistics module
 */
#ifndef DEVICESTATS_H
#define DEVICESTATS_H

#include "stm32l1xx_hal.h"

typedef struct ResetCounter_s{
    uint32_t OBLRST; // Options bytes loading reset flag
    uint32_t NRST; // NRST Pin reset
    uint32_t PORRST; // POR/PDR reset
    uint32_t SWRST; // Software reset
    uint32_t IWDRST; // Independent Watchdog reset
    uint32_t WWDRST; // Window watchdog reset
    uint32_t LPRST; // low-power reset
}ResetCounter_t;

#endif // DEVICESTATS_H