/*
 * main.c
 *
 *  Created on: Aug 13, 2023
 *      Author: Minh
 */
#include "Source/App/Main/main.h"

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init!!!");
	ledInit();
	buttonInit(userButton_HoldingEventHandle,userButton_PressAndHoldEventHandle);
	networkInit(userNETWORK_EventHandle);
//	turnOnLed(LED1,ledPink);
	relayInit();
	sigInit();
	switch_config = SWITCH_NONE;
	output_ModeSet(0,output_default);
	output_ModeSet(1,output_default);
	input_ModeSet(0,MomentarySwitch);
	input_ModeSet(1,MomentarySwitch);
//	test_UART();
	systemState = POWER_ON_STATE;
	emberEventControlSetActive(mainStateEventControl);
}

/**
 * @func	mainStateEventHandle
 *
 * @brief	main process
 *
 * @param	[none]
 *
 * @retval	none
 */
void mainStateEventHandle()
{
	EmberNetworkStatus nwkStatusCurrent;

	emberEventControlSetInactive(mainStateEventControl);

	switch(systemState)
	{
	case POWER_ON_STATE:
		nwkStatusCurrent = emberNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			toggleLed(LED1,ledRed,3,200,300);
			NETWORK_FindAndJoin();
		}
		switch_config = SWITCH_NONE;
		systemState = IDLE_STATE;
		break;
	case REPORT_STATE:
		toggleLed(LED1,ledRed,2,400,600);
		networkReady = true;
		systemState = IDLE_STATE;
		SEND_ReportInfoToHc();
		break;
	case IDLE_STATE:
		toggleLed(LED1,ledPink,3,300,300);
		break;
	case SETTING_STATE:
		turnOnLed(LED1,ledPink);
		break;
	case CHOOSE_FEATURES_SETTING:
		toggleLed(LED1,ledBlue,2,300,300);
		break;
	case INPUT_MODE_SETTING_STATE:
		toggleLed(LED1,ledBlue,3,300,300);
		break;
	case OUTPUT_MODE_SETTING_STATE:
		toggleLed(LED1,ledBlue,3,300,300);
		break;
	case BINDING_MODE:
		toggleLed(LED1,ledBlue,4,300,300);
		break;
	case POWER_MODE_SETTING_STATE:
		toggleLed(LED1,ledBlue,5,300,300);
		break;
	case REBOOT_STATE:
		systemState = IDLE_STATE;
		halReboot();
		break;
	default:
		break;
	}
}

/**
 * @func	userNETWORK_EventHandle
 *
 * @brief	process follow network state
 *
 * @param	[networkResult]: result from callback
 *
 * @retval	none
 */
void userNETWORK_EventHandle(uint8_t networkResult)
{
	emberAfCorePrintln("NETWORK_EventHandle!!!");
	switch(networkResult)
	{
	case NETWORK_HAS_PARENT:
		toggleLed(LED1,ledBlue,3,200,300);
		emberAfCorePrintln("Has Parenttt!!!");
		networkReady = true;
		systemState = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_JOIN_FAIL:
		systemState = IDLE_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_JOIN_SUCCESS:
		emberAfCorePrintln("Join Success!!!");
		networkReady = true;
		systemState = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_LOST_PARENT:
		toggleLed(LED1,ledRed,2,400,600);
		systemState = IDLE_STATE;
		emberAfCorePrintln("Losttt!!!");
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_OUT_NETWORK:
		if(networkReady)
		{
			toggleLed(LED1,ledRed,4,200,300);
			systemState = REBOOT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl,3000);
		}
		break;
	default:
		break;
	}
}

/**
 * @func	userButton_PressAndHoldEventHandle
 *
 * @brief	button press and hold handle
 *
 * @param	[button]
 *
 * @param	[pressAndHoldEvent]
 *
 * @retval	none
 */
