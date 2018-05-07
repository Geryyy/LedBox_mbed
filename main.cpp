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
        printf("System Status:\n----------------------\n");
        printf("%s\n",msg);
        wait(2);
    } 
}


#define BUF_SIZE 1024

CircularBuffer<char, BUF_SIZE> RadioTxBuf;
CircularBuffer<char, BUF_SIZE> RadioRxBuf;



void radioTransceiveTask(){
    char data[256] = {0};
    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, RADIO_RESET, LORA_BAUD, DEBUG_ON, rxCallback);

    wait(1);

    while(1){
        // if databuf not empty -> transmit
        int i = 0;
        while(RadioTxBuf.empty() == false){
            RadioTxBuf.pop(data[i]);
            i++;
            if(i >= 100) // 100 bytes -> 200 hex values < max 255
                break;
        }

        if(i > 0){
            // radio.sendBytes((unsigned char*)data, i);
            radio.write((char*)data, i);
            // back to start, transmit remaining data
            continue;
        }

        // wait(1);
        // if nothing to transmit, receive data
        unsigned char *ret;
        printf("\t\t receiveBytes()\n");
        int len = radio.receiveBytes(&ret); //radio.readLine(&ret);
        for(int i = 0; i<len; i++){
            RadioRxBuf.push(ret[i]);
            printf("\t\t rxbuffer.push(): %c\n", ret[i]);
        }
        

        wait(1);
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
    // RadioThread.start(BatteryTaskRadio);
    // LoraDevThread.start(LoraDevTask);
    RadioThread.start(radioTransceiveTask); // transmit with ringbuffer

    // LEDdriverThread.start(LEDdriverTask);
    // BatteryThread.start(BatteryTask2);

    //printf("Deep sleep allowed: %i\r\n", sleep_manager_can_deep_sleep());
    char msg[1024] = {0};
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    while(true) {
        
        
        // sprintf(msg, "Tbat:\t%4.1f C\nUbat:\t%4.2f V\nIbat:\t%4.3f A\nUin:\t%4.2f V\nUsys:\t%4.2f V\nIin:\t%4.3f A\nTdie:\t%4.1f C\n\r",\
        //     bat.getBatTemp(), \
        //     bat.getUBat(), \
        //     bat.getIBat(),
        //     bat.getUin(),\
        //     bat.getUsys(),\
        //     bat.getIin(),\
        //     bat.getTdie() );
        
        // for(uint16_t i = 0; i<4; i++){ // for(uint16_t i = 0; i<strlen(msg); i++){
        //     RadioTxBuf.push(msg[i]);
        // }
        for(int i = 0; i<3; i++){
            RadioTxBuf.push('U');
        }

        printf("TX buffer size: %ld\nRX buffer size: %ld\n",RadioTxBuf.size(), RadioRxBuf.size());
    wait(5);

    }
}


void init(){
    printf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    printf("System Clock: %ld\n", SystemCoreClock);
    procResetCounter();
    printDeviceStats();
}



