#include "mbed.h"
#include "BSP.h"
#include "stm32l1xx_ll_adc.h"

DigitalOut led(User_LED);
void LEDTask(){
    while(true){
        led = !led;
        wait(1);
    }
}

void PrintSystemInformation(){
    printf("\nLED Box Status Infomation:\n");
    while(true){
        printf("ADC Temp = %f\n", getMCUTemp());
        printf("ADC VRef = %f\n", getUrefCal());         
        printf("SysVolt  = %f\n", getSysVoltage());
        printf("\033[3A");
        wait(1.0);
    }
}


// AnalogIn adc_temp(ADC_TEMP);
// AnalogIn adc_vref(ADC_VREF);

//         printf("ADC Temp = %f\n", (adc_temp.read()*100));
//         printf("ADC VRef = %f\n", adc_vref.read()); 

float getUrefCal(){
    uint16_t *UrefCalVal = VREFINT_CAL_ADDR;
    uint32_t UrefCalVol = VREFINT_CAL_VREF;
    const float UrefCal = (float)(*UrefCalVal) / 4096.0;
    const float VddaCal = (float)(UrefCalVol) / 1000.0; // mV -> V
    return UrefCal * VddaCal;
}

static AnalogIn adc_vref(ADC_VREF);
float getSysVoltage(){
    float UrefCal = getUrefCal(); // [V]
    float Uref_raw = adc_vref.read();
    return UrefCal / Uref_raw; // System Voltage [V]
}

static AnalogIn adc_temp(ADC_TEMP);
float getMCUTemp(){
    uint16_t *TempCalVal1 = TEMPSENSOR_CAL1_ADDR; // calibration value @ 30°C
    uint16_t *TempCalVal2 = TEMPSENSOR_CAL2_ADDR; // calibration value @ 110°C
    uint32_t CalTemp1 = TEMPSENSOR_CAL1_TEMP;
    uint32_t CalTemp2 = TEMPSENSOR_CAL2_TEMP;
    uint32_t UrefCal= TEMPSENSOR_CAL_VREFANALOG;
    float U_T1 = (float)(*TempCalVal1)/4096.0 * (float)UrefCal / 1000.0;
    float U_T2 = (float)(*TempCalVal2)/4096.0 * (float)UrefCal / 1000.0;
    float T1 = (float) CalTemp1;
    float T2 = (float) CalTemp2;
    float k = (T2-T1)/(U_T2-U_T1);
    float d = T1 - k*U_T1;
    float U_Traw = adc_temp.read();
    float Uref = getSysVoltage();
    float Temp = k * U_Traw * Uref + d;
    return Temp;
}