void userButton_PressAndHoldEventHandle(uint8_t button, uint8_t pressAndHoldEvent)
{
	if (button == SW_1) // for endpoint 1
	{
		switch(pressAndHoldEvent)
		{
		case press_1:
			emberAfCorePrintln("press_1!!!");
			choose_Setting(pressAndHoldEvent);
			break;
		case press_2:
			emberAfCorePrintln("press_2!!!");
			choose_Setting(pressAndHoldEvent);
			break;
		case press_3:// target find
			emberAfCorePrintln("press_3!!!");
			choose_Setting(pressAndHoldEvent);
			break;
		case press_4:// initiator find
			emberAfCorePrintln("press_4!!!");
			choose_Setting(pressAndHoldEvent);
			break;
		case press_5:
			emberAfCorePrintln("press_5!!!");
			choose_Setting(pressAndHoldEvent);
			break;
		case unknown:
			break;
		default:
			break;
		}
	}
}

/**
 * @func	userButton_HoldingEventHandle
 *
 * @brief	button hold handle
 *
 * @param	[button]
 *
 * @param	[holdingEvent]
 *
 * @retval	none
 */
void userButton_HoldingEventHandle(uint8_t button, BUTTON_Event_t holdingEvent)
{
	if (button == SW_1) // for endpoint 1
	{
		switch(holdingEvent)
		{
		case hold_5s:
			emberAfCorePrintln("hold_5s!!!");
			systemState = SETTING_STATE;
			emberEventControlSetActive(mainStateEventControl);
			break;
		case hold_10s:
			emberAfCorePrintln("hold_10s!!!");
			if(emberNetworkState() == EMBER_NO_NETWORK) // đảm bảo thiết bị không vào mạng vẫn có thể reboot bình thường
			{
				toggleLed(LED1,ledRed,4,200,300);
				systemState = REBOOT_STATE;
				emberEventControlSetDelayMS(mainStateEventControl,3000);
			}
				NETWORK_Leave();
			break;
		case unknown:
			break;
		default:
			break;
		}
	}
}

/**
 * @func	choose_Setting
 *
 * @brief	select mode setting from button
 *
 * @param	[button]
 *
 * @retval	none
 */
void choose_Setting(BUTTON_Event_t button)
{
	switch(systemState)
	{
	case SETTING_STATE:
		if(button == press_1)
		{
			switch_config = SWITCH_1;
			toggleLed(LED1,ledPink,1,300,300);
		}
		else if(button == press_2){
			switch_config = SWITCH_2;
			toggleLed(LED1,ledPink,2,300,300);
		}
		emberAfCorePrintln("SWITCH_%d!!!",switch_config+1);
		systemState = CHOOSE_FEATURES_SETTING;
		emberEventControlSetActive(mainStateEventControl);
		break;
	case CHOOSE_FEATURES_SETTING:
		if(button == press_1)
		{
			systemState = OUTPUT_MODE_SETTING_STATE;
			toggleLed(LED1,ledPink,1,300,300);
		}
		else if(button == press_2){
			systemState = INPUT_MODE_SETTING_STATE;
			toggleLed(LED1,ledPink,2,300,300);
		}
		else if(button == press_3){
			// binding using
			systemState = BINDING_MODE;
			toggleLed(LED1,ledPink,3,300,300);
		}
		else if(button == press_4){
			systemState = OUTPUT_SET_WAIT_TIME;
			toggleLed(LED1,ledPink,4,300,300);
		}
		emberAfCorePrintln("systemState = %d!!!",systemState);
		emberEventControlSetActive(mainStateEventControl);
		break;
	case OUTPUT_MODE_SETTING_STATE:
		process_Setting(button);
		break;
	case INPUT_MODE_SETTING_STATE:
		process_Setting(button);
		break;
	case OUTPUT_SET_WAIT_TIME:
		process_Setting(button);
		break;
	case BINDING_MODE:
		process_Setting(button);
		break;
	default:
		break;
	}
}

/**
 * @func	process_Setting
 *
 * @brief	process setting follow mode chosen
 *
 * @param	[button]	: number button press
 *
 * @retval	none
 */
