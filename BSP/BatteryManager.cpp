/* @file: BatteryManager.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class implementation
 */

#include "BatteryManager.h"
#include "mbed.h"

BatteryManager::BatteryManager(PinName SDA, PinName SCL, PinName SMBAlert){
    i2c = new I2C(SDA,SCL);
    Alert = new InterruptIn(SMBAlert);
    Alert->fall(&(this->serviceSMBAlert));
}


void BatteryManager::serviceSMBAlert(){

}