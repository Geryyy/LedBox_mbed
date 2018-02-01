/* @file: BatteryManager.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class definition
 */

#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include "mbed.h"

#define LTC4015_ADDR (0xD0)
#define ALERTADDR (0x19)


class BatteryManager{
private:
    I2C *_i2c;
    InterruptIn *_Alert;
    int _devAddr;

public: 
    BatteryManager(int addr, PinName SDA, PinName SCL, PinName SMBAlert);
    void serviceSMBAlert();
    int write(char reg, uint16_t data);
    int read(char reg, uint16_t *rxdata);
};

/*** Testfunktion ***/

void BatteryTask(void);

#endif // BATTERYMANAGER_H