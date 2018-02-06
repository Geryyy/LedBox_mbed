/* @file: Watchdog.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: definition of watchdog module
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "stm32l1xx_hal.h"

class Watchdog{

private:
    IWDG_HandleTypeDef _hiwdg;

public:
    Watchdog(float period_s);
    void kick();
};

/*** task ***/

void WatchdogTask();
void WatchdogTest();


#endif // WATCHDOG_H