/*
 * main.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

#include <Source/App/Network/network.h>
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "protocol/zigbee/app/framework/util/af-main.h"
#include "hal/hal.h"
#include "Source/Mid/led/led.h"
#include "Source/Mid/button/button.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Receive/received.h"
#include "Source/Hardd/RL_SIG/rl_sig.h"
//#include "find-and-bind-initiator.c"

typedef enum
{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	SETTING_STATE,
	CHOOSE_FEATURES_SETTING,
	INPUT_MODE_SETTING_STATE,
	OUTPUT_MODE_SETTING_STATE,
	OUTPUT_SET_WAIT_TIME,
	BINDING_MODE,
	POWER_MODE_SETTING_STATE,
	REBOOT_STATE
}sysState;

typedef enum{
	SWITCH_1,
	SWITCH_2,
	SWITCH_NONE
}switch_config_e;

EmberEventControl mainStateEventControl;
sysState systemState ;
bool networkReady;
switch_config_e switch_config;

void mainStateEventHandle();
void userButton_PressAndHoldEventHandle(uint8_t button, uint8_t pressAndHoldEvent);
void userButton_HoldingEventHandle(uint8_t button, BUTTON_Event_t holdingEvent);

void process_Setting(BUTTON_Event_t button);
void choose_Setting(BUTTON_Event_t button);

#endif /* SOURCE_APP_MAIN_MAIN_H_ */
