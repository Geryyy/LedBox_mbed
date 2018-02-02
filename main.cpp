#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include "LEDdriver.h"
#include "BatteryManager.h"

/*
   This basic example just shows how to read the ADC internal channels raw values.
   Please look in the corresponding device reference manual for a complete
   description of how to make a temperature sensor, VBat or Vref measurement.
*/
Thread LEDThread;
Thread SysPrintThread;
Thread RadioThread;
Thread LEDdriverThread;
Thread BatteryThread;

int main()
{   
    LEDThread.start(LEDTask);
    //SysPrintThread.start(PrintSystemInformation);
    RadioThread.start(RadioTask);
    //LEDdriverThread.start(LEDdriverTask);
    //BatteryThread.start(BatteryTask2);

    //printf("Deep sleep allowed: %i\r\n", sleep_manager_can_deep_sleep());

    while(true) {
        wait(1.0);
        //sleep();
    }
}
