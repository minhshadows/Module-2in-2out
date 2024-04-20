/*
 * received.c
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#include "received.h"

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
		case ZCL_ON_OFF_CLUSTER_ID:
			RECEIVE_HandleOnOffCluster(cmd);
			return true;
		default:
			break;
		}
	}
  return false;
}

/**
 * @func	RECEIVE_HandleBasicCluster
 *
 * @brief	Turn on/off led follow command ID
 *
 * @param	[cmd]: packet message result from callback
 *
 * @retval	none
 */
void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID				= cmd->commandId;
	uint8_t destination_Endpoint 	= cmd->apsFrame->destinationEndpoint;
	EmberNodeId source_Address		= cmd->source;

	if(cmd->type == EMBER_INCOMING_UNICAST)
	{
		switch(commandID)
		{
		case ZCL_OFF_COMMAND_ID:
			if(destination_Endpoint == 1)
			{
				set_RelayState(RELAY1,OFF);
				switch_StatusChange(RELAY1);
				GET_BindingTalbe(commandID,source_Address,destination_Endpoint);
			}
			else if(destination_Endpoint == 2)
			{
				set_RelayState(RELAY2,OFF);
				switch_StatusChange(RELAY2);
				GET_BindingTalbe(commandID,source_Address,destination_Endpoint);
			}

			break;
		case ZCL_ON_COMMAND_ID:
			if(destination_Endpoint == 1)
			{
				set_RelayState(RELAY1,ON);
				switch_StatusChange(RELAY1);
				GET_BindingTalbe(commandID,source_Address,destination_Endpoint);
			}
			else if(destination_Endpoint == 2)
			{
				set_RelayState(RELAY2,ON);
				switch_StatusChange(RELAY2);
				GET_BindingTalbe(commandID,source_Address,destination_Endpoint);
			}
			break;
		default:
			break;
		}
	}
}

/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback.

 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note: 	This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
  return false;
}

/**
 * @func	GET_BindingTalbe
 *
 * @brief	get binding table and process message to send target
 *
 * @param	[cmdID] : command ID
 *
 * @param	[source] : NodeID of initiatal
 *
 * @retval	none
 */
void GET_BindingTalbe(uint8_t cmdID,EmberNodeId source_Address,uint8_t localEndpoint)
{
	EmberBindingTableEntry talbe;
	EmberNodeId remoteNodeID;
	for(uint8_t i=0; i<emberAfGetBindingTableSize();i++)
	{
		if(emberGetBinding(i,&talbe) == EMBER_SUCCESS)
		{
			if(talbe.clusterId == ZCL_ON_OFF_CLUSTER_ID)
			{
				emberAfCorePrintln("----------%d-----------",i);
				emberAfCorePrintln("cluster id: %X",talbe.clusterId);
				emberAfCorePrintln("networkIndex: %d",talbe.networkIndex);
				emberAfCorePrintln("type		: %d",talbe.type);
				emberAfCorePrintln("identifier	: %X",talbe.identifier);
				emberAfCorePrintln("local		: %d",talbe.local);
				emberAfCorePrintln("remote		: %d",talbe.remote);
				emberAfCorePrintln("-----------------------");
				if(talbe.local == localEndpoint)
				{
					emberAfCorePrintln("localEndpoint == %d",localEndpoint);
					remoteNodeID = emberGetBindingRemoteNodeId(i);
					if(source_Address != remoteNodeID)
					{
						emberAfCorePrintln("Node == %X",remoteNodeID);
						SEND_CommandBinding(cmdID,localEndpoint,talbe.remote,remoteNodeID);
					}
				}
			}
		}
	}
}
