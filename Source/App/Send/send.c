/*
 * send.c
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#include "send.h"

static void SEND_FillBufferGlobalComman(EmberAfClusterId clusterID,
										EmberAfAttributeId aitributeID,
										uint8_t globalCommand,
										uint8_t* value,
										uint8_t length,
										uint8_t datatype)
{
	uint8_t data[255];
	data[0] = (uint8_t) (aitributeID & 0x00FF);
	data[1] = (uint8_t) ((aitributeID & 0xFF00) >> 8);
	data[2] = EMBER_SUCCESS;
	data[3] = (uint8_t) datatype;
	memcpy(&data[4], value, length);

	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
										clusterID,
										globalCommand,
										"b",
										data,
										length+4);
}

static void SEND_SendCommandUnicast(uint8_t source,uint8_t destination,uint16_t address)
{
	emberAfSetCommandEndpoints(source,destination);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}

/**
 * @func	SEND_CommandBinding
 *
 * @brief	Send command from initiator to target
 *
 * @param	[cmdID]: command ID
 *
 * @param	[indexTable]: index of table
 *
 * @retval	none
 */
void SEND_CommandBinding(uint8_t cmdID,uint8_t sourceEndpoint,uint8_t desEndpoint, EmberNodeId remoteAddess)
{
	emberAfFillExternalBuffer(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER ,
								ZCL_ON_OFF_CLUSTER_ID,
								cmdID,
								"");

	SEND_SendCommandUnicast(sourceEndpoint,desEndpoint, remoteAddess);
}

/**
 * @func	SEND_ReportInfoToHc
 *
 * @brief	Send model ID to HC
 *
 * @param	none
 *
 * @retval	none
 */
void SEND_ReportInfoToHc()
{
	uint8_t modelID[4] = {3, 'S', 'W', '2'};
//	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
//	uint8_t version = 1;
	uint16_t addressHC = 0x0000;//emberGetParentId();
	emberAfCorePrintln("Report!!!");
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalComman(ZCL_BASIC_CLUSTER_ID,
								ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								modelID,
								8,
								ZCL_CHAR_STRING_ATTRIBUTE_TYPE);


	SEND_SendCommandUnicast(1,1,addressHC);
}

/**
 * @func	SEND_OnOffStateReport
 *
 * @brief	Send led state to HC
 *
 * @param	[endpoint]:
 *
 * @param	[state]: state of led
 *
 * @retval	none
 */
void SEND_OnOffStateReport(uint8_t endpoint, ledOnOffState_e state)
{
	uint16_t HC_address =0x0000;
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &state,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalComman(ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &state,
								1,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(endpoint,1,HC_address);
}

