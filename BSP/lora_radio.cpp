/* @file: lora_radio.cpp
 * @author: Gerald Ebmer
 * @date: 31.01.2018
 * @brief: support for lora module rn2483
 */

#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include <stdio.h>
#include <stdlib.h>
#include "libsmp.h"
#include "libfifo.h"
#include <cstddef>


LoraRadio::LoraRadio(PinName PinTX, PinName PinRX, PinName PinNRST, int baud = LORA_BAUD, int debug = DEBUG_OFF, signed char (*rxCallback)(fifo_t* buffer)=NULL){
    _resetPin = new DigitalInOut(PinNRST, PIN_OUTPUT, OpenDrain, 1);
    _serial = new UARTSerial(PinTX,PinRX,baud);
    _parser = new ATCmdParser(_serial);
    _parser->debug_on( debug );
    _parser->set_delimiter( "\r\n" );
    _parser->set_timeout(2000);

    hardreset();
    init();

    // SMP
    smp_frameReady = rxCallback;
    fifo_init(&fifo,buffer,sizeof(buffer));
    smp.buffer = &fifo;
    smp.frameReadyCallback = smp_frameReady;
    smp.rogueFrameCallback = 0;
    SMP_Init(&smp);

    // wait_ms(10);    
}

bool LoraRadio::init(void){
    bool success = _parser->send("radio set mod lora")
    && _parser->recv("ok")
    && _parser->send("radio set freq 868100000")
    && _parser->recv("ok")
    && _parser->send("radio set pwr 14")
    && _parser->recv("ok")
    && _parser->send("radio set sf sf12")
    && _parser->recv("ok")
    && _parser->send("radio set afcbw 125")
    && _parser->recv("ok")
    && _parser->send("radio set rxbw 250")
    && _parser->recv("ok")
    && _parser->send("radio set fdev 5000")
    && _parser->recv("ok")
    && _parser->send("radio set prlen 8")
    && _parser->recv("ok")
    && _parser->send("radio set crc on")
    && _parser->recv("ok")
    && _parser->send("radio set cr 4/8")
    && _parser->recv("ok")
    && _parser->send("radio set wdt 5500")
    && _parser->recv("ok")
    && _parser->send("radio set sync 12")
    && _parser->recv("ok")
    && _parser->send("radio set bw 250")
    && _parser->recv("ok")
    && _parser->send("sys get hweui")
    && _parser->recv("ok")
    && _parser->recv("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",&hweui[15],&hweui[14],&hweui[13],&hweui[12],&hweui[11],&hweui[10],&hweui[9],&hweui[8],&hweui[7],&hweui[6],&hweui[5],&hweui[4],&hweui[3],&hweui[2],&hweui[1],&hweui[0])
    && _parser->send("mac pause");
    return success;
}

bool LoraRadio::softreset(void){
    bool success = _parser->send("sys reset")
    && _parser->recv("RN2483 %d.%d.%d %s %d %d %d:%d:%d", &fw[0], &fw[1], &fw[2], month, &day, &year, &hour, &min, &sec);
    return success;
} 

void LoraRadio::hardreset(void){
    /* Hard reset */
    _resetPin->write(0);
    wait_ms(10);
    _resetPin->write(1);
    wait_ms(200);
} 


int LoraRadio::write(char *data, int len){
    uint32_t txlen = SMP_Send((unsigned char*)data,len,transmitBuffer,sizeof(transmitBuffer), &messageStart);
    while(txlen>0){
        if(txlen >= TX_MAX){
            sendBytes(messageStart,TX_MAX);
            txlen -= TX_MAX;
            messageStart += TX_MAX;
        }
        else{
            // send remaining data
            sendBytes(messageStart,txlen);
            txlen -= txlen; // break;
        }
    }
    return SUCCESS;
}

int LoraRadio::getFwVersion(){
    _parser->flush();
    _parser->send("sys get ver");
    if(_parser->recv("RN2483 %d.%d.%d %s %d %d %d:%d:%d", &fw[0], &fw[1], &fw[2], month, &day, &year, &hour, &min, &sec)) {
        return SUCCESS;
    } else { 
        return ERROR;
    }
}

