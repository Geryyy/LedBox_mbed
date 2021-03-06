/* @file: BatteryManager.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: BatteryManager class definition
 */

#ifndef BATTERYMANAGER_H
#define BATTERYMANAGER_H

#include "mbed.h"
#include "mbed_events.h"

#define LTC4015_ADDR (0xD0)
#define ALERTADDR (0x19)


/* LTC4015 register */

// read
#define VBAT 0x3A
#define VIN  0x3B
#define VSYS 0x3C
#define IBAT 0x3D
#define IIN  0x3E
#define DIE_TEMP 0x3F
#define NTC_RATIO 0x40
#define BSR 0x41

#define R_MEAS_SYS_VALID 0x4A

// control register
#define IIN_LIMIT_SETTING 0x15
#define VIN_UVCL_SETTING  0x16
#define ARM_SHIP_MODE 0x19
#define ICHARGE_TARGET 0x1A
#define VCHARGE_SETTING 0x1B
#define C_OVER_X_THRESHOLD 0x1C
#define MAX_CV_TIME 0x1D
#define MAX_CHARGE_TIME 0x1E

#define CHARGER_CONFIG_BITS 0x29

#define VABSORB_DELTA 0x2A
#define MAX_ABSORB_TIME 0x2B
#define VEQUALIZE_DELTA 0x2C
#define EQUALIZE_TIME 0x2D
#define LIFEP04_RECHARGE_THRESHOLD 0x2E

#define MAX_CHARGE_TIMER 0x30
#define CV_TIMER 0x31
#define ABSORB_TIMER 0x32
#define EQUALIZE_TIMER 0x33

#define CHARGER_STATE 0x34
#define CHARGE_STATUS 0x35
#define LIMIT_ALERTS 0x36
#define CHARGER_STATE_ALERTS 0x37
#define CHARGE_STATUS_ALERTS 0x38
#define SYSTEM_STATUS 0x39

// Coulomb Counter
#define QCOUNT_LO_ALERT_LIMIT 0x10
#define QCOUNT_HI_ALERT_LIMIT 0x11
#define QCOUNT_PRESCALE_FACTOR 0x12
#define QCOUNT 0x13
#define CONFIG_BITS 0x14

// BSR
#define BSR 0x41

// CONFIG_BITS bitdefinition
#define suspend_charger     0x0100 
#define run_bsr             0x0020
#define force_meas_sys_on   0x0010
#define mppt_en_i2c         0x0008
#define en_qcount           0x0004

#define K_QC 8333.33

// Chemistry selector and cell count readout
#define CHEM_CELLS 0x43

// alerts
#define EN_LIMIT_ALERTS 0x0D

#define en_meas_sys_valid_alert 0x8000
#define en_qcount_low_alert     0x2000
#define en_qcount_high_alert    0x1000
#define en_vbat_lo_alert        0x0800
#define en_vbat_hi_alert        0x0400
#define en_vin_lo_alert         0x0200
#define en_vin_hi_alert         0x0100
#define en_vsys_lo_alert        0x0080
#define en_vsys_hi_alert        0x0040
#define en_iin_hi_alert         0x0020
#define en_ibat_lo_alert        0x0010
#define en_die_temp_hi_alert    0x0008
#define en_bsr_hi_alert         0x0004
#define en_ntc_ratio_hi_alert   0x0002
#define en_ntc_ratio_lo_alert   0x0001


typedef enum state_e{
	ERR = 0,
    INIT,
	READY,
    BATMISSING,
	HEAT,
	CHARGE,
	RUN_BSR
} state_t;


typedef struct batdata_s{
    float voltage;
    float current; 
    float temperature;
    float pvvoltage;
    float sysvoltage;
    float chargecurrent;
    float inputcurrent;
    float dietemperature;
    float batteryresistance;
    float stateofcharge;
}batdata_t;



class BatteryManager{
private:
    bool _debug;
    I2C *_i2c;
    InterruptIn *_Alert;
    volatile bool alertevent;
    int _devAddr;
    float _R_SNSI;
    float _R_SNSB;
    int _cellcount;
    uint16_t _qcount_prescaler;
    float _bat_capacity_As;
    void _serviceSMBAlert();
    

public:
    batdata_t data;


public: 
    BatteryManager(int addr, PinName SDA, PinName SCL, PinName SMBAlert, float BatCapacity_Ah, bool debug);
    void controller(float TZyklus);
    void printStatus();

private:
    int write(char reg, int16_t data);
    int read(char reg, int16_t *rxdata);
    int suspendCharger(bool suspend);

public:
    float getBatTemp();
    float getUBat();
    float getIBat();
    float getUin();
    float getUsys();
    float getIin();
    float getTdie();
    
    int setIcharge(float Icharge);
    float getIcharge();
    int setVcharge(float U);
    float getVcharge();
    int setMaxCVTime(float hours);
    float getMaxCVTime();
    int setMaxChargeTime(float hours);
    float getMaxChargeTime();
    int setLIFEPO4RechargeThreshold(float U);
    float getLIFEPO4RechargeThreshold();
    int setIinLimit(float Iin);
    float getIinLimit();
    int setUVCL(float Uin);
    float getUVCL();
    int setChargerParameter();
    int setInputThresholds();
    int forceMeasSysOn();
    int forceMeasSysOff();
    int setLimitAlert(int16_t alert);
    int clearLimitAlert(int16_t alert);
    bool getLimitAlert(int16_t alert);
    int sampleMeasSys();

    float q_lsb();
    int setCoulombCounterPrescaler();
    int setStateOfCharge(float SOC);
    float getStateOfCharge();
    int  enableCoulombCounter();
    int disableCoulombCounter();

    int runBSR();
    float getBatRes();

private:
    uint16_t getChargerStatus();
    uint16_t getChargerState();
    uint16_t getSystemStatus();
    uint16_t getChargerConfig();
    uint16_t getConfig();
    uint16_t getChemCells();

};

#endif // BATTERYMANAGER_H