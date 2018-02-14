/* @file: lora_radio.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: header file for support module for lora module rn2483
 */

#ifndef LORA_RADIO_H
#define LORA_RADIO_H

#define LORA_BAUD 57600
#define DEBUG_ON 1
#define DEBUG_OFF 0

#define BUF_SIZE 1024

void RadioTask();
extern CircularBuffer<char, BUF_SIZE> RadioTxBuf;
extern CircularBuffer<char, BUF_SIZE> RadioRxBuf;
void radioTransceiveTask();

class LoraRadio{
private:
    UARTSerial *_serial;
    ATCmdParser *_parser;
    DigitalOut *_resetPin;
    int fw[3];
    int day, year, hour, min, sec;
    char month[4];
    int u_vdd[4]; // RN2483 VDD in [mV]
    bool debug;


public: 
    LoraRadio(PinName PinTX, PinName PinRX, PinName PinNRST, int baud, int debug);
    void printFwVersion();
    void setSleep(int ms);
    int reset();
    int getVDD();
    int sendBytes(char *data, int len);
    int readLine(char **data);
    void sendtest();


private:
    int getFwVersion();
};

#endif
