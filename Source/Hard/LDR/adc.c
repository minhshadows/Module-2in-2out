/*
 * adc.c
 *
 *  Created on: Sep 24, 2023
 *      Author: Minh
 */
#include "adc.h"

/**
 * @func	adc_Initt
 *
 * @brief	initialization ADC
 *
 * @param	[none]
 *
 * @retval	none
 */
void adc_Initt()
{
	// Declare initialization structures
	IADC_Init_t adc_struct = IADC_INIT_DEFAULT;
	IADC_AllConfigs_t  init_AllConfigs = IADC_ALLCONFIGS_DEFAULT;
	IADC_InitSingle_t init_Single = IADC_INITSINGLE_DEFAULT;
	IADC_SingleInput_t single_Input = IADC_SINGLEINPUT_DEFAULT;

	//enable ADC clock
	CMU_ClockEnable(cmuClock_IADC0, true);
//	CMU_ClockEnable(cmuClock_GPIO, true);

	//reset ADC0
	IADC_reset(IADC0);

	// Use the FSRC0 as the IADC clock so it can run in EM2
	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

	adc_struct.warmup = iadcWarmupNormal; // shutdown after each conversion.

	// Set the HFSCLK prescale value here
	adc_struct.srcClkPrescale = (IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0));

	init_AllConfigs.configs[0].reference 		= iadcCfgReferenceVddx;
//	init_AllConfigs.configs[0].vRef 			= 1210; 					// Vref
//	init_AllConfigs.configs[0].analogGain 		= iadcCfgAnalogGain0P5x;

	init_AllConfigs.configs[0].adcClkPrescale 	= IADC_calcAdcClkPrescale(IADC0,
																		CLK_ADC_FREQ,
																		0,
																		iadcCfgModeNormal,
																		adc_struct.srcClkPrescale);

	init_AllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;

	init_Single.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;

	single_Input.posInput = iadcPosInputPortCPin5 ;
	single_Input.negInput = iadcNegInputGnd;

	// Initialize IADC
	IADC_init(IADC0, &adc_struct, &init_AllConfigs);

	// Initialize scan
	IADC_initSingle(IADC0, &init_Single, &single_Input);

	// Allocate the analog bus for ADC0 inputs
	GPIO->IADC_INPUT_0_BUS |= GPIO_CDBUSALLOC_CDODD0_ADC0;

	// Initialize Kalman Filter
	KalmanFilterInit(0.1,1,0.01);
}

/**
 * @func	get_SampleADC
 *
 * @brief	get sample ADC
 *
 * @param	[none]
 *
 * @retval	[uint32_t] : return value
 */
uint32_t get_SampleADC()
{
	uint32_t sample = 0;
	IADC_command(IADC0, iadcCmdStartSingle);

	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV);

	sample = IADC_pullSingleFifoResult(IADC0).data;

	return sample;
}

/**
 * @func	LightSensor_Measure
 *
 * @brief	get sample ADC and filter it with kalman filter
 *
 * @param	[none]
 *
 * @retval	[uint32_t] : return value
 */
uint32_t LightSensor_Measure()
{
//	emberAfCorePrintln("--------------------------");
	uint32_t temp = get_SampleADC();
	uint32_t lux = KalmanFilter_updateEstimate(temp);

//	emberAfCorePrintln("lux = %d ",lux);
//	emberAfCorePrintln("--------------------------");
	return lux;
}

/**
 * @func	CalculateLux
 *
 * @brief	send lux to HC if lux change 30 unit
 *
 * @param	[none]
 *
 * @retval	none
 */
uint32_t luxx = 0;
void CalculateLux()
{
	uint32_t temp = LightSensor_Measure();;
	if(luxx > temp)
	{
		if((luxx - temp) >= 30)
		{
			SEND_measuredValueReport(3,(uint16_t)temp);
		}
	}
	else if(luxx < temp)
	{
		if((temp - luxx) >= 30)
		{
			SEND_measuredValueReport(3,(uint16_t)temp);
		}
	}
	adc_ToggleLed();
	luxx = temp;
}

/**
 * @func	adc_ToggleLed
 *
 * @brief	toggle led follow lux
 *
 * @param	[none]
 *
 * @retval	none
 */
void adc_ToggleLed()
{
	if (luxx >= 500)
	{
		turnOnLed(LED2,ledGreen);
//		emberAfCorePrintln("led on");
	}else if (luxx < 500)
	{
		turnOffRBGLed(LED2);
//		emberAfCorePrintln("led off");
	}
}
