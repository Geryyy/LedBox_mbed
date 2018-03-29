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


signed char rxCallback(fifo_t *buffer){
    return 0;
}
/*** TASKS ***/

void BatteryTaskRadio(){
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, RADIO_RESET, LORA_BAUD, DEBUG_ON,NULL);

    char msg[1024] = {0};
    while(1){
        sprintf(msg, "Tbat:\t%4.1f C\nUbat:\t%4.2f V\nIbat:\t%4.3f A\nUin:\t%4.2f V\nUsys:\t%4.2f V\nIin:\t%4.3f A\nTdie:\t%4.1f C\n\r",\
            bat.getBatTemp(), \
            bat.getUBat(), \
            bat.getIBat(),
            bat.getUin(),\
            bat.getUsys(),\
            bat.getIin(),\
            bat.getTdie() );
        radio.write(msg,strlen(msg));
        bat.printStatus();
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
    
    //Watchdog Lessie = Watchdog(1);
    WatchdogThread.start(WatchdogTask);
    LEDThread.start(LEDTask);
    //SysPrintThread.start(PrintSystemInformation);
    RadioThread.start(BatteryTaskRadio);
    
    //RadioThread.start(radioTransceiveTask); // transmit with ringbuffer

    // LEDdriverThread.start(LEDdriverTask);
    // BatteryThread.start(BatteryTask2);

    //printf("Deep sleep allowed: %i\r\n", sleep_manager_can_deep_sleep());
    // char msg[1024] = {0};
    // BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    while(true) {
        wait(5);
        
        // sprintf(msg, "Tbat:\t%4.1f C\nUbat:\t%4.2f V\nIbat:\t%4.3f A\nUin:\t%4.2f V\nUsys:\t%4.2f V\nIin:\t%4.3f A\nTdie:\t%4.1f C\n\r",\
        //     bat.getBatTemp(), \
        //     bat.getUBat(), \
        //     bat.getIBat(),
        //     bat.getUin(),\
        //     bat.getUsys(),\
        //     bat.getIin(),\
        //     bat.getTdie() );
        
        // for(uint16_t i = 0; i<strlen(msg); i++){
        //     RadioTxBuf.push(msg[i]);
        // }

        // printf("TX buffer size: %ld\nRX buffer size: %ld\n",RadioTxBuf.size(), RadioRxBuf.size());


    }
}


void init(){
    printf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    printf("System Clock: %ld\n", SystemCoreClock);
    procResetCounter();
    printDeviceStats();
}