void LoraRadio::printFwVersion(){
    _parser->flush();
    this->getFwVersion();
    printf("\tLora Radio FW Version: %d.%d.%d\n", fw[0], fw[1], fw[2]);
    printf("\tLora Radio Fw Time:    %s %d %d %d:%d:%d\n", month, day, year, hour, min, sec);
}

void LoraRadio::setSleep(int ms){
    _parser->flush();
    _parser->send("sys sleep %d",ms);
    if(_parser->recv("invalid param")){
        printf("sleep: success\n");
    }
    else{
        printf("sleep: failure\n");
    }
}

int LoraRadio::getVDD(){
    _parser->flush();
    _parser->send("sys get vdd");
    char data[5] = {0};
    if(_parser->read(data, 4) != -1) {
        printf("RN2483 VDD: %s\n",data);
        return SUCCESS;
    } else { 
        printf("error in getVDD()\n");
        return ERROR;
    }
}



int LoraRadio::sendBytes(unsigned char *data, int len){
    char *ret;
    int i = 0;
    char txdat[10+2*255] = {0}; // "radio tx <payload 64 bytes>\0";
    int hexlen = 2*len; // zwei hex zeichen pro byte
    
    if(hexlen >= 255){
        fprintf(stderr, "%s %d: <len> too long\n", __FILE__, __LINE__);
        return ERROR; // FSK Modulation maximal 64 Bytes
    }

    sprintf(txdat, "radio tx ");
    // copy payload
    for(int i = 0; i<(len);i++){
        sprintf(&txdat[2*i+9], "%.2x", data[i]);
    }
    _parser->send(txdat);

    // OK
    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);
    
    // wait for the radio to transmit data
    wait(1);

    // RADIO TX OK
    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);

    return SUCCESS;
}

int LoraRadio::readLine(char **data){
    #define BUFFLEN 64
    static char msg[BUFFLEN];
    int i = 0;
    while(i<(BUFFLEN-1)){
        int ret = _parser->getc();
        if(ret == -1){
            // timeout occured
            break;
        }
        msg[i] = (char) ret;
        
        if(msg[i] == '\n'){
            break;
        }
        i++;
    }
    msg[i+1] = '\0';
    *data = msg;
    return i;
    #undef BUFFLEN
}

void LoraRadio::sendtest(){
    char msg[] = "radio tx deadbeef";
    _parser->flush();
    //_parser->send("mac pause");
    // _parser->flush();
    _parser->send(msg);

    char *ret;
    int i = 0;
    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);
    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);
}



/*** Task ***/
void RadioTask(){
    printf("\n RN2483 AT CmdParse\n");

    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, RADIO_RESET, LORA_BAUD, DEBUG_ON);
    wait_ms(500);

    char *msg = (char*)"Hello World!"; 

    while(1){
        wait(2.0);
        radio.sendBytes((unsigned char*)msg,strlen(msg));
       // radio.sendtest();
    } 
}



/*** RADIO SMP COM ***/
unsigned char buffer[100];

fifo_t fifo;
smp_struct_t smp;

signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
{
    return 0;
}

void RadioSMPTask()
{

    unsigned int frameLength; //Länge des SMP Frames
    unsigned char transmitBuffer[SMP_SEND_BUFFER_LENGTH(1000)]; //Puffer in den das fertige SMP Packet geschrieben wird
    //Das SMP_SEND_BUFFER_LENGTH Makro erzeugt einen Puffer in welchem das SMP Packet auch im worst case platz findet, in vielen Fällen kann der Puffer jedoch verkleinert werden
    //taucht das Preamble in den gesendeten Daten nicht auf, kann als Puffergröße "Läng der größten Nachricht" + 9 gewählt werden
    unsigned char* messageStart; //Pointer auf den Start der Nachricht im transmitBuffer

    fifo_init(&fifo,buffer,sizeof(buffer));

    smp.buffer = &fifo;
    smp.frameReadyCallback = smp_frameReady;
    smp.rogueFrameCallback = 0;

    SMP_Init(&smp);

    const char message[] = "Teststring";

    frameLength = SMP_Send((const byte*)message,sizeof(message),transmitBuffer,sizeof(transmitBuffer), &messageStart);

    SMP_RecieveInBytes(messageStart,frameLength,&smp);
}




