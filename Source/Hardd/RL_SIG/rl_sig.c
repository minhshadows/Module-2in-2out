/*
 * rl_sig.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Minh
 */
#include "rl_sig.h"

RELAYx_t relayArr[RL_COUNT] = RL_INIT;
SIGNALx_t sigArr[SIG_COUNT] = SIG_INIT;

EmberEventControl relay1ToggleEventControl,relay2ToggleEventControl,signalEventControl,inputScanEventControl,InputHoldEventControl;
EmberEventControl *relayEventControl[RL_COUNT];

/**
 * @func	relayInit
 *
 * @brief	Initialize relay
 *
 * @param	[none]
 *
 * @retval	none
 */
void relayInit()
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	for(uint8_t i = 0; i <RL_COUNT; i++)
	{
		GPIO_PinModeSet(relayArr[i].port,relayArr[i].pin,gpioModePushPull,0);
	}
	turnOffRelay(RELAY1);
	turnOffRelay(RELAY2);
	relayEventControl[RELAY1] =(EmberEventControl *) &relay1ToggleEventControl;
	relayEventControl[RELAY2] =(EmberEventControl *) &relay2ToggleEventControl;
}

/**
 * @func	turnOffRelay
 *
 * @brief	Turn off relay
 *
 * @param	[index]
 *
 * @retval	none
 */
void turnOffRelay(uint8_t index)
{
	GPIO_PinOutClear(relayArr[index].port, relayArr[index].pin);
	emberAfCorePrintln("Relay %d OFF",index+1);
}

/**
 * @func	turnOnRelay
 *
 * @brief	turn on Relay
 *
 * @param	[index]
 *
 * @retval	none
 */
void turnOnRelay(uint8_t index)
{
	GPIO_PinOutSet(relayArr[index].port, relayArr[index].pin);
	emberAfCorePrintln("Relay %d ON",index+1);
}

/**
 * @func	sigInit
 *
 * @brief	Initialize signal input
 *
 * @param	[none]
 *
 * @retval	none
 */
void sigInit()
{
	CMU_ClockEnable(cmuClock_GPIO, true);

	for(uint8_t i = 0; i <SIG_COUNT; i++)
	{
		GPIO_PinModeSet(sigArr[i].port,sigArr[i].pin,gpioModeInputPull,0);
	}
	emberEventControlSetDelayMS(inputScanEventControl,20);
}

/**
 * @func	input_ModeCheck
 *
 * @brief	check input mode
 *
 * @param	[signal_id] : input channel
 *
 * @retval	none
 */
void input_ModeCheck(uint8_t signal_id)
{
	switch(sigArr[signal_id].mode)
	{
	case ToggleSwitch:
		if(sigArr[signal_id].state == ON)
		{
			relayArr[signal_id].state = ON;
		}
		else{
			relayArr[signal_id].state = OFF;
		}
		switch_StatusChange(signal_id);
		break;
	case AnyChange:
		relayArr[signal_id].state = !relayArr[signal_id].state;
		switch_StatusChange(signal_id);
		break;
	case MomentarySwitch:
		if(sigArr[signal_id].state == ON)
		{
			sigArr[signal_id].press = true;
			emberEventControlSetActive(InputHoldEventControl);
		}
		else{
			if(sigArr[signal_id].holdTrigger == true)
			{
				relayArr[signal_id].state = !relayArr[signal_id].state;
				emberAfCorePrintln("light ON!!!");
			}
			sigArr[signal_id].holdCount=0;
			sigArr[signal_id].holdTrigger = false;
			sigArr[signal_id].press = false;
		}
		switch_StatusChange(signal_id);
		break;
	default:
		break;
	}
}

/**
 * @func	InputHoldEventHandle
 *
 * @brief	Input momentary scan handle
 *
 * @param	none
 *
 * @retval	none
 */
void InputHoldEventHandle()
{
	emberEventControlSetInactive(InputHoldEventControl);
	bool holdTrigger = false;
	for(uint8_t i =0;i< SIG_COUNT; i++)
	{
		if(sigArr[i].press == true)
		{
			holdTrigger = true;
			sigArr[i].holdCount++;
			if(sigArr[i].holdCount >=5)
			{
				sigArr[i].holdTrigger = true;
			}
		}
	}
	if(holdTrigger == true)
	{
		emberEventControlSetDelayMS(InputHoldEventControl,200);
	}

}

/**
 * @func	switch_StatusChange
 *
 * @brief	process relay_x follow mode set
 *
 * @param	[relayID] : Relay channel
 *
 * @retval	none
 */
