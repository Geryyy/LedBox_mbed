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




