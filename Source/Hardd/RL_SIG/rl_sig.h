/*
 * rl_sig.h
 *
 *  Created on: Feb 1, 2024
 *      Author: Minh
 */

#ifndef SOURCE_HARDD_RL_SIG_RL_SIG_H_
#define SOURCE_HARDD_RL_SIG_RL_SIG_H_

#include "app/framework/include/af.h"
#include "Source/App/Send/send.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#define	ON							true
#define OFF							false

#define MAX_DEBOUNCE				5
#define MAX_HOLD					350

#define RL_COUNT					2
#define SIG_COUNT					2

#define RL_Port_2					gpioPortC
#define RL_Pin_2					3U
#define RL_Port_1					gpioPortC
#define RL_Pin_1					4U

#define SIG_Port_1					gpioPortC
#define SIG_Pin_1					0U
#define SIG_Port_2					gpioPortA
#define SIG_Pin_2					0U


#define RL_INIT						{{RL_Port_1,RL_Pin_1,output_default,OFF,5000},{RL_Port_2,RL_Pin_2,output_default,OFF,5000}}
#define SIG_INIT					{{SIG_Port_1,SIG_Pin_1},{SIG_Port_2,SIG_Pin_2}}

typedef enum{
	RELAY1,
	RELAY2
}relayNumber;

typedef enum{
	INPUT1,
	INPUT2
}inputNumber;

typedef enum{
	ToggleSwitch,
	AnyChange,
	MomentarySwitch
}INPUT_MODE_e;

typedef enum{
	output_default,
	autoON,
	autoOFF,
	delayON,
	delayOFF
}OUTPUT_MODE_e;

typedef struct{
  GPIO_Port_TypeDef		port;
  unsigned int			pin;
  OUTPUT_MODE_e			mode;
  bool					state;
  uint32_t				waitTime;
}RELAYx_t;

typedef struct{
  GPIO_Port_TypeDef		port;
  unsigned int			pin;
  INPUT_MODE_e			mode;
  bool					state;
  uint32_t				holdCount;
  bool					holdTrigger;
  bool					press;
  bool					release;
}SIGNALx_t;

void relayInit();
void turnOffRelay(uint8_t index);
void turnOnRelay(uint8_t index);
void switch_StatusChange(uint8_t relayID);
void output_ModeSet(relayNumber id,OUTPUT_MODE_e mode);
void output_SetTime(relayNumber id,uint32_t time);

void sigInit();
void input_ModeCheck(uint8_t signal_id);
void input_ModeSet(inputNumber id,INPUT_MODE_e mode);

void set_RelayState(relayNumber id, bool state);
uint8_t get_Relaystate(uint8_t relayID);

void test_UART();

#endif /* SOURCE_HARDD_RL_SIG_RL_SIG_H_ */