void switch_StatusChange(uint8_t relayID)// output
{
	// khi có ngắt sẽ nhảy vào hàm này và xử lý output
	switch(relayArr[relayID].mode)
	{
	case output_default:
		emberEventControlSetActive(*relayEventControl[relayID]);
		break;
	case autoON:
		if(relayArr[relayID].state == OFF)
		{
			emberAfCorePrintln("-------------------------------------------");
			turnOffRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_OFF);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(relayID));
			emberEventControlSetDelayMS(*relayEventControl[relayID],relayArr[relayID].waitTime);
			emberAfCorePrintln("Relay %d autoON",relayID+1);
			emberAfCorePrintln("-------------------------------------------");
		}
		else{
			turnOnRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_ON);
		}
		break;
	case autoOFF:
		if(relayArr[relayID].state == ON)
		{
			emberAfCorePrintln("-------------------------------------------");
			turnOnRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_ON);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(relayID));
			emberEventControlSetDelayMS(*relayEventControl[relayID],relayArr[relayID].waitTime);
			emberAfCorePrintln("Relay %d autoOFF",relayID+1);
			emberAfCorePrintln("-------------------------------------------");
		}
		else{
			turnOffRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_OFF);
		}
		break;
	case delayON:
		if(relayArr[relayID].state == ON)
		{
			emberAfCorePrintln("-------------------------------------------");
			emberAfCorePrintln("delay state = %d ",get_Relaystate(relayID));
			emberEventControlSetDelayMS(*relayEventControl[relayID],relayArr[relayID].waitTime);
			emberAfCorePrintln("Relay %d delayON",relayID+1);
			emberAfCorePrintln("-------------------------------------------");
		}
		else{
			turnOffRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_OFF);
		}
		break;
	case delayOFF:
		if(relayArr[relayID].state == OFF)
		{
			emberAfCorePrintln("-------------------------------------------");
			emberAfCorePrintln("delay state = %d ",get_Relaystate(relayID));
			emberEventControlSetDelayMS(*relayEventControl[relayID],relayArr[relayID].waitTime);
			emberAfCorePrintln("Relay %d delayOFF",relayID+1);
			emberAfCorePrintln("-------------------------------------------");
		}
		else{
			turnOnRelay(relayID);
			SEND_OnOffStateReport(relayID+1,LED_ON);
		}
		break;
	default:
		break;
	}
}

/**
 * @func	output_ModeSet
 *
 * @brief	set mode for relay_x
 *
 * @param	[id] : Relay channel
 *
 * @param	[mode] : output mode
 *
 * @retval	none
 */
void output_ModeSet(relayNumber id,OUTPUT_MODE_e mode)
{
	relayArr[id].mode = mode;
}

/**
 * @func	input_ModeSet
 *
 * @brief	set mode for input_x
 *
 * @param	[id] : input channel
 *
 * @param	[mode] : input mode
 *
 * @retval	none
 */
void input_ModeSet(inputNumber id,INPUT_MODE_e mode)
{
	sigArr[id].mode = mode;
}

/**
 * @func	relay1ToggleEventHandle
 *
 * @brief	handle process Relay_1 follow mode chosen
 *
 * @param	none
 *
 * @retval	none
 */
void relay1ToggleEventHandle()
{
	emberEventControlSetInactive(relay1ToggleEventControl);
	switch(relayArr[RELAY1].mode)
	{
	case output_default:
		if(relayArr[RELAY1].state == ON)
		{
			turnOnRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_ON);
		}
		else
		{
			turnOffRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_OFF);
		}
		break;
	case autoON:
		if(get_Relaystate(RELAY1) == OFF)
		{
//			relayArr[RELAY1].state = ON;
			turnOnRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_ON);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(RELAY1));
		}
		break;
	case autoOFF:
		if(get_Relaystate(RELAY1) == ON)
		{
//			relayArr[RELAY1].state = OFF;
			turnOffRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_OFF);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(RELAY1));
		}
		break;
	case delayON:
			turnOnRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_ON);
			emberAfCorePrintln("delay state = %d ",get_Relaystate(RELAY1));
		break;
	case delayOFF:
			turnOffRelay(RELAY1);
			SEND_OnOffStateReport(1,LED_OFF);
			emberAfCorePrintln("delay state = %d ",get_Relaystate(RELAY1));
		break;
	default:
		break;
	}
}

/**
 * @func	relay2ToggleEventHandle
 *
 * @brief	handle process Relay_2 follow mode chosen
 *
 * @param	none
 *
 * @retval	none
 */
