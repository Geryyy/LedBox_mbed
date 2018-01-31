/* @file: BatteryManager.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class definition
 */

#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include "mbed.h"

class BatteryManager{
private:
    I2C *i2c;
    InterruptIn *Alert;

public: 
    BatteryManager(PinName SDA, PinName SCL, PinName SMBAlert);
    serviceSMBAlert();

};


#endif // BATTERYMANAGER_H