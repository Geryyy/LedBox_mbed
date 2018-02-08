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

    _devAddr = addr;
    _queue = new EventQueue(32 * EVENTS_EVENT_SIZE);
    _t = new Thread();

    _R_SNSI = 0.01;
    _R_SNSB = 0.01;
    _cellcount = 1;

    _t->start(callback(_queue, &EventQueue::dispatch_forever));
    _Alert->fall(_queue->event(BatteryManager::_serviceSMBAlert));
    // _Alert->fall(callback(this, &BatteryManager::serviceSMBAlert));
}


void BatteryManager::_serviceSMBAlert(){
    printf("\n\nSMBALERT MESSAGE!!\n\n");
}


int BatteryManager::write(char reg, int16_t data){
    char tx[3] = {  reg, \
                    (char)((data>>0)&0xFF), /* LB */
                    (char)((data>>8)&0xFF) }; /* HB */
    int error = 0;
    error += (_i2c->write(_devAddr, tx, 4, false));
    if (error == 0)
        return SUCCESS;
    else return ERROR;
}

int BatteryManager::read(char reg, int16_t *rxdata){
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
    int16_t ntc_ratio = 0;
    this->read(NTC_RATIO, &ntc_ratio);
    float R_ntcbias = 10000.0; // 10 kOhm
    float RT = ntc_ratio * R_ntcbias/(21845.0 - ntc_ratio);
    const float RN = 10000.0; // 10 kOhm
    const float B = 3435; // [K]
    const float TN = 298.15; // [K] .. 25°C
    float T = (B*TN)/(TN*log(RT/RN)+B);
    return T - 273.15;
}

float BatteryManager::getUBat(){
    int16_t vbat = 0;
    this->read(VBAT, &vbat);
    return (float)vbat * 0.000192264 * (float)_cellcount;
}


float BatteryManager::getIBat(){
    int16_t ibat = 0;
    this->read(IBAT, &ibat);
    return ((float)ibat * 0.00000146487 / _R_SNSB);
}

float BatteryManager::getUin(){
    int16_t vin = 0;
    this->read(VIN, &vin);
    return (float)vin * 0.001648;
}

float BatteryManager::getUsys(){
    int16_t vsys = 0;
    this->read(VSYS, &vsys);
    return (float)vsys * 0.001648;
}

float BatteryManager::getIin(){
    int16_t iin = 0;
    this->read(IIN, &iin);
    return (float)iin * 0.00000146487 / _R_SNSI;
}

float BatteryManager::getTdie(){
    int16_t tdie = 0;
    this->read(DIE_TEMP, &tdie);
    return ((float)tdie - 12010.0)/45.6;
}

float BatteryManager::getBatRes(){
    int16_t bsr;
    this->read(BSR, &bsr);
    return (float)bsr * _R_SNSB / 500.0 * (float)_cellcount;
}

int BatteryManager::setIchargeRel(float Icharge_rel){
    int16_t icharge_target = 0;
    if(Icharge_rel >= 0.0 && Icharge_rel <= 1.0){
        icharge_target = (uint16_t)(32.0 * Icharge_rel);
        this->write(ICHARGE_TARGET,icharge_target);
        return SUCCESS;
    }
    else{
        return ERROR;
    }
}

float BatteryManager::getIchargeRel(){
    float IchargeRel = 0.0;
    int16_t icharge_target;
    this->read(ICHARGE_TARGET, &icharge_target);
    IchargeRel = (float)icharge_target / 32.0;
    return IchargeRel;
}


int BatteryManager::updateChargerSettings(){
    // ICHARGE_TARGET;
    // VABSORB_DELTA;
    // MAX_ABSORB_TIME;
    // VCHARGE_SETTING;
    // MAX_CV_TIME;
    // en_jeita;
    // en_c_over_x_term;
    // C_OVER_X_THRESHOLD;
    // MAX_CHARGE_TIME;
    // LIFEPO4_RECHARGE_THRESHOLD;
    return SUCCESS;
}

int BatteryManager::setInputThresholds(){
    // IIN_LIMIT_SETTING;
    // VIN_UVCL_SETTING;
    return SUCCESS;
}

uint16_t BatteryManager::getChargerStatus(){
    uint16_t data = 0x0000;
    this->read(CHARGE_STATUS, (int16_t*)&data);
    return data;
}

uint16_t BatteryManager::getChargerState(){
    uint16_t data = 0x0000;
    this->read(CHARGER_STATE, (int16_t*)&data);
    return data;
}

uint16_t BatteryManager::getSystemStatus(){
    uint16_t data = 0x0000;
    this->read(SYSTEM_STATUS, (int16_t*)&data);
    return data;

}

void BatteryManager::printStatus(){
    uint16_t charger_state = getChargerState();
    uint16_t charger_status = getChargerStatus();
    uint16_t system_status = getSystemStatus();

    printf("\nLTC4015 Status Print:\n ------------------------\n");
    printf("Charge Status:\nVIN_UVCL_ACTIVE:\t%d\nIIN_LIMIT_ACTIVE:\t%d\nCONTANT_CURRENT:\t%d\nCONSTANT_VOLTAGE:\t%d\n\n", \
            (charger_status & 0x08) >> 3, \
            (charger_status & 0x04) >> 2, \
            (charger_status & 0x02) >> 1, \
            (charger_status & 0x01) >> 0 );
    printf("Charger State:\nequalize_charge:\t%d\nabsorb_charge:\t\t%d\ncharger_suspended:\t%d\nprecharge:\t\t%d\ncc_cv_charge:\t\t%d\ntc_pause:\t\t%d\ntimer_term:\t\t%d\nc_over_x_term:\t\t%d\nmax_charge_time_fault:\t%d\nbat_missing_fault:\t%d\nbat_short_fault:\t%d\n\n", \
            (charger_state & 0x400) >> 10, \
            (charger_state & 0x200) >> 9, \
            (charger_state & 0x100) >> 8, \
            (charger_state & 0x80) >> 7, \
            (charger_state & 0x40) >> 6, \
            (charger_state & 0x20) >> 5, \
            (charger_state & 0x10) >> 4, \
            (charger_state & 0x08) >> 3, \
            (charger_state & 0x04) >> 2, \
            (charger_state & 0x02) >> 1, \
            (charger_state & 0x01) >> 0 );
    printf("System Status:\ncharger_enabled:\t%d\nmppt_en_pin:\t\t%d\nequalize_req:\t\t%d\ndrvcc_good:\t\t%d\ncell_count_error:\t%d\nok_to_charge:\t\t%d\nno_rt:\t\t\t%d\nthermal_shutdown:\t%d\nvin_ovlo:\t\t%d\nvin_gt_vbat:\t\t%d\nintvcc_gt_4p3v:\t\t%d\nintvcc_gt2p8v:\t\t%d\n\n", \
            (system_status & 0x2000) >> 13, \
            (system_status & 0x800) >> 11, \
            (system_status & 0x400) >> 10, \
            (system_status & 0x200) >> 9, \
            (system_status & 0x100) >> 8, \
            (system_status & 0x40) >> 6, \
            (system_status & 0x20) >> 5, \
            (system_status & 0x10) >> 4, \
            (system_status & 0x08) >> 3, \
            (system_status & 0x04) >> 2, \
            (system_status & 0x02) >> 1, \
            (system_status & 0x01) >> 0 );        
}


/*** Testfunktion ***/

void BatteryTask(){
    BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA);
    char reg = 0x00;
    int16_t rx = 0;
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