void relay2ToggleEventHandle()
{
	emberEventControlSetInactive(relay2ToggleEventControl);
	switch(relayArr[RELAY2].mode)
	{
	case output_default:
		if(relayArr[RELAY2].state == ON)
		{
			turnOnRelay(RELAY2);
			SEND_OnOffStateReport(RELAY2+1,LED_ON);
		}
		else
		{
			turnOffRelay(RELAY2);
			SEND_OnOffStateReport(RELAY2+1,LED_OFF);
		}
		break;
	case autoON:
		if(get_Relaystate(RELAY2) == OFF)
		{
			relayArr[RELAY2].state = ON;
			turnOnRelay(RELAY2);
			SEND_OnOffStateReport(2,LED_ON);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(RELAY2));
		}
		break;
	case autoOFF:
		if(get_Relaystate(RELAY2) == ON)
		{
			relayArr[RELAY2].state = OFF;
			turnOffRelay(RELAY2);
			SEND_OnOffStateReport(2,LED_OFF);
			emberAfCorePrintln("Relay state = %d ",get_Relaystate(RELAY2));
		}
		break;
	case delayON:
			turnOnRelay(RELAY2);
			SEND_OnOffStateReport(2,LED_ON);
			emberAfCorePrintln("delay state = %d ",get_Relaystate(RELAY2));
		break;
	case delayOFF:
			turnOffRelay(RELAY2);
			SEND_OnOffStateReport(2,LED_OFF);
			emberAfCorePrintln("delay state = %d ",get_Relaystate(RELAY2));
		break;
	default:
		break;
	}
}

/**
 * @func	inputScanEventHandle
 *
 * @brief	scan signal input every 20ms
 *
 * @param	none
 *
 * @retval	none
 */
void inputScanEventHandle()
{
	emberEventControlSetInactive(inputScanEventControl);
	uint8_t debounce,inputStateNow,inputStatePrev;

	for(uint8_t i=0; i<SIG_COUNT; i++)
	{
		if((GPIO_PinInGet(sigArr[i].port,sigArr[i].pin))!= sigArr[i].state)
		{
			inputStateNow = GPIO_PinInGet(sigArr[i].port,sigArr[i].pin);
			//xử lý rung phím bằng phương pháp lấy mẫu
			for(debounce = 0;debounce < MAX_DEBOUNCE;debounce = (inputStateNow == inputStatePrev) ? debounce + 1 : 0)
			{
				inputStatePrev =inputStateNow;
				inputStateNow = GPIO_PinInGet(sigArr[i].port,sigArr[i].pin);
			}
			sigArr[i].state = inputStateNow;
			input_ModeCheck(i);
		}
	}
	emberEventControlSetDelayMS(inputScanEventControl,20);
}

/**
 * @func	output_SetTime
 *
 * @brief	set wait time for relay_x
 *
 * @param	[id]	: relay_x channel
 *
 * @param	[time]	: time delay
 *
 * @retval	none
 */
void output_SetTime(relayNumber id,uint32_t time)
{
	relayArr[id].waitTime = time;
}

/**
 * @func	set_RelayState
 *
 * @brief	set relay_x state (using for input want to control relay)
 *
 * @param	[id]	: input_x channel
 *
 * @param	[state]	: ON or OFF
 *
 * @retval	none
 */
void set_RelayState(relayNumber id, bool state)
{
	relayArr[id].state = state;
}

/**
 * @func	get_Relaystate
 *
 * @brief	get relay_x state
 *
 * @param	[id]	: relay_x channel
 *
 * @retval	none
 */
uint8_t get_Relaystate(uint8_t relayID)
{
	uint8_t state = GPIO_PinOutGet(relayArr[relayID].port,relayArr[relayID].pin);
	return state;
}

/****************************************************************************************************
 * 																									*
 * 											TEST													*
 * 																									*
 ****************************************************************************************************/
void test_UART(void)
{
	emberEventControlSetDelayMS(signalEventControl,3000);
}

void signalEventHandle()
{
	emberEventControlSetInactive(signalEventControl);

	emberAfCorePrintln("INPUT 1 = %d!!!!!",sigArr[INPUT1].state);
	emberAfCorePrintln("INPUT 2 = %d!!!!!",sigArr[INPUT2].state);
	emberAfCorePrintln("RELAY 1 = %d!!!!!",relayArr[RELAY1].state);
	emberAfCorePrintln("RELAY 2 = %d!!!!!",relayArr[RELAY2].state);
	emberAfCorePrintln("-------------------------------------------");
	emberEventControlSetDelayMS(signalEventControl,3000);
}
