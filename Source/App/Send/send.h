/*
 * send.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_SEND_SEND_H_
#define SOURCE_APP_SEND_SEND_H_

#include "app/framework/include/af.h"
#include "Source/Mid/led/led.h"
#include "Source/App/Network/network.h"
#include "Source/App/Receive/received.h"
#include "Source/App/Main/main.h"

void SEND_ReportInfoToHc();
void SEND_OnOffStateReport(uint8_t endpoint, ledOnOffState_e state);
void SEND_CommandBinding(uint8_t cmdID,uint8_t sourceEndpoint,uint8_t desEndpoint, EmberNodeId remoteAddess);

#endif /* SOURCE_APP_SEND_SEND_H_ */
