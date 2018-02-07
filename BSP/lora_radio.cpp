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


LoraRadio::LoraRadio(PinName PinTX, PinName PinRX, int baud = LORA_BAUD, int debug = DEBUG_OFF){
    _serial = new UARTSerial(PinTX,PinRX,baud);
    _parser = new ATCmdParser(_serial);
    _parser->debug_on( debug );
    _parser->set_delimiter( "\r\n" );

    /* Hard reset */
    DigitalOut _resetPin(RADIO_RESET,0);
    wait_ms(1);
    _resetPin = 1;
    wait_ms(5);
     
    char *ret;
    /* radio config */
    //_serial->send_break();
    _parser->send("sys reset");
    readLine(&ret);  
    if(debug) printf("%s\n",ret); 

    _parser->send("radio set mod lora");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set freq 868100000");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set pwr 14");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set sf sf12");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set afcbw 125");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set rxbw 250");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set fdev 5000");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set prlen 8");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set crc on");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set cr 4/8");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set wdt 5500");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set sync 12");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("radio set bw 250");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("sys get hweui");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    _parser->send("mac pause");
    readLine(&ret);
    if(debug) printf("%s\n",ret);

    wait_ms(10);
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


int LoraRadio::sendBytes(char *data, int len){
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

    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);
    i = readLine(&ret);
    printf("lora return len: %d, msg: %s\n",i, ret);

    return SUCCESS;
}

int LoraRadio::readLine(char **data){
    #define BUFFLEN 64
    static char msg[BUFFLEN];
    int i = 0;
    while(i<(BUFFLEN-1)){
        msg[i] = _parser->getc();
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

    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, LORA_BAUD, 1);
    wait_ms(500);

    char *msg = (char*)"Hello World!"; 

    while(1){
        wait(2.0);
        radio.sendBytes(msg,strlen(msg));
       // radio.sendtest();
    } 
}

