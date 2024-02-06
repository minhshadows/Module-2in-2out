/*
 * rl_sig.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Minh
 */
#include "rl_sig.h"

RELAYx_t relayArr[RL_COUNT] = RL_INIT;
SIGNALx_t sigArr[SIG_COUNT] = SIG_INIT;

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
	turnOffRelay();
}

/**
 * @func	turnOffRelay
 *
 * @brief	Turn off relay
 *
 * @param	[none]
 *
 * @retval	none
 */
void turnOffRelay()
{
	for(uint8_t j=0;j<RL_COUNT;j++)
	{
		GPIO_PinOutSet(relayArr[j].port, relayArr[j].pin);
	}
}

/**
 * @func	turnOnRelay
 *
 * @brief	turn on Relay
 *
 * @param	[none]
 *
 * @retval	none
 */
void turnOnRelay()
{
	for(uint8_t j=0;j<RL_COUNT;j++)
	{
		GPIO_PinOutClear(relayArr[j].port, relayArr[j].pin);
	}
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
	    /* Register callbacks before setting up and enabling pin interrupt. */
	    GPIOINT_CallbackRegister(sigArr[i].pin,
	                             halInternalButtonIsr);
	    /* Set rising and falling edge interrupts */
	    GPIO_ExtIntConfig(	sigArr[i].port,
	    					sigArr[i].pin,
							sigArr[i].pin,
	                      true,
	                      true,
	                      true);
	}


}


