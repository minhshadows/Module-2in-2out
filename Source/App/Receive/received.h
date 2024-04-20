/*
 * received.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVED_H_
#define SOURCE_APP_RECEIVE_RECEIVED_H_

#include "app/framework/include/af.h"
#include "Source/Mid/led/led.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Main/main.h"
#include "Source/Hardd/RL_SIG/rl_sig.h"

void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd);
void GET_BindingTalbe(uint8_t cmdID,EmberNodeId source_Address,uint8_t localEndpoint);

#endif /* SOURCE_APP_RECEIVE_RECEIVED_H_ */
