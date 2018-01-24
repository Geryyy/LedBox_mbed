#include "BSP.h"


void LEDTask(){
     static UserLED led = new UserLED();
    led.toggle();
    wait_ms(500);
}