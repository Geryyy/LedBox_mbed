/* @file: BSP.h
 * @author: Gerald Ebmer
 * @date: 24.01.2018
 * @brief: board support package for LED Box Driver Board
 */

#ifndef BSP_H
#define BSP_H

/* Pin Definition */
#define User_LED PC_12
#define Uer_Button PC_13

// LED Driver 
#define LED1_PWM PB_10
#define LED1_SHDN PB_1
#define ILED1 PA_4

#define LED2_PWM PB_11
#define LED2_SHDN PB_2
#define ILED2 PA_5

// virtual com port (USB COM)
#define VCP_TX PA_2
#define VCP_RX PA_3

// Lora Modul
#define RADIO_TX PA_9
#define RADIO_RX PA_10
#define RADIO_CTS PA_11
#define RADIO_RTS PA_12

// Battery Management
#define SMBA PB_5
#define SCL PB_6
#define SDA PB_7

#define EQ PB_8
#define MPPT PB_9

// Battery Heater
#define HEATER PA_6


/* Konstanten */


// user led
void LEDTask();
void PrintSystemInformation();

void RadioTask();
void loopTask();
void radioTask3();

float getUrefCal();
float getSysVoltage();
float getMCUTemp();

#endif // BSP_H