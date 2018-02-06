/* @file: Watchdog.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: watchdog implementation
 */

#include "mbed.h"
#include "Watchdog.h"
#include "BSP.h"

#define LSI_CLK 37000.0

Watchdog::Watchdog(float period_s){
  _hiwdg.Instance = IWDG;
  _hiwdg.Init.Prescaler = 0x03; // div 32
  _hiwdg.Init.Reload = (uint16_t)(period_s*LSI_CLK/32.0); // 12 Bit period_s = 0.001 .. 3,5
  if (HAL_IWDG_Init(&_hiwdg) != HAL_OK)
  {
    fprintf(stderr,"error: __FILE__, __LINE__\n");
  }
}

void Watchdog::kick(){
    HAL_IWDG_Refresh(&_hiwdg);
}


/*** task ***/

void WatchdogTask(){
    Watchdog lessie = Watchdog(1);

    while(true){
        lessie.kick();
        wait_ms(900);
    }
}

DigitalIn Taster(USER_BUTTON);
void WatchdogTest(){
    Watchdog lessie = Watchdog(1);

    while(true){
        if(Taster){
            lessie.kick();
        }
        wait_ms(900);
    }
}