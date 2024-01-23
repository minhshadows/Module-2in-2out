/*
 * pir.h
 *
 *  Created on: Aug 18, 2023
 *      Author: Minh
 */

#ifndef SOURCE_HARD_PIR_PIR_H_
#define SOURCE_HARD_PIR_PIR_H_

#include "app/framework/include/af.h"
#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#define PIR_PORT					gpioPortC
#define PIR_PIN						(4U)

typedef enum
{
	PIR_UNMOTION,
	PIR_MOTION
}pirAction_e;

typedef enum
{
	PIR_STATE_DEBOUNCE,
	PIR_STATE_5S,
	PIR_STATE_WAIT_30S
}pirState_t;

typedef void (*pirActionHandle_t)(pirAction_e pinAction);

void pirInit(pirActionHandle_t userPirHandle);

#endif /* SOURCE_HARD_PIR_PIR_H_ */
