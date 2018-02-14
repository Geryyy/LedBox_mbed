#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "Watchdog.h"
#include "DeviceStats.h"

void init();
void BatteryTaskRadio();

Thread LEDThread;
Thread SysPrintThread;
Thread RadioThread;
Thread LEDdriverThread;
Thread BatteryThread;
Thread WatchdogThread;


int main()
{   
    init();
    
    //Watchdog Lessie = Watchdog(1);
     WatchdogThread.start(WatchdogTask);
    LEDThread.start(LEDTask);
    //SysPrintThread.start(PrintSystemInformation);
    //RadioThread.start(BatteryTaskRadio);
    
    RadioThread.start(radioTransceiveTask); // transmit with ringbuffer

    // LEDdriverThread.start(LEDdriverTask);
    //BatteryThread.start(BatteryTask2);

    //printf("Deep sleep allowed: %i\r\n", sleep_manager_can_deep_sleep());
    char msg[1024] = {0};
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    while(true) {
        wait(5);
        
        sprintf(msg, "Tbat:\t%4.1f C\nUbat:\t%4.2f V\nIbat:\t%4.3f A\nUin:\t%4.2f V\nUsys:\t%4.2f V\nIin:\t%4.3f A\nTdie:\t%4.1f C\n\r",\
            bat.getBatTemp(), \
            bat.getUBat(), \
            bat.getIBat(),
            bat.getUin(),\
            bat.getUsys(),\
            bat.getIin(),\
            bat.getTdie() );
        
        for(uint16_t i = 0; i<strlen(msg); i++){
            RadioTxBuf.push(msg[i]);
        }

        printf("TX buffer size: %ld\nRX buffer size: %ld\n",RadioTxBuf.size(), RadioRxBuf.size());


    }
}


void init(){
    printf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    printf("System Clock: %ld\n", SystemCoreClock);
    procResetCounter();
    printDeviceStats();
}

void BatteryTaskRadio(){
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, RADIO_RESET, LORA_BAUD, DEBUG_OFF);

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
        radio.sendBytes(msg,strlen(msg));
        bat.printStatus();
        wait(1);
    } 
}
