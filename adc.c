/*
 * adc.c
 *
 *  Created on: 30 nov 2023
 *      Author: MIKE
 */

#include "adc.h"
#include "fsl_adc16.h"
#include "fsl_port.h"
#include "fsl_clock.h"

#define ADC16_CHANNEL       12U
#define ADC16_CHANNEL_GROUP 0U
#define ADC16_BASEADDR      ADC0
#define DMA_CHANNEL         0U
#define DMA_ADC_SOURCE      40U
#define ADC16_RESULT_REG_ADDR    0x4003b010U
#define PTB2 (2u)


void ADC_Setup(void)
{
	CLOCK_EnableClock(kCLOCK_PortB);
	PORT_SetPinMux(PORTB, PTB2, kPORT_PinDisabledOrAnalog);

    adc16_config_t adcUserConfig;

    /*
     * Initialization ADC for 12bit resolution, DMA mode, normal convert speed, VREFH/L as reference,
     * enable continuous convert mode.
     */
    ADC16_GetDefaultConfig(&adcUserConfig);
    adcUserConfig.resolution                 = kADC16_ResolutionSE12Bit;
    adcUserConfig.enableContinuousConversion = true;
    adcUserConfig.clockSource                = kADC16_ClockSourceAlt0;
    adcUserConfig.clockDivider =kADC16_ClockDivider4;
    adcUserConfig.enableLowPower = false;
    adcUserConfig.enableAsynchronousClock = false;
    adcUserConfig.enableHighSpeed= true;
    ADC16_Init(ADC16_BASEADDR, &adcUserConfig);

    /* Auto calibration */
    ADC16_DoAutoCalibration(ADC16_BASEADDR);

    /* Enable software trigger.  */
    ADC16_EnableHardwareTrigger(ADC16_BASEADDR, false);
    /* Enable DMA. */
    ADC16_EnableDMA(ADC16_BASEADDR, true);

	adc16_channel_config_t adcChnConfig;

	adcChnConfig.channelNumber = ADC16_CHANNEL;
	adcChnConfig.enableDifferentialConversion = false;
	adcChnConfig.enableInterruptOnConversionCompleted = false;
	ADC16_SetChannelConfig(ADC16_BASEADDR, ADC16_CHANNEL_GROUP, &adcChnConfig);
}

