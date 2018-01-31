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

    L1.setILed(1.0); // 50mA
    L1.setPWM(1.0);
    L1.on();

    L2.setILed(0.1); // 50mA
    L2.setPWM(1.0);
    L2.off();

    while(true){
        wait(1.0);
    }
}