void process_Setting(BUTTON_Event_t number_Button)
{
	switch(systemState)
	{
		case OUTPUT_MODE_SETTING_STATE:
			switch(number_Button)
			{
			case press_1:
				output_ModeSet(switch_config,output_default);
				toggleLed(LED1,ledPink,1,300,300);
				emberAfCorePrintln("output_default!!!");
				break;
			case press_2:
				output_ModeSet(switch_config,autoON);
				toggleLed(LED1,ledPink,2,300,300);
				emberAfCorePrintln("autoON!!!");
				break;
			case press_3:
				output_ModeSet(switch_config,autoOFF);
				toggleLed(LED1,ledPink,3,300,300);
				emberAfCorePrintln("autoOFF!!!");
				break;
			case press_4:
				output_ModeSet(switch_config,delayON);
				toggleLed(LED1,ledPink,4,300,300);
				emberAfCorePrintln("delayON!!!");
				break;
			case press_5:
				output_ModeSet(switch_config,delayOFF);
				toggleLed(LED1,ledPink,5,300,300);
				emberAfCorePrintln("delayOFF!!!");
				break;
			default:
				break;
			}
			break;
		case INPUT_MODE_SETTING_STATE:
			switch(number_Button)
			{
			case press_1:
				input_ModeSet(switch_config,ToggleSwitch);
				toggleLed(LED1,ledPink,1,300,300);
				emberAfCorePrintln("ToggleSwitch!!!");
				break;
			case press_2:
				input_ModeSet(switch_config,AnyChange);
				toggleLed(LED1,ledPink,2,300,300);
				emberAfCorePrintln("AnyChange!!!");
				break;
			case press_3:
				input_ModeSet(switch_config,MomentarySwitch);
				toggleLed(LED1,ledPink,3,300,300);
				emberAfCorePrintln("MomentarySwitch!!!");
				break;
			default:
				break;
			}
		break;
		case OUTPUT_SET_WAIT_TIME:
			switch(number_Button)
			{
			case press_1:
				output_SetTime(switch_config,5000);
				toggleLed(LED1,ledPink,1,300,300);
				emberAfCorePrintln("RELAY %d output_SetTime = %d!!!",switch_config+1,5000);
				break;
			case press_2:
				output_SetTime(switch_config,10000);
				toggleLed(LED1,ledPink,2,300,300);
				emberAfCorePrintln("RELAY %d output_SetTime = %d!!!",switch_config+1,10000);
				break;
			case press_3:
				output_SetTime(switch_config,15000);
				toggleLed(LED1,ledPink,3,300,300);
				emberAfCorePrintln("RELAY %d output_SetTime = %d!!!",switch_config+1,15000);
				break;
			default:
				break;
			}
		break;
		case BINDING_MODE:
			switch(number_Button)
			{
			case press_1://find initiator
				if(switch_config == SWITCH_1)
				{
					(void)emberAfPluginFindAndBindTargetStart(1);
				}
				else if(switch_config == SWITCH_2)
				{
					(void)emberAfPluginFindAndBindTargetStart(2);
				}
				toggleLed(LED1,ledPink,1,200,300);
				break;
			case press_2://find target
				if(switch_config == SWITCH_1)
				{
					(void)emberAfPluginFindAndBindInitiatorStart(1);
				}
				else if(switch_config == SWITCH_2)
				{
					(void)emberAfPluginFindAndBindInitiatorStart(2);
				}
				toggleLed(LED1,ledPink,2,200,300);
				break;
			case press_3: // clear binding table
				emberClearBindingTable();
				toggleLed(LED1,ledPink,3,200,300);
				break;
			default:
				break;
			}
		break;
			default:
				break;
	}
	//trở về chế độ IDLE
	systemState = IDLE_STATE;
	switch_config = SWITCH_NONE;
	emberEventControlSetActive(mainStateEventControl);
}
