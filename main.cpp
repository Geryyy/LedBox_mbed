#include "mbed.h"
// #include "BSP/BSP.h"
// // #include "BSP/lora_radio.h"
// #include "LEDdriver.h"
// #include "BatteryManager.h"
// #include "Watchdog.h"
#include "DeviceStats.h"
// #include "libsmp.h"
// #include "libfifo.h"
// #include <cstddef>
// #include "RFM98W.h"
// #include "TerminalParser/terminal.h"
// #include "com.h"

// void init();
// void BatteryTaskRadio();
// void radioTransceiveTask();
// void sendTestMsg();
// void sendTestHKD();
// void terminalTask();
// void SystemTask();

// signed char rxCallback(fifo_t *buffer){
//     return 0;
// }
// /*** TASKS ***/

// /****************** MAIN **********************/
// signed char smp_frameReady(fifo_t* buffer);


// Serial pc(USBTX, USBRX, 9600);
// // Thread LEDdriverThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDdriverThread");
// // Thread LEDThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDThread");
// // Thread WatchdogThread(osPriorityNormal, OS_STACK_SIZE,NULL,"WatchdogThread");
// // Thread SystemThread(osPriorityNormal, OS_STACK_SIZE,NULL,"SystemThread");
// // Thread radioThread;
// // Thread terminalThread;

// RFM98W radio(PB_15, PB_14, PB_13, PB_12, PC_6, PC_7, 2, smp_frameReady, NULL, false);
// BatteryManager bat = BatteryManager(LTC4015_ADDR, SDA,SCL,SMBA, 1.1, true);
// LEDdriver L1(LED1_SHDN, LED1_PWM, ILED1);
// LEDdriver L2(LED2_SHDN, LED2_PWM, ILED2);
// Com radiocom = Com();

// #define DATASIZE 128
// uint8_t data[DATASIZE];

// signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
// {
//     int32_t len = fifo_datasize(buffer);
//     bool read = false;
//     int j = 0;

//     for(int i = 0; i<len; i++){
//         uint8_t ch;
//         fifo_read_byte(&ch,buffer);

//         if(ch == 129){
//             read = true;
//         }
//         if(read){
//             data[j] = ch;
//             j++;
//         }
//     }
//     // printf("\n");
//     radiocom.updateLaserSettings(data,j);
//     return len;
// }

// void radioTask(){
//     while(true){
//         radio.serviceRadio();
//         wait(0.2);
//     }
// }


void init(){
    printf("LED Box Rev 0.1\nGerald Ebmer (c) 2018\nACIN TU WIEN\n\n");
    printf("System Clock: %ld\n", SystemCoreClock);
    // procResetCounter();
    // printDeviceStats();
}

int main()
{   

    init();
    printf("testoutput\n");
    // WatchdogThread.start(WatchdogTask);
    // LEDThread.start(LEDTask);
    // radioThread.start(radioTask);
    // terminalThread.start(terminalTask);
    // SystemThread.start(SystemTask);
    
    while(true) {
        wait(2);
        // radio.stopreceive();
        // sendTestHKD();
        // bat.forceMeasSysOn();
    }
}






// void sendTestMsg(){
//     static char msg[256];
//     static int i = 0;
//     sprintf(msg,"hello nr: %d\n",i);
//     radio.sendPacket(msg,strlen(msg));
//     printf("Packet send\n");
//     i++;
// }

// void sendHKD(){
//     static uint8_t hkd[128];
//     int len = radiocom.sendHKD(hkd,128);
//     if(len>0){
//         radio.sendPacket((char*)hkd,len);
//         //printf("Packet send\n");
//     }
//     else{
//         //printf("Buffer too small\n");
//     }
// }


// void SystemTask(){
//     float TZyklus = 4.0;

//     while(true){
//         sendHKD(); 
//         bat.controller(TZyklus); // battery manager
//         wait(TZyklus);
//     }
// }










// // #define TERMINAL_STR_LEN 24

// void terminalTask(){
//     char *cmdstring;
// 	printf("TerminalParser 0.1\n\n");

//     while(true){

//         printf("\nenter command: \n");
// 		cmdstring = getline();

// 		if (strcmp(cmdstring, "exit\n") == 0)
// 			break;
// 		procTerminalString(cmdstring, cmd_list);
//         free(cmdstring);
//         wait(0.1);
//     }
// }

