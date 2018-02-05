/* @file: Watchdog.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: watchdog implementation
 */

#include "mbed.h"
#include "Watchdog.h"


Watchdog::Watchdog(float period_s){
  _hiwdg.Instance = IWDG;
  _hiwdg.Init.Prescaler = 0x02; // div 16
  _hiwdg.Init.Reload = (uint16_t)(period_s*1000000.0/16.0); // 12 Bit !! Watchdog LS Clk 1 MHz
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
        wait_ms(90);
    }
}