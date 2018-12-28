#include "mbed.h"
#include "com.h"
#include "BSP/BSP.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "RFM98W.h"
#include "DeviceStats.h"

extern Serial pc;
// extern RFM98W radio;
extern BatteryManager bat;
extern LEDdriver L1;
extern LEDdriver L2;
extern Thread LEDdriverThread;

 Com::Com(){

}

void printLaserSettings(LaserSetting_t *ls){
    printf("Laser1on: %d \t\tLaser2on: %d \nLaser1dc: %f \tLaser2dc: %f \nOOKenable: %d \t\tOOKpattern: %ld \tOOKfreq: %f\n\n",\
    ls->Laser1on, \
    ls->Laser2on, \
    ls->Laser1dutycycle, \
    ls->Laser2dutycycle, \
    ls->OOKenable, \
    ls->OOKpattern, \
    ls->OOKfrequency);
}

void writeLaserSettings(LaserSetting_t *ls, LEDdriver *L1, LEDdriver *L2){
    L1->setILed(ls->Laser1dutycycle);
    L1->setPWM(0.0); // 0.0 -> constant on
    L2->setILed(ls->Laser2dutycycle);
    L2->setPWM(0.0); // 0.0 -> constant on

    if(ls->Laser1on)
        L1->on();
    else
        L1->off();

    if(ls->Laser2on)
        L2->on();
    else
        L2->off();
}


int Com::updateLaserSettings(uint8_t * data, int len){
    if(len >= (int)sizeof(LaserSetting_t)){
        LaserSetting_t *ls = (LaserSetting_t*)(data); // +1 for ID byte
        if(ls->packetID == 129){
            //printf("Lasersettings received\n");
            // printLaserSettings(ls);
            writeLaserSettings(ls,&L1,&L2);
        }            
        else{
            printf("error: packetID = %d\n",ls->packetID);
            // printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));
        }
    }
    else
        printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));

    return 0;
}

int Com::sendHKD(uint8_t *data, uint32_t maxlen){
    hkd.id = 128;
    hkd.batteryVoltage = bat.data.voltage;
    hkd.batteryCurrent = bat.data.current;
    hkd.batteryTemperature = bat.data.temperature;
    hkd.pVVoltage = bat.data.pvvoltage;
    hkd.loadCurrent = bat.data.chargecurrent;
    hkd.boxTemperature = bat.data.temperature;
    hkd.heaterState = getheaterstate();
    hkd.mCUReset = getPowerOnReset();
    hkd.rSSI_GS = 0.0;
    hkd.rSSI_Box = 0.0;
    hkd.batteryResistance = bat.data.batteryresistance;
    hkd.coloumbcounter = bat.data.stateofcharge;

    if(maxlen >= sizeof(hkd)){
        memcpy(data,&hkd,sizeof(hkd));
        return sizeof(hkd);
    }
    else
        return 0;
}