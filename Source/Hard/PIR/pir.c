/*
 * pir.c
 *
 *  Created on: Aug 18, 2023
 *      Author: Minh
 */


#include <Source/Hard/PIR/pir.h>
#include "Source/Mid/led/led.h"

pirState_t pirState;

EmberEventControl pirDetechEventControl;
pirActionHandle_t pirHardHandle = NULL;
static void PIR_INTSignalHandle(uint8_t pin);
boolean isMotionSignal();
void PIR_Enable(boolean en);

/**
 * @func	RECEIVE_HandleBasicCluster
 *
 * @brief	initialization PIR
 *
 * @param	[userPirHandle]: handle call back
 *
 * @retval	none
 */
void pirInit(pirActionHandle_t userPirHandle)
{
	GPIO_PinModeSet(PIR_PORT,PIR_PIN,gpioModeInput,1);

	/*Register callbacks before setting up and enabling pin interrupt. */
	GPIOINT_CallbackRegister(PIR_PIN,PIR_INTSignalHandle);

	/*Set rising and falling edge interrupts */

	pirHardHandle=userPirHandle;

	PIR_Enable(true);

	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/**
 * @func	PIR_Enable
 *
 * @brief	process follow command ID
 *
 * @param	[boolean]: turn on/off interrupt of PIR
 *
 * @retval	none
 */
void PIR_Enable(boolean en)
{
	if(en)
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,true,false,true);
	}
	else
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,false,false,false);
	}
}

/**
 * @func	PIR_INTSignalHandle
 *
 * @brief	while detech motion turn on Detechhandle
 *
 * @param	[pin]
 *
 * @retval	none
 */
static void PIR_INTSignalHandle(uint8_t pin)
{
	if(pin != PIR_PIN)
	{
		return;
	}
	if(isMotionSignal())
	{
		pirState = PIR_STATE_DEBOUNCE;
		PIR_Enable(false);

		emberEventControlSetInactive(pirDetechEventControl);
		emberEventControlSetDelayMS(pirDetechEventControl,200);
	}
}

/**
 * @func	pirDetechEventHandle
 *
 * @brief	handle pir detech event
 *
 * @param	[none]
 *
 * @retval	none
 */
void pirDetechEventHandle()
{
	emberEventControlSetInactive(pirDetechEventControl);

	switch(pirState)
	{
	case PIR_STATE_DEBOUNCE:
		if(isMotionSignal())
		{
			emberAfCorePrintln("PIR_DETECH_MOTION");
			pirState = PIR_STATE_5S;
			if(pirHardHandle != NULL)
			{
				pirHardHandle(PIR_MOTION);
			}
			emberEventControlSetDelayMS(pirDetechEventControl,5000);
		}
		else
		{
			PIR_Enable(true);
		}
		break;
	case PIR_STATE_5S:
		pirState = PIR_STATE_WAIT_30S;
		PIR_Enable(true);
		emberEventControlSetDelayMS(pirDetechEventControl,30000);
		break;
	case PIR_STATE_WAIT_30S:
		if(pirHardHandle != NULL)
		{
			emberAfCorePrintln("PIR_DETECH_UNMOTION");
			pirHardHandle(PIR_UNMOTION);
		}
		break;
	default:
		break;
	}
}

/**
 * @func	isMotionSignal
 *
 * @brief	check pir signal
 *
 * @param	[none]
 *
 * @retval	[boolean] return 1 while motion, 0 while unmotion
 */
boolean isMotionSignal()
{
	return (GPIO_PinInGet(PIR_PORT,PIR_PIN) == 0) ? false:true;
}
