#include "mbed.h"
#include "BSP.h"

DigitalOut LED(User_LED);
void LEDTask(){
    LED = !LED;
    wait_ms(500);
}