/*
 * dac.c
 *
 *  Created on: 30 sep 2023
 *      Author: MIKE
 */

#include <stdint.h>
#include "dac.h"
#include "fsl_dac.h"

//DAC configuration data types
static  dac_config_t DacConfig;

#define BUFF_MAX_INDEX 15u

/*!
 * DAC_setup configures and initializes the DAC
 * in its circular buffer working mode
 * and enables the DAC's buffer
 */
void DAC_setup(void)
{
	DAC_GetDefaultConfig(&DacConfig);
	DAC_Init(DAC0, &DacConfig);
	DAC_Enable(DAC0, true);
	DAC_SetBufferValue(DAC0, 0, 0x00);
}
