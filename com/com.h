#include "mbed.h"
#include "LEDdriver.h"


typedef struct LaserSetting_s{
    uint8_t packetID; /* 129 */
    bool Laser1on;
    bool Laser2on;
    float Laser1dutycycle;
    float Laser2dutycycle;
    bool OOKenable;
    uint32_t OOKpattern;
    float OOKfrequency;
}LaserSetting_t;

typedef struct hkd_s{
    uint8_t id;
    float batteryVoltage;
    float batteryCurrent;
    float batteryTemperature;
    float pVVoltage;
    float loadCurrent;
    float boxTemperature;
    uint8_t heaterState;
    uint32_t mCUReset;
    float rSSI_GS;
    float rSSI_Box;
    float batteryResistance;
    float coloumbcounter;
}  __attribute__ ((packed)) hkd_t;

class Com{

private:
    LaserSetting_t lasersettings;
    hkd_t hkd;

public:
    Com(void);
    int updateLaserSettings(uint8_t * data, int len);
    int sendHKD(uint8_t *data, uint32_t maxlen);
};