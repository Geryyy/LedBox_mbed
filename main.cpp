#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "Watchdog.h"
#include "DeviceStats.h"

void init();

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
    RadioThread.start(RadioTask);
    LEDdriverThread.start(LEDdriverTask);
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
