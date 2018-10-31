/* @file: BatteryManager.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class implementation
 */

#include "BatteryManager.h"
#include "mbed.h"
#include "BSP.h"
#include "math.h"

BatteryManager::BatteryManager(int addr, PinName SDA, PinName SCL, PinName SMBAlert, float BatCapacity_Ah, bool debug){
    _debug = debug;
    _i2c = new I2C(SDA,SCL);
    _Alert = new InterruptIn(SMBAlert);
    _devAddr = addr;
    // _queue = new EventQueue(32 * EVENTS_EVENT_SIZE);
    // _t = new Thread();

    _R_SNSI = 0.01;
    _R_SNSB = 0.01;
    _cellcount = 1;
    _bat_capacity_As = BatCapacity_Ah * 3600;

    // _t->start(callback(_queue, &EventQueue::dispatch_forever));
    _Alert->fall(callback(this, &BatteryManager::_serviceSMBAlert));
    // _Alert->fall(callback(this, &BatteryManager::serviceSMBAlert));

    forceMeasSysOn();   /* Errata workaround */
    setChargerParameter();
    setInputThresholds();
    // forceMeasSysOn();
    setCoulombCounterPrescaler();
    enableCoulombCounter();
    suspendCharger(false);
    
}


void BatteryManager::controller(float TZyklus){
    const float Temp_low = 1.0; // [°C] heater is on
    const float Temp_high = 3.0; // [°C] heater is off
	const float T_bsr = 60.0; // [s] run bsr measurement
	const float T_error = 2.0; // [s] time in error state
    const float Ubat_min = 1.0; // [V] .. detect if bat is missing or broken

    const uint16_t charger_state = getChargerState();
	const uint16_t iabsorb_charge = (charger_state & 0x200) >> 9;
	const uint16_t iprecharge = (charger_state & 0x80) >> 7;
    const uint16_t icc_cv_charge = (charger_state & 0x40) >> 6;
    const uint16_t ibatmissing = (charger_state & 0x02) >> 1;
    const uint16_t ibatshort = (charger_state & 0x01) >> 0;
    const uint16_t icharging = iabsorb_charge + iprecharge + icc_cv_charge;
 
    const float iTempBat = getBatTemp();
    const float iUbat = getUBat();

	static float t = 0.0;
	static state_t state = INIT;
	static state_t mstate = INIT;



	do{
#if LIBRE_DEBUG
		printf("t = %f\n",t);
#endif

		if(state == INIT){
			state = READY;
            // set charger parameter

		}

        /* error handling */
        else if(ibatmissing > 0 || ibatshort > 0){
            state = ERR;
        }
        else if(state == ERR && t > T_error){
            state = READY;
        }

        /* normal operation */
        else if(state == READY && iUbat < Ubat_min){
            // check if battery is present via measured battery voltage
            state = BATMISSING;
        }
        else if(state == BATMISSING && t>0.0){
            state = READY;
        }
        else if(state == READY && iTempBat < Temp_low){
            state = HEAT;
        }
        else if(state == READY && iTempBat > Temp_high && icharging > 0){
            state = CHARGE;
        }
		else if(state == HEAT && iTempBat > Temp_high){
            state = READY;
        }
        else if(state == CHARGE && !icharging){
            state = READY;
            setStateOfCharge(1.0); // battery if fully charged at this state
        }
        else if(state == CHARGE && t > T_bsr){
            state = RUN_BSR;
            runBSR();
        }
        else if(state == RUN_BSR && t>0){
            state = CHARGE;
            bsr = getBatRes();
            if(_debug){
                printf("bsr: %f\n\n",bsr);
            }
        }
		//else;

		/* time in current state */
		if(state != mstate)
			t = 0.0;
		else
			t = t + TZyklus;

		/* update state marker */
		mstate = state;

	}while(state != mstate);

    /* state dependent actions */
	if(state == ERR){
		suspendCharger(true);
	}

    if(state == HEAT){
        suspendCharger(true);
    }

    if(state == READY){
        suspendCharger(false);
    }

    /* update measurement values */
    soc = getStateOfCharge();
    
    /* debug */
    if(_debug){
        printf("battery controller\n\tstate: %d\n\tsoc:  %f\n\n",state,soc);
        // printf("iTempBat: %f\n icharging: %d\n\n",iTempBat, icharging);
        // printf("icc_cv_charge: %d\n ",icc_cv_charge);
        // printf("iabsorb_charge  : %d\n ",iabsorb_charge);
        // printf("iprecharge: %d\n ",iprecharge);
        
    }

}


