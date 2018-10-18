#include <stdio.h>
#include "terminal.h"

#include "mbed.h"
#include "BSP/BSP.h"
#include "LEDdriver.h"
#include "BatteryManager.h"
#include "RFM98W.h"

extern Serial pc;
extern RFM98W radio;
extern BatteryManager bat;
extern LEDdriver L1;
extern LEDdriver L2;
extern Thread LEDdriverThread;

/**
* Example Commands
*/

error_t _printstatus(int argc, arg_t* argv);
error_t _argtest(int argc, arg_t* argv);
error_t _systemstatus(int argc, arg_t* argv);
error_t _led1on(int argc, arg_t* argv);
error_t _led1off(int argc, arg_t* argv);
error_t _led2on(int argc, arg_t* argv);
error_t _led2off(int argc, arg_t* argv);
error_t _ledshowon(int argc, arg_t* argv);
error_t _ledshowoff(int argc, arg_t* argv);
error_t _send(int argc, arg_t* argv);
error_t _getChargeCurrent(int argc, arg_t* argv);
error_t _setChargeCurrent(int argc, arg_t* argv);
error_t _meassyson(int argc, arg_t* argv);
error_t _meassysoff(int argc, arg_t* argv);
error_t _samplemeas(int argc, arg_t* argv);
error_t _setSOC(int argc, arg_t* argv);
error_t _getSOC(int argc, arg_t* argv);
error_t _setHeaterOn(int argc, arg_t* argv);
error_t _setHeaterOff(int argc, arg_t* argv);


termcmd_t cmd_printStatus{
	"printstatus",
	"printstatus",
	"<nothing>",
	_printstatus
};

termcmd_t cmd_argtest{
	"argtest",
	"argtest [-info] -[ver]",
	"print command and arguments",
	_argtest
};


termcmd_t cmd_systemstatus{
	"status",
	"status",
	"prints voltage, current values and charger status flags ",
	_systemstatus
};

termcmd_t cmd_led1on{
	"led1on",
	"led1on [float:current] [float:pwm]",
	"Turn LED 1 on ",
	_led1on
};

termcmd_t cmd_led1off{
	"led1off",
	"led1off",
	" Turn LED 1 off ",
	_led1off
};

termcmd_t cmd_led2on{
	"led2on",
	"led2on [float:current] [float:pwm]",
	"Turn LED 2 on ",
	_led2on
};

termcmd_t cmd_led2off{
	"led2off",
	"led2off",
	" Turn LED 2 off ",
	_led2off
};

termcmd_t cmd_ledshowon{
	"ledshowon",
	"ledshowon",
	"Turn on LED Show",
	_ledshowon
};

termcmd_t cmd_ledshowoff{
	"ledshowoff",
	"ledshowoff",
	"Turn off LED Show",
	_ledshowoff
};

termcmd_t cmd_send{
	"send",
	"send [string:message]",
	"Send message over radio",
	_send
};

termcmd_t cmd_setChargeCurrent{
	"setIcharge",
	"setIcharge [float:current]",
	"Set battery charge current in ampere.",
	_setChargeCurrent
};


termcmd_t cmd_getChargeCurrent{
	"getIcharge",
	"getIcharge",
	"Returns programmed charge current in ampere.",
	_getChargeCurrent
};

termcmd_t cmd_meassyson{
	"meassyson",
	"meassyson",
	"Forces LTC4015 measurement system on.",
	_meassyson
};

termcmd_t cmd_meassysoff{
	"meassysoff",
	"meassysoff",
	"Forces LTC4015 measurement system off in battery only mode.",
	_meassysoff
};

termcmd_t cmd_sampmeas{
	"sampmeas",
	"sampmeas",
	"Samples LTC4015 measurement system.",
	_samplemeas
};

termcmd_t cmd_setsoc{
	"setSOC",
	"setSOC [float:SOC]",
	"Sets normalized battery state of charge.",
	_setSOC
};

termcmd_t cmd_getsoc{
	"getSOC",
	"setSOC",
	"Returns normalized battery state of charge.",
	_getSOC
};

termcmd_t cmd_heateron{
	"heateron",
	"heateron",
	"Enables battery heater.",
	_setHeaterOn
};

termcmd_t cmd_heateroff{
	"heateroff",
	"heateroff",
	"Disables battery heater.",
	_setHeaterOff
};


/**
* @brief prints name of commands and usage
*/
error_t _printstatus(int argc, arg_t* argv) {
	printf("Print Status\n");
	return E_SUCCESS;
}

error_t _argtest(int argc, arg_t* argv) {
	printf("cmd: argtest:\n\targc: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("\t%s\n", argv[i].arg);
	}
	return E_SUCCESS;
}

