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
	"systemstatus",
	"systemstatus",
	"prints voltage, current values and charger status flags ",
	_systemstatus
};

termcmd_t cmd_led1on{
	"led1on",
	"led1on",
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
	"led2on",
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
    L1.setILed(0.1);
    L1.on();
    return E_SUCCESS;
}

error_t _led1off(int argc, arg_t* argv){
    L1.off();
    return E_SUCCESS;
}

error_t _led2on(int argc, arg_t* argv){
    L2.setILed(0.1);
    L2.on();
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



/*** command list ***/
termcmd_t *cmd_list[] = { &cmd_printStatus, &cmd_argtest, &cmd_systemstatus, &cmd_led1on, &cmd_led1off, &cmd_led2on, &cmd_led2off, &cmd_ledshowon, &cmd_ledshowoff };
const int cmdlist_len = (sizeof(cmd_list) / sizeof(termcmd_t*));

