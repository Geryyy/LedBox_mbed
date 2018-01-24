#include "mbed.h"
#include "BSP/BSP.h"

Serial radio(RADIO_TX, RADIO_RX);

void theradio(){
    radio.baud(57600);
    while(true){
        radio.printf("sys get ver");
        wait(1);
    }
}