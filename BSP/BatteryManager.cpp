/* @file: BatteryManager.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class implementation
 */

#include "BatteryManager.h"
#include "mbed.h"
#include "BSP.h"

BatteryManager::BatteryManager(int addr, PinName SDA, PinName SCL, PinName SMBAlert){
    _i2c = new I2C(SDA,SCL);
    _Alert = new InterruptIn(SMBAlert);
    _Alert->fall(callback(this, &BatteryManager::serviceSMBAlert));
    _devAddr = addr;
}


void BatteryManager::serviceSMBAlert(){

}


int BatteryManager::write(char reg, uint16_t data){
    char tx[3] = {  reg, \
                    (char)((data>>8)&0xFF),
                    (char)((data>>0)&0xFF) };
    int error = 0;
    error += (_i2c->write(_devAddr, tx, 4, false));
    if (error == 0)
        return SUCCESS;
    else return ERROR;
}

int BatteryManager::read(char reg, uint16_t *rxdata){
    char rx[2] = { 0x00, 0x00 };
    int error = SUCCESS;
    error += _i2c->write(_devAddr, &reg, 1, false); // register waehlen
    error += _i2c->read(_devAddr,rx,2,false); //register auslesen
    *rxdata = (uint16_t)((rx[1]<<8) + rx[0]);
    if (error == 0)
        return SUCCESS;
    else return ERROR;
}

float BatteryManager::getBatTemp(){
    uint16_t NTC_ratio = 0;
    this.read(R_NTC_RATIO, &NTC_ratio);
    float RNTCBIAS = 10000.0; // 10 kOhm
    float RNTC = NTC_ratio • RNTCBIAS/(21845.0 – NTC_ratio);
    float B;
    float T = (B*TN)/(TN*ln(RT/RN)+B);
}


/*** Testfunktion ***/

void BatteryTask(){
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    char reg = 0x00;
    uint16_t rx = 0;
    while(true){
        bat.read(reg,&rx);
        printf("register: %x \tvalue: %hx\n", reg, rx);
        reg = (reg+1) % 0x50;
        wait(0.5);
    }

}