void BatteryManager::_serviceSMBAlert(){
    alertevent = true;
}


int BatteryManager::write(char reg, int16_t data){
    char tx[3] = {  reg, \
                    (char)((data>>0)&0xFF), /* LB */
                    (char)((data>>8)&0xFF) }; /* HB */
    int error = 0;
    error += (_i2c->write(_devAddr, tx, 3, false));
    if (error == 0)
        return SUCCESS;
    else 
        return ERROR;
}

int BatteryManager::read(char reg, int16_t *rxdata){
    char rx[2] = { 0x00, 0x00 };
    int error = SUCCESS;
    error += _i2c->write(_devAddr, &reg, 1, false); // register waehlen
    error += _i2c->read(_devAddr,rx,2,false); //register auslesen
    *rxdata = (int16_t)((rx[1]<<8) + rx[0]);
    if (error == 0)
        return SUCCESS;
    else 
        return ERROR;
}

int BatteryManager::suspendCharger(bool suspend){
    uint16_t data = 0x0000;

    if(suspend){
        this->read(CONFIG_BITS, (int16_t*)&data);
        data |= suspend_charger;
        this->write(CONFIG_BITS, (int16_t)data);
        return SUCCESS;
    }
    else{
        this->read(CONFIG_BITS, (int16_t*)&data);
        data &= ~(suspend_charger);
        this->write(CONFIG_BITS, (int16_t)data);
        return SUCCESS;
    }
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

int BatteryManager::setIcharge(float Icharge){
    if(Icharge > 0.0 && Icharge <= 32.0*0.001/_R_SNSB){ // 0.0 < Icharge <= 3.2A
        uint16_t icharge_targ = uint16_t(Icharge*_R_SNSB/0.001 );
        this->write(ICHARGE_TARGET,icharge_targ);

        // printf("setIcharge(): ICHARGE_TARGET = %d\n",icharge_targ);

        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getIcharge(){
    float Icharge = 0.0;
    int16_t icharge_target;
    this->read(ICHARGE_TARGET, &icharge_target);
    Icharge = (float)(icharge_target+1)*0.001/_R_SNSB;
    return Icharge;
}


int BatteryManager::setVcharge(float U){
    if(U>3.4125 && U<4.2){
        int16_t vcharge_setting = int16_t((U-3.4125)*80.0);
        vcharge_setting &= 0x1F; // Bits(4:0)
        this->write(VCHARGE_SETTING,vcharge_setting);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getVcharge(){
    int16_t vcharge = 0;
    this->read(VCHARGE_SETTING, &vcharge);
    return 3.4125 + (float)vcharge / 80.0;
}

int BatteryManager::setMaxCVTime(float hours){
    if(hours > 0.0 && hours < 18.0){
        uint16_t max_cv_time = uint16_t(hours*3600.0);
        this->write(MAX_CV_TIME, max_cv_time);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getMaxCVTime(){
    int16_t maxcvtime = 0;
    this->read(MAX_CV_TIME, &maxcvtime);
    return (float)maxcvtime /3600.0;
}

int BatteryManager::setMaxChargeTime(float hours){
    if(hours > 0.0 && hours < 18.0){
        uint16_t max_charge_time = uint16_t(hours*3600.0);
        this->write(MAX_CHARGE_TIME, max_charge_time);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getMaxChargeTime(){
    int16_t maxchargetime = 0;
    this->read(MAX_CHARGE_TIME, &maxchargetime);
    return (float)maxchargetime /3600.0;
}

int BatteryManager::setLIFEPO4RechargeThreshold(float U){
    if(U > 0 && U < 4.2){
        int16_t lifepo45_recharge_theshold = int16_t(U/0.000192264);
        this->write(LIFEP04_RECHARGE_THRESHOLD, lifepo45_recharge_theshold);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getLIFEPO4RechargeThreshold(){
    int16_t lifepo45_recharge_theshold = 0;
    this->read(LIFEP04_RECHARGE_THRESHOLD, &lifepo45_recharge_theshold);
    return 0.000192264 * (float)lifepo45_recharge_theshold;
}


int BatteryManager::setIinLimit(float Iin){
    if(Iin > 0 && Iin <= 3.2){
        uint16_t iin_limit_setting = uint16_t(Iin * _R_SNSI / 0.0005);
        iin_limit_setting &= 0x3F;
        this->write(IIN_LIMIT_SETTING, iin_limit_setting);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getIinLimit(){
    int16_t iin_limit = 0;
    this->read(IIN_LIMIT_SETTING, &iin_limit);
    return 0.0005 / _R_SNSI * (float)iin_limit;
}

int BatteryManager::setUVCL(float Uin){
    if(Uin > 0.0 && Uin <= 36.0){
        float R2B = 294.0; // kohm
        float R4B = 10.0; // kohm
        float Uvcl_pin = Uin * R4B / (R2B + R4B);
        uint16_t vin_uvcl_setting = uint16_t(Uvcl_pin/0.0046875 - 1.0);
        vin_uvcl_setting &= 0xFF;
        this->write(VIN_UVCL_SETTING, vin_uvcl_setting);

        // printf("setUVCL: %d\n",vin_uvcl_setting);
        return SUCCESS;
    }
    else
        return ERROR;
}

float BatteryManager::getUVCL(){
    float R2B = 294.0; // kohm
    float R4B = 10.0; // kohm
    int16_t uvcl = 0;
    this->read(VIN_UVCL_SETTING, &uvcl);
    float Uvcl_pin = 0.0046875 * (float)(uvcl + 1);
 
    return Uvcl_pin * (R2B + R4B) / R4B;
}


int BatteryManager::setChargerParameter(){
    // ICHARGE_TARGET;
    setIcharge(1.0);
    // VABSORB_DELTA;
    // MAX_ABSORB_TIME;
    // VCHARGE_SETTING;
    setVcharge(3.6); // max ladespannung 3.6V
    // MAX_CV_TIME;
    setMaxCVTime(1.0); // max 1h CV charge
    // en_jeita;
    // en_c_over_x_term;
    // C_OVER_X_THRESHOLD;
    // MAX_CHARGE_TIME;
    setMaxChargeTime(4);
    // LIFEPO4_RECHARGE_THRESHOLD;
    setLIFEPO4RechargeThreshold(3.2);

    return SUCCESS;
}

int BatteryManager::setInputThresholds(){
    // IIN_LIMIT_SETTING;
    setIinLimit(2.0); // 2A max Eingangsstrom
    // VIN_UVCL_SETTING;
    setUVCL(8.0); // Uin min 8V
    return SUCCESS;
}

int BatteryManager::forceMeasSysOn(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS, (int16_t*)&data);
    data |= force_meas_sys_on;
    this->write(CONFIG_BITS, (int16_t)data);
    return SUCCESS;
}

int BatteryManager::forceMeasSysOff(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS, (int16_t*)&data);
    data &= ~(force_meas_sys_on);
    this->write(CONFIG_BITS, (int16_t)data);
    return SUCCESS;
}

int BatteryManager::setLimitAlert(int16_t alert){
    uint16_t data = 0x0000;
    this->read(EN_LIMIT_ALERTS, (int16_t*)&data);
    data |= alert;
    this->write(EN_LIMIT_ALERTS, data);
    return SUCCESS;
}

int BatteryManager::clearLimitAlert(int16_t alert){
    uint16_t data = 0x0000;
    this->read(EN_LIMIT_ALERTS, (int16_t*)&data);
    data &= ~alert;
    this->write(EN_LIMIT_ALERTS, data);
    return SUCCESS;
}

bool BatteryManager::getLimitAlert(int16_t alert){
    uint16_t data = 0x0000;
    this->read(EN_LIMIT_ALERTS, (int16_t*)&data);
    data &= alert;
    if(data == alert)
        return true;
    else 
        return false;
}

int BatteryManager::sampleMeasSys(){
    // en meas sys alert
    setLimitAlert(en_meas_sys_valid_alert);
    // force meas sys on
    forceMeasSysOn();
    // wait for smbalert
    int t = 0;
    while(alertevent == false){
        wait_ms(1);
        t++;
        if(t>200)
            break;
    }
    
    alertevent = false;
    // verify meas sys alert
    if(getLimitAlert(en_meas_sys_valid_alert))
        printf("en_meas_sys_valid_alert is true\n");
    else
        printf("en_meas_sys_valid_alert is false");

    // disable meas sys alert
    clearLimitAlert(en_meas_sys_valid_alert);
    // disable force meas sys
    forceMeasSysOff();
    // read updated meas sys data from ltc4015
    return SUCCESS;
}

/*** coulomb counter ***/
float BatteryManager::q_lsb(){
    return _qcount_prescaler / (K_QC * _R_SNSB); // amount of charge in As represented by the LSB
}

int BatteryManager::setCoulombCounterPrescaler(){
    float q_lsb_max = _bat_capacity_As / 65535.0;
    float prescaler = q_lsb_max * K_QC * _R_SNSB * 2.0; // times 2 for safety margin
    _qcount_prescaler = (uint16_t) round(prescaler);
    this->write(QCOUNT_PRESCALE_FACTOR,_qcount_prescaler);
    
    // printf("QCOUNT_PRESCALE_FACTOR: %d\n",_qcount_prescaler);

    return SUCCESS;
}

int BatteryManager::setStateOfCharge(float SOC){
    if(SOC > 1.0 || SOC < 0.0)
        return ERROR;
    uint16_t qcount =  uint16_t((32768.0 * SOC) + 16384.0);
    this->write(QCOUNT,qcount);
    return SUCCESS;
}

float BatteryManager::getStateOfCharge(){
    uint16_t qcount;
    this->read(QCOUNT, (int16_t*)&qcount);
    return ((float)qcount - 16384.0) / 32768.0;
}

int  BatteryManager::enableCoulombCounter(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS, (int16_t*)&data);
    data |= en_qcount;
    this->write(CONFIG_BITS, data);
    return SUCCESS;
}

int BatteryManager::disableCoulombCounter(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS, (int16_t*)&data);
    data &= ~(en_qcount);
    this->write(CONFIG_BITS, data);
    return SUCCESS;
}

/*** BSR ***/

int BatteryManager::runBSR(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS, (int16_t*)&data);
    data |= run_bsr;
    this->write(CONFIG_BITS, data);
    return SUCCESS;
}

float BatteryManager::getBatRes(){
    int16_t bsr;
    this->read(BSR, &bsr);
    return (float)bsr * _R_SNSB / 500.0 * (float)_cellcount;
}


/*** LTC4015 status and config ***/

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


uint16_t BatteryManager::getChargerConfig(){
    uint16_t data = 0x0000;
    this->read(CHARGER_CONFIG_BITS,(int16_t*)&data);
    return data & 0x0007; // Bits 0,1,2
}

uint16_t BatteryManager::getConfig(){
    uint16_t data = 0x0000;
    this->read(CONFIG_BITS,(int16_t*)&data);
    return data; // Bits 0,..,8
}

/* Bit 8..11 selected chemistry
 * Bit 4..7  reserved
 * Bit 0..3  cell count
 */
uint16_t BatteryManager::getChemCells(){
    uint16_t data = 0x0000;
    this->read(CHEM_CELLS,(int16_t*)&data);
    return data; // Bits 0,..,8
}

void BatteryManager::printStatus(){
    uint16_t charger_state = getChargerState();
    uint16_t charger_status = getChargerStatus();
    uint16_t system_status = getSystemStatus();
    uint16_t charger_config = getChargerConfig();
    uint16_t config = getConfig();
    uint16_t chemcells = getChemCells();

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

    printf("Charger Config Bits:\nen_c_over_x_term:\t%d\nen_lead_acid_temp_comp:\t%d\nen_jeita:\t\t%d\n\n",\
            (charger_config & 0x0004) >> 2, \
            (charger_config & 0x0002) >> 1, \
            (charger_config & 0x0002) >> 0 );

    printf("Config Bits:\nsuspend_charger:\t%d\nrun_bsr:\t\t%d\nforce_meas_sys_on:\t%d\nmppt_en_i2c:\t\t%d\nen_qcount:\t\t%d\n\n",\
            (config & 0x0100) >> 8, \
            (config & 0x0020) >> 5, \
            (config & 0x0010) >> 4, \
            (config & 0x0008) >> 3, \
            (config & 0x0004) >> 2 );             
                        
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

    printf("Chemistry Selection:\t");
    switch((chemcells >> 8) & 0x0F){
        case 0: printf("Li-Ion Programmable\n"); break;
        case 1: printf("Li-Ion Fixed 4.2V/cell\n"); break;
        case 2: printf("Li-Ion Fixed 4.1V/cell\n"); break;
        case 3: printf("Li-Ion Fixed 4.0V/cell\n"); break;
        case 4: printf("LiFePo4 Programmable\n"); break;
        case 5: printf("LiFePo4 Fixed Fast Charge\n"); break;
        case 6: printf("LiFePo4 Fixed 3.6V/cell\n"); break;
        case 7: printf("Lead-acid Fixed Programmable\n"); break;
        case 8: printf("Lead-acid Programmable\n"); break;
        default: printf("unknown chemistry\n"); break;
    }
    printf("\n");

    printf("Cell Count:\t\t%d\n\n", chemcells & 0x0F);
}