/**
* @brief prints name of commands and usage
*/
error_t _systemstatus(int argc, arg_t* argv) {
	
	PrintSystemInformation();
    printf("System Status:\n");
    printf("Tbat:\t%4.1f C\nUbat:\t%4.2f V\nIbat:\t%4.3f A\nUin:\t%4.2f V\nUsys:\t%4.2f V\nIin:\t%4.3f A\nTdie:\t%4.1f C\n\r",\
        bat.getBatTemp(), \
        bat.getUBat(), \
        bat.getIBat(),
        bat.getUin(),\
        bat.getUsys(),\
        bat.getIin(),\
        bat.getTdie() );

    printf("Battery Charger Status:\n");
	bat.printStatus();
	return E_SUCCESS;
}

error_t _led1on(int argc, arg_t* argv){
	if(argc == 3){
		float current = atof(argv[1].arg);
		float pwm = atof(argv[2].arg);
		L1.setILed(current);
		L1.setPWM(pwm);
		L1.on();
	}
	else if(argc == 2){
		float current = atof(argv[1].arg);
		L1.setILed(current);
		L1.setPWM(0.5);
		L1.on();
	}
	else if(argc == 1){
		L1.setILed(0.1);
		L1.setPWM(0.5);
		L1.on();
	}
	else{
		printf("command syntax wrong!");
	}
    return E_SUCCESS;
}

error_t _led1off(int argc, arg_t* argv){
    L1.off();
    return E_SUCCESS;
}

error_t _led2on(int argc, arg_t* argv){
	if(argc == 3){
		float current = atof(argv[1].arg);
		float pwm = atof(argv[2].arg);
		L2.setILed(current);
		L2.setPWM(pwm);
		L2.on();
	}
	else if(argc == 2){
		float current = atof(argv[1].arg);
		L2.setILed(current);
		L2.setPWM(0.5);
		L2.on();
	}
	else if(argc == 1){
		L2.setILed(0.1);
		L2.setPWM(0.5);
		L2.on();
	}
	else{
		printf("command syntax wrong!");
	}
    return E_SUCCESS;
}

error_t _led2off(int argc, arg_t* argv){
    L2.off();
    return E_SUCCESS;
}

error_t _ledshowon(int argc, arg_t* argv){
    LEDdriverThread.start(LEDdriverTask);
    return E_SUCCESS;
}

error_t _ledshowoff(int argc, arg_t* argv){
    LEDdriverThread.terminate();
    return E_SUCCESS;
}

error_t _send(int argc, arg_t* argv){
	if(argc > 1){
		for (int i = 1; i < argc; i++) {
			radio.sendPacket(argv[i].arg,strlen(argv[i].arg));
		}
	}
	else{
		printf("command syntax wrong!");
	}
    return E_SUCCESS;
}

error_t _getChargeCurrent(int argc, arg_t* argv){
	if(argc >= 1){
		float current = bat.getIcharge();
		printf("\t %f A\n", current);
	}
	else{
		printf("command syntax wrong!");
	}
	return E_SUCCESS;
}

error_t _setChargeCurrent(int argc, arg_t* argv){
	if(argc >= 2){
		float current = atof(argv[1].arg);
		bat.setIcharge(current);
		printf("Charge current set to %f A.\n",current);
	}
	else{
		printf("command syntax wrong!");
	}
	return E_SUCCESS;
}

error_t _meassyson(int argc, arg_t* argv){
    bat.forceMeasSysOn();
    return E_SUCCESS;
}

error_t _meassysoff(int argc, arg_t* argv){
    bat.forceMeasSysOff();
    return E_SUCCESS;
}

error_t _samplemeas(int argc, arg_t* argv){
    bat.sampleMeasSys();
    return E_SUCCESS;
}

error_t _setSOC(int argc, arg_t* argv){
	if(argc >= 2){
		float soc = atof(argv[1].arg);
		bat.setStateOfCharge(soc);
		printf("State of Charge set to %f\n",soc);
	}
	else{
		printf("command syntax wrong!");
	}
	return E_SUCCESS;
}

error_t _getSOC(int argc, arg_t* argv){
    printf("State of Charge is %f.\n",bat.getStateOfCharge());
    return E_SUCCESS;
}

error_t _setHeaterOn(int argc, arg_t* argv){
	setHeater(true);
	printf("Heater is on\n");
	return E_SUCCESS;
}

error_t _setHeaterOff(int argc, arg_t* argv){
	setHeater(false);
	printf("Heater is off\n");
	return E_SUCCESS;
}




/*** command list ***/
termcmd_t *cmd_list[] = {	&cmd_printStatus, &cmd_argtest, &cmd_systemstatus, &cmd_led1on, \
							&cmd_led1off, &cmd_led2on, &cmd_led2off, &cmd_ledshowon, \
							&cmd_ledshowoff, &cmd_send, &cmd_setChargeCurrent, &cmd_getChargeCurrent, \
							&cmd_meassyson, &cmd_meassysoff, &cmd_sampmeas, &cmd_setsoc, &cmd_getsoc, \
							&cmd_heateroff, &cmd_heateron
						};
const int cmdlist_len = (sizeof(cmd_list) / sizeof(termcmd_t*));

