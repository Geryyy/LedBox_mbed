#include "mbed.h"
#include "BSP/BSP.h"
#include "lora_radio.h"

/*
   This basic example just shows how to read the ADC internal channels raw values.
   Please look in the corresponding device reference manual for a complete
   description of how to make a temperature sensor, VBat or Vref measurement.
*/
Thread LEDThread;
Thread SysPrintThread;
Thread RadioThread;

int main()
{   
    LEDThread.start(LEDTask);
    //SysPrintThread.start(PrintSystemInformation);
    RadioThread.start(radioTask3);

    while(true) {
        wait(1.0);
    }
}
