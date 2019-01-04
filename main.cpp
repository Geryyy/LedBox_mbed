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
#include "DS1820.h"

/* for LOG and WARNING */
#define MODULE_NAME "MAIN"

/* DS18b20 Temperature Sensor: Box Temperature */
#define DATA_PIN PA_9
#define VDD_PIN PA_8

/****************** MAIN **********************/
bool _debugSMPCallbacks = false;
bool _debug = true;

signed char smp_frameReady(fifo_t* buffer);
signed char smp_rogueframeReady(fifo_t* buffer);

Serial pc(USBTX, USBRX, 9600);
RFM98W radiophy(PB_15, PB_14, PB_13, PB_12, PC_6, PC_7, 0, false);
Radio radio(smp_frameReady,smp_rogueframeReady,&radiophy, Radio::remote, false);
BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA, 20.0, false);
LEDdriver L1(LED1_SHDN, LED1_PWM, ILED1);
LEDdriver L2(LED2_SHDN, LED2_PWM, ILED2);
Com radiocom = Com(false);
// DS1820 probe(DATA_PIN);
// DigitalOut VDDPin(VDD_PIN);

DigitalOut StatusLed1(PC_12,1);
DigitalOut StatusLed2(PC_11,1);
DigitalOut StatusLed3(PC_10,1);
DigitalIn StatusButton(USER_BUTTON);

float BoxTemperature;

#define DATASIZE 128
uint8_t data[DATASIZE];

signed char smp_rogueframeReady(fifo_t* buffer){
    static int i = 0;

    if(_debugSMPCallbacks)
        LOG("\n-->smp rogue frame callback!! i=%d\n",i);
    i++;
    return 0;
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
    if(_debugSMPCallbacks)
        LOG("\n-->smp frame received!! i=%d\n",i);
    i++;
    /* write settings to LED Driver */
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
    
    if(_debug){
        radiocom.printHKD();
    }
}

float radioTZyklus = 0.5;
float systemTZyklus = 6.0;

void radioTask(){
    // StatusLed2 = !StatusLed2;
    radio.run(radioTZyklus);
}

void SystemTask(){
    const float TZyklus = systemTZyklus;
    sendHKD(); 
    bat.controller(TZyklus); // battery manager
    // StatusLed3 = !StatusLed3;
    // VDDPin = 1;
    // probe.convertTemperature(true, DS1820::all_devices);         //Start temperature conversion, wait until ready
    // BoxTemperature = probe.temperature();
    // VDDPin = 0;
    BoxTemperature = 22.0;//getMCUTemp();
}

void BlinkTask(){
    StatusLed1 = 0; // on
    wait_ms(20);
    StatusLed1 = 1; // off

    // show battery state of charge
    if(StatusButton == 0){
        if(bat.data.stateofcharge > 0.75){
            StatusLed1 = 0;
        }
        if(bat.data.stateofcharge > 0.5){
            StatusLed2 = 0;
        }
        if(bat.data.stateofcharge > 0.25){
            StatusLed3 = 0;
        }
    }
    else{
        StatusLed1 = 1; // off
        StatusLed2 = 1; 
        StatusLed3 = 1;
    }
}

void init(){
    xprintf("LED Box Rev 1.0\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    xprintf("System Clock: %ld\n", SystemCoreClock);
    procResetCounter();
    printDeviceStats();
}

void terminalTask();

int main()
{   
    Thread radioThread(osPriorityNormal, OS_STACK_SIZE,NULL,"RadioThread");
    Thread systemThread(osPriorityNormal, OS_STACK_SIZE,NULL,"SystemThread");
    Thread watchdogThread(osPriorityNormal, OS_STACK_SIZE,NULL,"WatchdogThread");
    Thread statusLEDThread(osPriorityNormal, OS_STACK_SIZE,NULL,"StatusLEDThread");
    /* Thread EventQueue and LowPowerTicker for PowerLED OOK is used in com Module */
    // Thread terminalThread;

    EventQueue radioevents;
    EventQueue systemevents;
    EventQueue statusevents;

    radioThread.start(callback(&radioevents, &EventQueue::dispatch_forever));
    systemThread.start(callback(&systemevents, &EventQueue::dispatch_forever));
    statusLEDThread.start(callback(&statusevents,&EventQueue::dispatch_forever));
    watchdogThread.start(WatchdogTask);

    init();

    LowPowerTicker RadioTicker;
    LowPowerTicker SystemTicker;
    LowPowerTicker StatusLEDTicker;

    RadioTicker.attach(radioevents.event(&radioTask),radioTZyklus);
    SystemTicker.attach(systemevents.event(&SystemTask),systemTZyklus);
    StatusLEDTicker.attach(statusevents.event(&BlinkTask),2.0);

    // terminalThread.start(terminalTask);
    
    while(true) {
        wait(0.1);
        printOnTerminal(); // display xprint output
        sleep();
    }
}

#define TERMINAL_STR_LEN 24

void terminalTask(){
    char *cmdstring;
	xprintf("TerminalParser\n\n");

    while(true){

        xprintf("\nenter command: \n");
		cmdstring = getline();
        xprintf("getline: %s\n",cmdstring);
		// if (strcmp(cmdstring, "exit\n") == 0)
		// 	break;
		procTerminalString(cmdstring, cmd_list);
        free(cmdstring);
        wait(0.1);
    }
}


