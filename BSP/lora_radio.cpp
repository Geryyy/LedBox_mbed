#include "mbed.h"
#include "BSP/BSP.h"
#include "BSP/lora_radio.h"


LoraRadio::LoraRadio(PinName PinTX, PinName PinRX, int baud = LORA_BAUD, int debug = 0){
    _serial = new UARTSerial(PinTX,PinRX,baud);
    _parser = new ATCmdParser(_serial);
    _parser->debug_on( debug );
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
    printf("\tLora Radio Fw Time:       %s %d %d %d:%d:%d\n", month, day, year, hour, min, sec);
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
    if(_parser->recv("%d%d%d%d",&u_vdd[0],&u_vdd[1],&u_vdd[2],&u_vdd[3])) {
        printf("RN2483 VDD: %d%d%d%d",u_vdd[0],u_vdd[1],u_vdd[2],u_vdd[3]);
        return SUCCESS;
    } else { 
        return ERROR;
    }
}



/*** Task ***/
void RadioTask(){
    printf("\n RN2483 AT CmdParse\n");

    LoraRadio radio = LoraRadio(RADIO_TX, RADIO_RX, LORA_BAUD, 1);
    radio.printFwVersion();
    radio.getVDD();
    radio.reset();

    while(1){
        wait(1.0);
    } 
}

