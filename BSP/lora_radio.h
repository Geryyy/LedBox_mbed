
#ifndef LORA_RADIO_H
#define LORA_RADIO_H

#define LORA_BAUD 57600

void RadioTask();

class LoraRadio{
private:
    UARTSerial *_serial;
    ATCmdParser *_parser;
    int fw[3];
    int day, year, hour, min, sec;
    char month[4];
    int u_vdd[4]; // RN2483 VDD in [mV]

public: 
    LoraRadio(PinName PinTX, PinName PinRX, int baud, int debug);
    void printFwVersion();
    void setSleep(int ms);
    int reset();
    int getVDD();
    int send(char *data, int len);

private:
    int getFwVersion();
};

#endif
