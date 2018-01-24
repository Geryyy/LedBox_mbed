#include "mbed.h"
#include "BSP/BSP.h"

/*
   This basic example just shows how to read the ADC internal channels raw values.
   Please look in the corresponding device reference manual for a complete
   description of how to make a temperature sensor, VBat or Vref measurement.
*/
Thread LEDthread;

AnalogIn adc_temp(ADC_TEMP);
AnalogIn adc_vref(ADC_VREF);
#if defined(TARGET_DISCO_F051R8) || defined(TARGET_NUCLEO_F030R8) || defined(TARGET_NUCLEO_F031K6) || defined(TARGET_NUCLEO_F042K6) || defined(TARGET_NUCLEO_F070RB) || defined(TARGET_NUCLEO_F072RB) || defined(TARGET_NUCLEO_F091RC) || defined(TARGET_NUCLEO_F207ZG) || defined(TARGET_NUCLEO_F302R8) || defined(TARGET_NUCLEO_F303K8) || defined(TARGET_DISCO_F303VC) || defined(TARGET_NUCLEO_F303RE) || defined(TARGET_NUCLEO_F303ZE) || defined(TARGET_DISCO_F334C8) || defined(TARGET_NUCLEO_F334R8) || defined(TARGET_MTS_DRAGONFLY_F411RE) || defined(TARGET_MTS_MDOT_F405RG) || defined(TARGET_MTS_MDOT_F411RE) || defined(TARGET_DISCO_F401VC) || defined(TARGET_NUCLEO_F401RE) || defined(TARGET_ARCH_MAX) || defined(TARGET_DISCO_F407VG) || defined(TARGET_NUCLEO_F410RB) || defined(TARGET_ELMO_F411RE) || defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F412ZG) || defined(TARGET_DISCO_F413ZH) || defined(TARGET_DISCO_F429ZI) || defined(TARGET_NUCLEO_F429ZI) || defined(TARGET_NUCLEO_F439ZI) || defined(TARGET_B96B_F446VE) || defined(TARGET_NUCLEO_F446RE) || defined(TARGET_NUCLEO_F446ZE) || defined(TARGET_DISCO_F469NI) || defined(TARGET_DISCO_F746NG) || defined(TARGET_NUCLEO_F746ZG) || defined(TARGET_NUCLEO_F756ZG) || defined(TARGET_NUCLEO_F767ZI) || defined(TARGET_DISCO_F769NI) || defined(TARGET_DISCO_L072CZ_LRWAN1) || defined(TARGET_NUCLEO_L432KC) || defined(TARGET_DISCO_L476VG) || defined(TARGET_NUCLEO_L476RG) || defined(TARGET_NUCLEO_L486RG) 
AnalogIn adc_vbat(ADC_VBAT); // Warning: Not available on all devices
#endif

DigitalOut led(LED1);

int main()
{   
    LEDthread.start(LEDTask);

    printf("\nSTM32 ADC internal channels reading example\n");
    while(1) {
        printf("ADC Temp = %f\n", (adc_temp.read()*100));
        printf("ADC VRef = %f\n", adc_vref.read());         
        #if defined(TARGET_DISCO_F051R8) || defined(TARGET_NUCLEO_F030R8) || defined(TARGET_NUCLEO_F031K6) || defined(TARGET_NUCLEO_F042K6) || defined(TARGET_NUCLEO_F070RB) || defined(TARGET_NUCLEO_F072RB) || defined(TARGET_NUCLEO_F091RC) || defined(TARGET_NUCLEO_F207ZG) || defined(TARGET_NUCLEO_F302R8) || defined(TARGET_NUCLEO_F303K8) || defined(TARGET_DISCO_F303VC) || defined(TARGET_NUCLEO_F303RE) || defined(TARGET_NUCLEO_F303ZE) || defined(TARGET_DISCO_F334C8) || defined(TARGET_NUCLEO_F334R8) || defined(TARGET_MTS_DRAGONFLY_F411RE) || defined(TARGET_MTS_MDOT_F405RG) || defined(TARGET_MTS_MDOT_F411RE) || defined(TARGET_DISCO_F401VC) || defined(TARGET_NUCLEO_F401RE) || defined(TARGET_ARCH_MAX) || defined(TARGET_DISCO_F407VG) || defined(TARGET_NUCLEO_F410RB) || defined(TARGET_ELMO_F411RE) || defined(TARGET_NUCLEO_F411RE) || defined(TARGET_NUCLEO_F412ZG) || defined(TARGET_DISCO_F413ZH) || defined(TARGET_DISCO_F429ZI) || defined(TARGET_NUCLEO_F429ZI) || defined(TARGET_NUCLEO_F439ZI) || defined(TARGET_B96B_F446VE) || defined(TARGET_NUCLEO_F446RE) || defined(TARGET_NUCLEO_F446ZE) || defined(TARGET_DISCO_F469NI) || defined(TARGET_DISCO_F746NG) || defined(TARGET_NUCLEO_F746ZG) || defined(TARGET_NUCLEO_F756ZG) || defined(TARGET_NUCLEO_F767ZI) || defined(TARGET_DISCO_F769NI) || defined(TARGET_DISCO_L072CZ_LRWAN1) || defined(TARGET_NUCLEO_L432KC) || defined(TARGET_DISCO_L476VG) || defined(TARGET_NUCLEO_L476RG) || defined(TARGET_NUCLEO_L486RG) 
        printf("ADC VBat = %f\n", adc_vbat.read());
        printf("\033[3A");
        #else
        printf("\033[2A");
        #endif
        led = !led;
        wait(1.0);
    }
}
