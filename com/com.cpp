#include "mbed.h"
#include "com.h"
#include "BSP/BSP.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "Radio.h"
#include "DeviceStats.h"
#include "logprintf.h"

extern Serial pc;
extern Radio radio;
extern BatteryManager bat;
extern LEDdriver L1;
extern LEDdriver L2;
extern Thread LEDdriverThread;
extern float BoxTemperature;

 Com::Com(bool debug){
     _debug = debug;
}

void Com::OOK(){
    static int i = 0;
    const uint32_t bitmask = 0x1;

    if( ((bitmask<<i) & this->ook_pattern) > 0 ){
        L1.setPWM(true);
        L2.setPWM(true);
    }
    else{
        L1.setPWM(false);
        L2.setPWM(false);
    }
    i = (i + 1) % 32; /* 32 Bit Pattern */
}

void Com::printLaserSettings(){
    xprintf("Laser1on: %d \t\tLaser2on: %d \nLaser1dc: %4.1f \tLaser2dc: %4.1f \nOOKenable: %d \t\tOOKpattern: %08x \tOOKfreq: %5.1f\n\n",\
    lasersettings.Laser1on, \
    lasersettings.Laser2on, \
    lasersettings.Laser1dutycycle, \
    lasersettings.Laser2dutycycle, \
    lasersettings.OOKenable, \
    lasersettings.OOKpattern, \
    lasersettings.OOKfrequency);
}

void Com::writeLaserSettings(LEDdriver *L1, LEDdriver *L2){
    L1->setILed(lasersettings.Laser1dutycycle);
    L2->setILed(lasersettings.Laser2dutycycle);

    /* normal mode */
    if(lasersettings.Laser1on){
        L1->on();
        L1->setPWM(true);
    }
    else{
        L1->off();
    }

    if(lasersettings.Laser2on){
        L2->on();
        L2->setPWM(true);
    }
    else{
        L2->off();
    }

    /* OOK mode: controls pwm pin of led driver */
    if(lasersettings.OOKenable){
        float freq = lasersettings.OOKfrequency;
        if(freq > 1000.0){
            freq = 1000.0;
        }
        if(freq < 0.1){
            freq = 0.1;
        }
        ook_pattern = lasersettings.OOKpattern;
        float TZyklus = 1.0/freq;
        OOK_ISR.attach(callback(this, &Com::OOK), TZyklus);
    }
    else{
        OOK_ISR.detach();
    }    
}


int Com::updateLaserSettings(uint8_t * data, int len){
    if(len >= (int)sizeof(LaserSetting_t)){
        LaserSetting_t *ls = (LaserSetting_t*)(data); // +1 for ID byte 
        memcpy(&lasersettings,ls,sizeof(LaserSetting_t));
        if(lasersettings.packetID == 129){
            if(_debug){
                printLaserSettings();
            }
            writeLaserSettings(&L1,&L2);
        }            
        else{
            printf("error: packetID = %d\n",lasersettings.packetID);
            // printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));
        }
    }
    else
        printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));

    return 0;
}


void Com::printHKD(){
        xprintf("Ubat=%7.4f\nIbat=%7.4f\nTbat=%7.4f\nUpv=%7.4f\nIload=%7.4f\nTbox=%7.4f\nHeaterstate=%d\nResetcount=%d\nRSSI_GS=%7.4f\nRSSI_Box=%7.4f\nBSR=%7.4f\nSOC=%7.4f\n\n", \
            hkd.batteryVoltage, \
            hkd.batteryCurrent,  \
            hkd.batteryTemperature, \
            hkd.pVVoltage, \
            hkd.loadCurrent, \
            hkd.boxTemperature, \
            hkd.heaterState, \
            hkd.mCUReset, \
            hkd.rSSI_GS, \
            hkd.rSSI_Box, \
            hkd.batteryResistance, \
            hkd.coloumbcounter);
}

int Com::sendHKD(uint8_t *data, uint32_t maxlen){
    hkd.id = 128;
    hkd.batteryVoltage = bat.data.voltage;
    hkd.batteryCurrent = bat.data.current;
    hkd.batteryTemperature = bat.data.temperature;
    hkd.pVVoltage = bat.data.pvvoltage;
    hkd.loadCurrent = bat.data.chargecurrent;
    hkd.boxTemperature = BoxTemperature;
    hkd.heaterState = getheaterstate();
    hkd.mCUReset = getPowerOnReset();
    hkd.rSSI_GS = 0.0;
    hkd.rSSI_Box = (float)radio.rssi;
    hkd.batteryResistance = bat.data.batteryresistance;
    hkd.coloumbcounter = bat.data.stateofcharge;

    if(maxlen >= sizeof(hkd)){
        memcpy(data,&hkd,sizeof(hkd));
        return sizeof(hkd);
    }
    else
        return 0;
}