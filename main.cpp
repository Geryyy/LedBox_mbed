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
    RadioThread.start(BatteryTaskRadio);
    // LEDdriverThread.start(LEDdriverTask);
    //BatteryThread.start(BatteryTask2);

    //printf("Deep sleep allowed: %i\r\n", sleep_manager_can_deep_sleep());
    while(true) {
        wait(0.1);
        //sleep();
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
    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, LORA_BAUD, DEBUG_ON);
    // wait_ms(10);

    // char *msg = (char*)"Hello World!"; 
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
        // sprintf(msg,"Hello World!");
        radio.sendBytes(msg,strlen(msg));
        wait(1);
        
       // radio.sendtest();
    } 
}
