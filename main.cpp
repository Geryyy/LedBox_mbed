#include "mbed.h"
#include "BSP/BSP.h"
// #include "BSP/lora_radio.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "Watchdog.h"
#include "DeviceStats.h"
#include "libsmp.h"
#include "libfifo.h"
#include <cstddef>
#include "RFM98W.h"
#include "Radio.h"
#include "TerminalParser/terminal.h"
#include "com.h"
#include "logprintf.h"

// void init();
// void BatteryTaskRadio();
// void radioTransceiveTask();
// void sendTestMsg();
// void sendTestHKD();
// void terminalTask();
// void SystemTask();

signed char rxCallback(fifo_t *buffer){
    return 0;
}
/*** TASKS ***/

/****************** MAIN **********************/
signed char smp_frameReady(fifo_t* buffer);
signed char smp_rogueframeReady(fifo_t* buffer);


Serial pc(USBTX, USBRX, 9600);
// Thread LEDdriverThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDdriverThread");
// Thread LEDThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDThread");
Thread WatchdogThread(osPriorityNormal, OS_STACK_SIZE,NULL,"WatchdogThread");
// Thread SystemThread(osPriorityNormal, OS_STACK_SIZE,NULL,"SystemThread");
// Thread radioThread;
// Thread terminalThread;

LowPowerTicker RadioTicker;
LowPowerTicker SystemTicker;

RFM98W radiophy(PB_15, PB_14, PB_13, PB_12, PC_6, PC_7, 0, false);
Radio radio(smp_frameReady,smp_rogueframeReady,&radiophy, Radio::remote, true);

BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA, 1.1, false);
LEDdriver L1(LED1_SHDN, LED1_PWM, ILED1);
LEDdriver L2(LED2_SHDN, LED2_PWM, ILED2);
Com radiocom = Com();

DigitalOut StatusLed1(PC_12,1);
DigitalOut StatusLed2(PC_11,1);
DigitalOut StatusLed3(PC_10,1);

#define DATASIZE 128
uint8_t data[DATASIZE];

signed char smp_rogueframeReady(fifo_t* buffer){
    static int i = 0;

    xprintf("\n-->smp rogue frame callback!! i=%d\n",i);
    i++;
}

signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
{
    int32_t len = fifo_datasize(buffer);
    bool read = false;
    int j = 0;

    for(int i = 0; i<len; i++){
        uint8_t ch;
        fifo_read_byte(&ch,buffer);

        if(ch == 129){
            read = true;
        }
        if(read){
            data[j] = ch;
            j++;
        }
    }
    static int i = 0;
    xprintf("\n-->smp frame received!! i=%d\n",i);
    i++;
    // xprintf("\n");
    StatusLed1 = !StatusLed1;
    radiocom.updateLaserSettings(data,j);
    return len;
}

void sendHKD(){
    static uint8_t hkd[128];
    int len = radiocom.sendHKD(hkd,128);
    if(len>0){
        radio.sendPacket((char*)hkd,len);
    }
    else{
    }
}

float radioTZyklus = 0.5;
float systemTZyklus = 6.0;

void radioTask(){
    StatusLed2 = !StatusLed2;
    // if(radio.hasreceived()){
    //     char msg[256];
    //     int len = radio.readPacket(msg,255);
    //     msg[len] = '\0';
    //     xprintf("rx msg: %s\n",msg);
    // }
    radio.run(radioTZyklus);
}

void SystemTask(){
    const float TZyklus = systemTZyklus;
    sendHKD(); 
    bat.controller(TZyklus); // battery manager
    StatusLed3 = !StatusLed3;
}

void init(){
    xprintf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    xprintf("System Clock: %ld\n", SystemCoreClock);
    // procResetCounter();
    // printDeviceStats();
}


int main()
{   
    Thread radioThread;
    Thread systemThread;
    EventQueue radioevents;
    EventQueue systemevents;
    radioThread.start(callback(&radioevents, &EventQueue::dispatch_forever));
    systemThread.start(callback(&systemevents, &EventQueue::dispatch_forever));

    init();

    RadioTicker.attach(radioevents.event(&radioTask),radioTZyklus);
    SystemTicker.attach(systemevents.event(&SystemTask),systemTZyklus);
    
    while(true) {
        wait(0.01);
        printOnTerminal(); // display log output (xprint)
        // char* msg = "Hello World";
        // radio.sendPacket(msg,strlen(msg));

    }
}

#define TERMINAL_STR_LEN 24

void terminalTask(){
    char *cmdstring;
	xprintf("TerminalParser 0.1\n\n");

    while(true){

        xprintf("\nenter command: \n");
		cmdstring = getline();

		if (strcmp(cmdstring, "exit\n") == 0)
			break;
		procTerminalString(cmdstring, cmd_list);
        free(cmdstring);
        wait(0.1);
    }
}


