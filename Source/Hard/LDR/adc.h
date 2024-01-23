/*
 * adc.h
 *
 *  Created on: Sep 24, 2023
 *      Author: Minh
 */

#ifndef SOURCE_HARD_LDR_ADC_H_
#define SOURCE_HARD_LDR_ADC_H_

#include "app/framework/include/af.h"
#include "Source/Kalman_filter/kalman_filter.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/led/led.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"

#define ADC_PORT					(gpioPortC)
#define ADC_PIN						5U
#define IADC_INPUT_0_PORT_PIN		iadcPosInputPortCPin5

#define IADC_INPUT_0_BUS         	CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC    	_GPIO_CDBUSALLOC_CDODD0_ADC0

#define IADC_LDMA_CH             	0

// Set CLK_ADC to 10 MHz
#define CLK_SRC_ADC_FREQ    		20000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ        		10000000  // CLK_ADC - 10 MHz max in normal mode


void adc_Initt();

uint32_t LightSensor_Measure();
void CalculateLux();
void adc_ToggleLed();

#endif /* SOURCE_HARD_LDR_ADC_H_ */
