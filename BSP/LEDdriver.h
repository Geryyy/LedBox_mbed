/* @file: LEDdriver.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: LEDdriver class definition
 */

#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include "mbed.h"

class LEDdriver{
private:
    DigitalOut *shdn;
    DigitalOut *pwm;
    AnalogOut *iLed;

public:
    LEDdriver(PinName pin_shdn, PinName pin_pwm, PinName pin_iLed);
    int setILed(float val);
    int setPWM(bool on);
    int on(void);
    int off(void);
};

/*** Testfunktion ***/

void LEDdriverTask(void);

#endif //LEDDRIVER_H
