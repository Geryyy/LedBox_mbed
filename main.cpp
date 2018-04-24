#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "Watchdog.h"
#include "DeviceStats.h"
#include "libsmp.h"
#include "libfifo.h"
#include <cstddef>

void init();
void BatteryTaskRadio();
void radioTransceiveTask();

Thread LEDThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDThread");
Thread SysPrintThread(osPriorityNormal, OS_STACK_SIZE,NULL,"SysPrintThread");
Thread RadioThread(osPriorityNormal, 8*1024,NULL,"RadioThread");
Thread LEDdriverThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDdriverThread");
Thread BatteryThread(osPriorityNormal, OS_STACK_SIZE,NULL,"BatteryThread");
Thread WatchdogThread(osPriorityNormal, OS_STACK_SIZE,NULL,"WatchdogThread");
Thread LoraDevThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LoraDevThread");


signed char rxCallback(fifo_t *buffer){
    return 0;
}
/*** TASKS ***/

void LoraDevTask(){
    DigitalInOut _reset = DigitalInOut(RADIO_RESET, PIN_OUTPUT, OpenDrain, 1);
    Serial _radio(RADIO_TX, RADIO_RX, LORA_BAUD);

    _radio.printf("sys get ver\r\n");
    wait_ms(10);

    _reset.write(0);
    wait_ms(100);
    _reset.write(1);
    wait_ms(200);
    _radio.printf("U\r\n");
    wait_ms(10);
    _radio.printf("sys get ver\r\n");
    // _radio.printf("sys reset\r\n0");

    while(1){


        wait(1);
    }
}



#define BUF_SIZE 1024

CircularBuffer<char, BUF_SIZE> RadioTxBuf;
CircularBuffer<char, BUF_SIZE> RadioRxBuf;



void radioTransceiveTask(){
    char data[256] = {0};
    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, RADIO_RESET, LORA_BAUD, DEBUG_ON, rxCallback);


    while(1){
        // if databuf not empty -> transmit
        int i = 0;
        while(RadioTxBuf.empty() == false){
            RadioTxBuf.pop(data[i]);
            i++;
            if(i >= 127) // 127 bytes -> 254 hex values < max 255
                break;
        }

        if(i > 0){
            radio.sendBytes((unsigned char*)data,i);
            // back to start to transmit remaining data
            continue;
        }

        // if nothing to transmit, receive data
        char *ret;
        printf("\t\t readline()\n");
        int len = radio.readLine(&ret);
        for(int i = 0; i<len; i++){
            RadioRxBuf.push(ret[i]);
            printf("\t\t rxbuffer.push(): %c\n", ret[i]);
        }
        

        wait(0.05);
    }
}

/****************** MAIN **********************/

int main()
{   
    init();
    
    WatchdogThread.start(WatchdogTask);
    LEDThread.start(LEDTask);

    LoraDevThread.start(LoraDevTask);

    while(true) {
        wait(5);
        


    }
}


void init(){
    printf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    printf("System Clock: %ld\n", SystemCoreClock);
    procResetCounter();
    printDeviceStats();
}



