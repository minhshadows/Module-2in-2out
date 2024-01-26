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
//	networkInit(userNETWORK_EventHandle);
	systemState = POWER_ON_STATE;
//	emberEventControlSetActive(mainStateEventControl);
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
			toggleLed(LED1,ledBlue,3,200,200);
			NETWORK_FindAndJoin();
		}
		systemState = IDLE_STATE;
		break;
	case REPORT_STATE:
		toggleLed(LED1,ledRed,3,300,300);
		networkReady = true;
		systemState = IDLE_STATE;
		SEND_ReportInfoToHc();
		break;
	case IDLE_STATE:
		break;
	case REBOOT_STATE:
		systemState = IDLE_STATE;
		halReboot();
		break;
	default:
		break;
	}
}
