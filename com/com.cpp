#include "mbed.h"
#include "com.h"
#include "BSP/BSP.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "RFM98W.h"
#include "DeviceStats.h"

extern Serial pc;
extern RFM98W radio;
extern BatteryManager bat;
extern LEDdriver L1;
extern LEDdriver L2;
extern Thread LEDdriverThread;

 Com::Com(){

}

int Com::updateLaserSettings(uint8_t * data, int len){
    if(len >= sizeof(LaserSetting_t)){
        LaserSetting_t *ls = (LaserSetting_t*)data;
        if(ls->packetID == 129)
            printf("Lasersettings received\n");
        else
            printf("error: packetID = %d\n",ls->packetID);
            // printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));
    }
    else
        printf("error: len = %d, sizeof(LaserSetting_t) = %d\n",len,sizeof(LaserSetting_t));

    return 0;
}

int Com::sendHKD(uint8_t *data, uint32_t maxlen){
    hkd.id = 128;
    hkd.batteryVoltage = bat.getUBat();
    hkd.batteryCurrent = bat.getIBat();
    hkd.batteryTemperature = bat.getBatTemp();
    hkd.pVVoltage = bat.getUin();
    hkd.loadCurrent = bat.getIcharge();
    hkd.boxTemperature = getMCUTemp();
    hkd.heaterState = getheaterstate();
    hkd.mCUReset = getPowerOnReset();
    hkd.rSSI_GS = 0.0;
    hkd.rSSI_Box = 0.0;
    hkd.batteryResistance = bat.bsr;
    hkd.coloumbcounter = bat.soc;

    if(maxlen >= sizeof(hkd)){
        memcpy(data,&hkd,sizeof(hkd));
        return sizeof(hkd);
    }
    else
        return 0;
}