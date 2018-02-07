/* @file: LEDdriver.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: LEDdriver class implementation
 */

#include "mbed.h"
#include "BSP.h"
#include "LEDdriver.h"

LEDdriver::LEDdriver(PinName pin_shdn, PinName pin_pwm, PinName pin_iLed){
    shdn = new DigitalOut(pin_shdn);
    pwm = new PwmOut(pin_pwm);
    iLed = new AnalogOut(pin_iLed);

    this->off(); 
    this->setILed(1.0); // 1A LED Strom
    this->setPWM(0.5); // 50% Dutycycle
}

int LEDdriver::on(void){
    shdn->write(1);
    return SUCCESS;
}

int LEDdriver::off(void){
    shdn->write(0);
    return SUCCESS;
}

int LEDdriver::setILed(float val){
    if(val>=0.0 && val <= 1.0){
        iLed->write(val);
        return SUCCESS;
    }   
    else
        return ERROR;
}

int LEDdriver::setPWM(float val){
    if(val>=0.0 && val <= 1.0){
        pwm->write(val);
        pwm->period_ms(1);
        return SUCCESS;
    }   
    else
        return ERROR;
}


/*** Testfunktion ***/

void LEDdriverTask(){
    printf("LEDdriver Task gestartet\n");
    LEDdriver L1(LED1_SHDN, LED1_PWM, ILED1);
    LEDdriver L2(LED2_SHDN, LED2_PWM, ILED2);

    L1.setILed(0.0); // 50mA
    L1.setPWM(0.0);
    L1.on();

    L2.setILed(0.0); // 50mA
    L2.setPWM(0.0);
    L2.on();

    float t = 0.0;

    while(true){
        wait(0.01);
         float val = 0.1*(1.0 + sin(2*M_PI*0.5*t));
         L1.setILed(val);
         val = 0.1*(1.0 + cos(2*M_PI*0.5*t));
         L2.setILed(val);
         t = t + 0.01;
    }
}

/*** Messwerte ***/
/*
setILED     I
0.01        0.021
0.05        0.073
0.075       0.142
0.1         0.217
0.2         0.467      
0.3         0.594
0.4         0.654
0.5         0.705
0.6         0.766


*/

