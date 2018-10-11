#include "mbed.h"
#include "com.h"
#include "LEDdriver.h"


 Com::Com(){

}

int Com::updateLaserSettings(LEDdriver *L1, LEDdriver *L2){

    return 0;
}

int Com::sendHKD(uint8_t *data, uint32_t maxlen){
    hkd.id = 128;
    hkd.batteryVoltage = 3.3;
    hkd.batteryCurrent = 0.15;
    hkd.batteryTemperature = 25.0;
    hkd.pVVoltage = 0.1;
    hkd.loadCurrent = 0.2;
    hkd.boxTemperature = 33.0;
    hkd.heaterState = 0;
    hkd.mCUReset = 12;
    hkd.rSSI_GS = 0.0;
    hkd.rSSI_Box = 0.0;
    hkd.batteryResistance = 0.0;
    hkd.coloumbcounter = 0.5;

    if(maxlen >= sizeof(hkd)){
        memcpy(data,&hkd,sizeof(hkd));
        return sizeof(hkd);
    }
    else
        return 0;
}