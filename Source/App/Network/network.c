/*
 * network.c
 *
 *  Created on: Aug 24, 2023
 *      Author: Minh
 */


#include <Source/App/Network/network.h>
#include "Source/Mid/led/led.h"

EmberEventControl joinNetworkEventControl;

networkHandle_t NETWORK_EventHandle = NULL;

uint8_t timeFindAndJoin;
bool networkReady;

void networkInit(networkHandle_t userNetworkHandle)
{
	NETWORK_EventHandle = userNetworkHandle;
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
boolean emberAfStackStatusCallback(EmberStatus status)
{
	emberAfCorePrintln("Stack Status!!!");
	if(status == EMBER_NETWORK_UP)
	{
		if(timeFindAndJoin > 0)
		{
			NETWORK_StopFinAndJoin();
			if(NETWORK_EventHandle != NULL)
			{
				NETWORK_EventHandle(NETWORK_JOIN_SUCCESS);
			}
		}else
		{
			if(NETWORK_EventHandle != NULL)
			{
				NETWORK_EventHandle(NETWORK_HAS_PARENT);
			}
		}
	}
	else
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(NETWORK_EventHandle != NULL)
			{
				NETWORK_EventHandle(NETWORK_OUT_NETWORK);
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT)
		{
			NETWORK_EventHandle(NETWORK_LOST_PARENT);
		}
	}
  return false;
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
		toggleLed(LED2,ledBlue,3,300,300);
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
		toggleLed(LED2,ledRed,3,300,300);
		systemState = IDLE_STATE;
		emberAfCorePrintln("Losttt!!!");
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_OUT_NETWORK:
		if(networkReady)
		{
			toggleLed(LED1,ledPink,3,300,300);
			systemState = REBOOT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl,3000);
		}
		break;
	default:
		break;
	}
}

/**
 * @func	NETWORK_FindAndJoin
 *
 * @brief	use to find network after 2000 ms
 *
 * @param	[none]
 *
 * @retval	none
 */
void NETWORK_FindAndJoin()
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl,2000);
	}
}

/**
 * @func	joinNetworkEventHandle
 *
 * @brief	Handle join network
 *
 * @param	[none]
 *
 * @retval	none
 */
void joinNetworkEventHandle()
{
	emberEventControlSetInactive(joinNetworkEventControl);
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();
		timeFindAndJoin++;
	}
}

/**
 * @func	NETWORK_StopFinAndJoin
 *
 * @brief	Stop find and join network
 *
 * @param	[none]
 *
 * @param	[none]
 *
 * @retval	none
 */
void NETWORK_StopFinAndJoin()
{
	if(emberAfNetworkState() == EMBER_JOINED_NETWORK)
	emberAfPluginNetworkSteeringStop();
}

/**
 * @func	NETWORK_Leave()
 *
 * @brief	Clear Binding-table and leave network
 *
 * @param	[none]
 *
 * @retval	none
 */
void NETWORK_Leave()
{
	emberClearBindingTable();
	emberLeaveNetwork();
}
