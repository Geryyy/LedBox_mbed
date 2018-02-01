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


/* LTC4015 register */

// rw
#define R_ICHARGE_TARGET 0x1A
#define R_VCHARGE_SETTING 0x1B

// read
#define R_VBAT 0x3A
#define R_VIN  0x3B
#define R_VSYS 0x3C
#define R_IBAT 0x3D
#define R_IIN  0x3E
#define R_DIE_TEMP 0x3F
#define R_NTC_RATIO 0x40
#define R_BSR 0x41

#define R_MEAS_SYS_VALID 0x4A



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