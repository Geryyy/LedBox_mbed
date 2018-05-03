/* @file: lora_radio.h
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: header file for support module for lora module rn2483
 */

#include "libsmp.h"
#include "libfifo.h"

#ifndef LORA_RADIO_H
#define LORA_RADIO_H

#define LORA_BAUD 57600
#define DEBUG_ON 1
#define DEBUG_OFF 0

#define TX_MAX (127) /* 127 bytes -> 254 hex values < max 255 */



void RadioTask();
// extern CircularBuffer<char, BUF_SIZE> RadioTxBuf;
// extern CircularBuffer<char, BUF_SIZE> RadioRxBuf;
// void radioTransceiveTask();

class LoraRadio{
private:
    UARTSerial *_serial;
    ATCmdParser *_parser;
    DigitalInOut *_resetPin;
    int fw[3];
    int day, year, hour, min, sec;
    char month[4];
    char hweui[16];
    int u_vdd[4]; // RN2483 VDD in [mV]
    bool debug;
    // SMP
    unsigned char buffer[100];
    fifo_t fifo;
    smp_struct_t smp;
    unsigned int frameLength; //Länge des SMP Frames
    unsigned char transmitBuffer[SMP_SEND_BUFFER_LENGTH(1000)]; 
    unsigned char* messageStart; //Pointer auf den Start der Nachricht im transmitBuffer
    signed char (*smp_frameReady)(fifo_t* buffer); //Frame wurde empfangen


public: 
    LoraRadio(PinName PinTX, PinName PinRX, PinName PinNRST, int baud, int debug, signed char (*rxCallback)(fifo_t* buffer));
    
    int write(char *data, int len);
    void printFwVersion();
    void setSleep(int ms);
  
    int getVDD();
    int sendBytes(unsigned char *data, int len);
    int readLine(char **data);
    void sendtest();


private:
    int getFwVersion();
    bool init(void);
    bool softreset(void);
    void hardreset(void);
};

#endif
