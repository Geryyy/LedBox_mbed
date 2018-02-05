#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"
#include <stdio.h>
#include <stdlib.h>


LoraRadio::LoraRadio(PinName PinTX, PinName PinRX, int baud = LORA_BAUD, int debug = 0){
    _serial = new UARTSerial(PinTX,PinRX,baud);
    _parser = new ATCmdParser(_serial);
    //_parser->debug_on( debug );
    _parser->set_delimiter( "\r\n" );
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

int LoraRadio::reset(){
    _parser->flush();
    _parser->send("sys reset");
    if(_parser->recv("RN2483 %d.%d.%d %s %d %d %d:%d:%d", &fw[0], &fw[1], &fw[2], month, &day, &year, &hour, &min, &sec)) {
        return SUCCESS;
    } else { 
        return ERROR;
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


int LoraRadio::send(char *data, int len){
    char rxdat[32] = {0};
    char txdat[10+2*64] = {0}; // "radio tx <payload 64 bytes>\0";
    if(len >= 64){
        fprintf(stderr, "%s %d: <len> too long\n", __FILE__, __LINE__);
        return ERROR; // FSK Modulation maximal 64 Bytes
    }
    sprintf(txdat, "radio tx ");
    // copy payload
    for(int i = 0; i<(len);i+=2){
        sprintf(&txdat[i+9], "%.2x", data[i]);
    }
    _parser->flush();
    //_parser->send("mac pause");
    // _parser->flush();
    _parser->send("radio tx deadbeef");

    putc(_parser->getc(),stdout);
    putc(_parser->getc(),stdout);
    putc('\n',stdout);
    return SUCCESS;
    // _parser->recv("%s",rxdat);
    // if(strcmp(rxdat, "ok") == 0){
    //     fprintf(stdout, "%s %d: ok\n", __FILE__, __LINE__);
    //     return SUCCESS;
    // }
    // else if(strcmp(rxdat, "invalid_param") == 0){
    //     fprintf(stderr, "%s %d: invalid param\n", __FILE__, __LINE__);
    //     return ERROR;
    // }
    // else if(strcmp(rxdat, "busy") == 0){
    //     fprintf(stderr, "%s %d: busy\n", __FILE__, __LINE__);
    //     return ERROR;
    // }
    // else{
    //     fprintf(stderr, "%s %d: unknown error\n", __FILE__, __LINE__);
    //     return ERROR;
    // }
}



/*** Task ***/
void RadioTask(){
    printf("\n RN2483 AT CmdParse\n");

    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, LORA_BAUD, 1);
    wait_ms(500);
    radio.printFwVersion();
    radio.getVDD();
    radio.reset();

    char *msg = (char*)"Hello World!"; 

    while(1){
        wait(1.0);
        radio.send(msg,strlen(msg));
    } 
}

