/* @file: BatteryManager.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class implementation
 */

#include "BatteryManager.h"
#include "mbed.h"
#include "BSP.h"
#include "math.h"

BatteryManager::BatteryManager(int addr, PinName SDA, PinName SCL, PinName SMBAlert){
    _i2c = new I2C(SDA,SCL);
    _Alert = new InterruptIn(SMBAlert);
    _Alert->fall(callback(this, &BatteryManager::serviceSMBAlert));
    _devAddr = addr;

    _R_SNSI = 0.01;
    _R_SNSB = 0.01;
    _cellcount = 1;
}


void BatteryManager::serviceSMBAlert(){

}


int BatteryManager::write(char reg, uint16_t data){
    char tx[3] = {  reg, \
                    (char)((data>>0)&0xFF), /* LB */
                    (char)((data>>8)&0xFF) }; /* HB */
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
    this->read(R_NTC_RATIO, &NTC_ratio);
    float RNTCBIAS = 10000.0; // 10 kOhm
    float RT = NTC_ratio * RNTCBIAS/(21845.0 - NTC_ratio);
    const float RN = 10000.0; // 10 kOhm
    const float B = 3435; // [K]
    const float TN = 298.15; // [K] .. 25°C
    float T = (B*TN)/(TN*log(RT/RN)+B);
    return T - 273.15;
}

float BatteryManager::getUBat(){
    uint16_t VBAT = 0;
    this->read(R_VBAT, &VBAT);
    return (float)VBAT * 0.000192264 * (float)_cellcount;
}


float BatteryManager::getIBat(){
    uint16_t IBAT = 0;
    this->read(R_IBAT, &IBAT);
    return ((float)IBAT * 0.00000146487 / _R_SNSB);
}

float BatteryManager::getUin(){
    uint16_t VIN = 0;
    this->read(R_VIN, &VIN);
    return (float)VIN * 0.001648;
}

float BatteryManager::getUsys(){
    uint16_t VSYS = 0;
    this->read(R_VSYS, &VSYS);
    return (float)VSYS * 0.001648;
}

float BatteryManager::getIin(){
    uint16_t IIN = 0;
    this->read(R_IIN, &IIN);
    return (float)IIN * 0.00000146487 / _R_SNSI;
}

float BatteryManager::getTdie(){
    uint16_t TDIE = 0;
    this->read(R_DIE_TEMP, &TDIE);
    return ((float)TDIE - 12010.0)/45.6;
}

float BatteryManager::getBatRes(){
    uint16_t BSR;
    this->read(R_BSR, &BSR);
    return (float)BSR * _R_SNSB / 500.0 * (float)_cellcount;
}

int BatteryManager::setIchargeRel(float Icharge_rel){
    uint16_t ICHARGE_TARGET = 0;
    if(Icharge_rel >= 0.0 && Icharge_rel <= 1.0){
        ICHARGE_TARGET = (uint16_t)(32.0 * Icharge_rel);
        this->write(R_ICHARGE_TARGET,ICHARGE_TARGET);
        return SUCCESS;
    }
    else{
        return ERROR;
    }
}

float BatteryManager::getIchargeRel(){
    float IchargeRel = 0.0;
    uint16_t ICHARGE_TARGET;
    this->read(R_ICHARGE_TARGET, &ICHARGE_TARGET);
    IchargeRel = (float)ICHARGE_TARGET / 32.0;
    return IchargeRel;
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

void BatteryTask2(){
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    
    bat.setIchargeRel(0.5);
    printf("\n\nIchargeRel: %f\n\n",bat.getIchargeRel());

    while(true){
        bat.setIchargeRel(0.5);
        printf("\n\nIchargeRel: %f\n\n",bat.getIchargeRel());

        printf("Tbat: %f\t", bat.getBatTemp());
        printf("Ubat: %f\t",bat.getUBat());
        printf("Ibat: %f\t", bat.getIBat());
        printf("Uin: %f\t", bat.getUin());
        printf("Usys: %f\t", bat.getUsys());
        printf("Iin: %f\t", bat.getIin());
        printf("Tdie: %f\t", bat.getTdie());
        printf("BatRes: %f\t",bat.getBatRes());
        printf("\n");
        wait(1);
    }
}