/*
 * rl_sig.h
 *
 *  Created on: Feb 1, 2024
 *      Author: Minh
 */

#ifndef SOURCE_HARDD_RL_SIG_RL_SIG_H_
#define SOURCE_HARDD_RL_SIG_RL_SIG_H_

#include "app/framework/include/af.h"

#define	ON							true
#define OFF							false


#define RL_COUNT					2
#define SIG_COUNT					2

#define RL_Port_1					gpioPortC
#define RL_Pin_1					3U
#define RL_Port_2					gpioPortC
#define RL_Pin_2					4U

#define SIG_Port_1					gpioPortC
#define SIG_Pin_1					0U
#define SIG_Port_2					gpioPortA
#define SIG_Pin_2					0U


#define RL_INIT						{{RL_Port_1,RL_Pin_1},{RL_Port_2,RL_Pin_2}}
#define SIG_INIT					{{SIG_Port_1,SIG_Pin_1},{SIG_Port_2,SIG_Pin_2}}

typedef struct{
  GPIO_Port_TypeDef		port;
  unsigned int			pin;
  bool					output_default;
  uint8_t				waitTime;
  bool					autoON;
  bool					delayON;
}RELAYx_t;

typedef struct{
  GPIO_Port_TypeDef		port;
  unsigned int			pin;
  bool					ToggleSwitch;
  bool					AnyChange;
  bool					MomentarySwitch;
}SIGNALx_t;

void relayInit();
void turnOffRelay();
void turnOnRelay();

void sigInit();

#endif /* SOURCE_HARDD_RL_SIG_RL_SIG_H_ */
