/*******************************************************************************

 ******************************************************************************/
#include "app/framework/include/af.h"
#include "button.h"



EmberEventControl buttonPressAndHoldEventControl;
EmberEventControl buttonReleaseEventControl;


BUTTON_holdingEvent_t holdingCallbackFunc = NULL;
BUTTON_pressAndHoldEvent_t pressAndHoldingCallbackFunc = NULL;
BUTTONx_t buttonArray[BUTTON_COUNT] = BUTTON_INIT;


void buttonPressAndHoldEventHandle(void);
void buttonReleaseEventHandle(void);

static void 	halInternalButtonIsr(uint8_t pin);
static uint8_t 	getButtonIndex(uint8_t pin);
static void 	resetButtonParameter(uint8_t index);

/*
 * @func	buttonInit
 * @brief	Initialize Button
 * @param	holdingHandle, pressAndHoldeHandler
 * @retval	None
 */
void buttonInit(BUTTON_holdingEvent_t holdingHandle,BUTTON_pressAndHoldEvent_t pressAndHoldeHandler)
{
  GPIOINT_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);
  uint8_t i;
  for ( i = 0; i < BUTTON_COUNT; i++ ) {
    /* Configure pin as input */
    GPIO_PinModeSet(buttonArray[i].port,
                    buttonArray[i].pin,
					gpioModeInput,
					GPIO_DOUT);
    /* Register callbacks before setting up and enabling pin interrupt. */
    GPIOINT_CallbackRegister(buttonArray[i].pin,
                             halInternalButtonIsr);
    /* Set rising and falling edge interrupts */
    GPIO_ExtIntConfig(buttonArray[i].port,
                      buttonArray[i].pin,
                      buttonArray[i].pin,
                      true,
                      true,
                      true);
  }

  holdingCallbackFunc=holdingHandle;
  pressAndHoldingCallbackFunc=pressAndHoldeHandler;

}
/*
 * @func	halInternalButtonIsr
 * @brief	IRQ button
 * @param	pin
 * @retval	None
 */
void halInternalButtonIsr(uint8_t pin)
{
  uint8_t buttonStateNow;
  uint8_t buttonStatePrev;
  uint32_t debounce;
  uint8_t buttonIndex;

  buttonIndex = getButtonIndex(pin);
  // check valid index
  if(buttonIndex==-1)
	  return;

  buttonStateNow = GPIO_PinInGet(buttonArray[buttonIndex].port, buttonArray[buttonIndex].pin);
  for ( debounce = 0;
        debounce < BUTTON_DEBOUNCE;
        debounce = (buttonStateNow == buttonStatePrev) ? debounce + 1 : 0 ) {
    buttonStatePrev = buttonStateNow;
    buttonStateNow = GPIO_PinInGet(buttonArray[buttonIndex].port, buttonArray[buttonIndex].pin);
  }

  buttonArray[buttonIndex].state = buttonStateNow;

  if(buttonStateNow == BUTTON_PRESS)
  {
	  buttonArray[buttonIndex].pressCount ++;
	  if(buttonArray[buttonIndex].press != true)
	  {
		  emberEventControlSetActive(buttonPressAndHoldEventControl);
	  }

	  buttonArray[buttonIndex].isHolding=false;
	  buttonArray[buttonIndex].holdTime=0;
	  buttonArray[buttonIndex].press = true;
	  buttonArray[buttonIndex].release = false;
  }
  else
  {

	  buttonArray[buttonIndex].release = true;
	  buttonArray[buttonIndex].press = false;
	  emberEventControlSetInactive(buttonReleaseEventControl);
	  emberEventControlSetDelayMS(buttonReleaseEventControl,BUTTON_CHECK_RELEASE_MS);
  }
}

/*
 * @func	buttonPressAndHoldEventHandle
 * @brief	Event Button Handler
 * @param	None
 * @retval	None
 */
void buttonPressAndHoldEventHandle(void)
{
	emberEventControlSetInactive(buttonPressAndHoldEventControl);
	bool holdTrigger =false;
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].press ==true)
		{
			holdTrigger = true;
			buttonArray[i].holdTime ++;
			if(buttonArray[i].holdTime>=5)
			{
				buttonArray[i].isHolding=true;
				buttonArray[i].pressCount=0;
			}

			if(holdingCallbackFunc != NULL)
			{
				if((buttonArray[i].holdTime %5)==0)
				{
					holdingCallbackFunc(i,buttonArray[i].holdTime/5 + press_max);
				}
			}
		}
	}
	if(holdTrigger == true)
		emberEventControlSetDelayMS(buttonPressAndHoldEventControl,BUTTON_CHECK_HOLD_CYCLES_MS);
}

/*
 * @func	buttonReleaseEventHandle
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void buttonReleaseEventHandle(void)
{
	emberEventControlSetInactive(buttonReleaseEventControl);
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].release == true)
		{
			if(pressAndHoldingCallbackFunc != NULL)
			{
				if(buttonArray[i].isHolding==false)
				{
					pressAndHoldingCallbackFunc(i, buttonArray[i].pressCount >= press_max ? unknown:buttonArray[i].pressCount);
				}else
				{
					pressAndHoldingCallbackFunc(i, (buttonArray[i].holdTime/5 + press_max) >= hold_max ? unknown :(buttonArray[i].holdTime/5 + press_max));
				}
			}

			resetButtonParameter(i);
		}
	}
}

/*
 * @func	resetButtonParameter
 * @brief	Reset parameter
 * @param	index
 * @retval	None
 */
static void resetButtonParameter(uint8_t index)
{
	buttonArray[index].holdTime 	= 0;
	buttonArray[index].pressCount	= 0;
	buttonArray[index].press		= false;
	buttonArray[index].release		= false;
	buttonArray[index].state		= 0;
	buttonArray[index].isHolding    =false;
}

/*
 * @func	getButtonIndex
 * @brief	get number button
 * @param	pin
 * @retval	None
 */
static uint8_t getButtonIndex(uint8_t pin)
{
	for(int i=0; i<BUTTON_COUNT; i++)
	{
		if(buttonArray[i].pin == pin)
			return i;
	}
	return -1;
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
//			turnOnLed(LED1,ledBlue);
//			SEND_OnOffStateReport(1,LED_ON);
			break;
		case press_2:
			emberAfCorePrintln("press_2!!!");
//			turnOffRBGLed(LED1);
//			SEND_OnOffStateReport(1,LED_OFF);
			break;
		case press_3:// target find
			emberAfCorePrintln("press_3!!!");
//			(void) emberAfPluginFindAndBindTargetStart(1);
			break;
		case press_4:// initiator find
			emberAfCorePrintln("press_4!!!");
//			(void) emberAfPluginFindAndBindInitiatorStart(1);
			break;
		case press_5:
			emberAfCorePrintln("press_5!!!");
//			NETWORK_Leave();
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

}

