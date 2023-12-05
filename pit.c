/*
 * pit.c
 *
 *  Created on: 4 oct 2023
 *      Author: MIKE
 */

#include "pit.h"
#include "fsl_pit.h"

typedef enum{
	False = 0u,
	True = 1u
}Bool_t;

static uint8_t g_Pit_0_flag = False;
static uint8_t g_Pit_1_flag = False;
static uint8_t g_Pit_2_flag = False;

/*!
 * PIT_init
 * Initializes PIT timer channels 0,1 and 2
 * */
void PIT_init(void)
{
	pit_config_t pitConf;
	PIT_GetDefaultConfig(&pitConf);
	PIT_Init(PIT,&pitConf);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_2, kPIT_TimerInterruptEnable);
	EnableIRQ(PIT2_IRQn);
}

/*!
 * PIT_startxTimer
 * Starts the specified PIT timer
 *
 * @param chnl The PIT chanel to start
 * @param delayUs the time in microseconds for the timer
 * */
void PIT_startxTimer(PIT_CHNL_t chnl, uint32_t delayUs)
{
	uint32_t clkFreq = CLOCK_GetCoreSysClkFreq();
	switch(chnl)
	{
	case kPit_0:
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(delayUs, clkFreq));
		PIT_StartTimer(PIT, kPIT_Chnl_0);
	break;
	case kPit_1:
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, USEC_TO_COUNT(delayUs, clkFreq));
		PIT_StartTimer(PIT, kPIT_Chnl_1);
	break;
	case kPit_2:
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_2, USEC_TO_COUNT(delayUs, clkFreq));
		PIT_StartTimer(PIT, kPIT_Chnl_2);
	break;
	default:
	break;
	}

}

/*!
 * PIT_stopxTimer
 * Stops the specified PIT timer
 *
 * @param chnl The PIT chanel to stop
 * */
void PIT_stopxTimer(PIT_CHNL_t chnl)
{
	switch(chnl)
	{
	case kPit_0:
		PIT_StopTimer(PIT, kPIT_Chnl_0);
	break;
	case kPit_1:
		PIT_StopTimer(PIT, kPIT_Chnl_0);
	break;
	case kPit_2:
		PIT_StopTimer(PIT, kPIT_Chnl_0);
	break;
	default:
	break;
	}

}

/*!
 * PIT_setISRFlags
 * Sets the interrupt status flags for a specific PIT channel
 *
 * @param pitChnl (kPit_0,kPit_1)
 * */
void PIT_setISRFlags(PIT_CHNL_t pitChnl)
{
	if(kPit_0 == pitChnl)
	{
		g_Pit_0_flag = True;
	}
	if(kPit_1== pitChnl)
	{
		g_Pit_1_flag = True;
	}
	if(kPit_2== pitChnl)
	{
		g_Pit_2_flag = True;
	}
}

/*!
 * PIT_clearISRFlags
 * Clears the interrupt status flags for a specific PIT channel
 *
 * @param pitChnl (kPit_0,kPit_1)
 * */
void PIT_clearISRFlags(PIT_CHNL_t pitChnl)
{
	if(kPit_0 == pitChnl)
	{
		g_Pit_0_flag = False;
	}
	if(kPit_1== pitChnl)
	{
		g_Pit_1_flag = False;
	}
	if(kPit_2== pitChnl)
	{
		g_Pit_2_flag = False;
	}
}

/*!
 * PIT_getISRFlags
 * Gets the interrupt status flags for a specific PIT channel
 *
 *@retval flag The value of the status flag
 * @param pitChnl (kPit_0,kPit_1)
 * */
uint8_t PIT_getISRFlags(PIT_CHNL_t pitChnl)
{
	uint8_t flag;
	if(kPit_0 == pitChnl)
	{
		flag = g_Pit_0_flag;
	}
	if(kPit_1== pitChnl)
	{
		flag = g_Pit_1_flag;
	}
	if(kPit_2== pitChnl)
	{
		flag = g_Pit_2_flag;
	}
	return flag;
}

/*!
 * PIT0_IRQHandler
 * Interrupt service routine for PIT channel 0
 * */
void PIT0_IRQHandler(void)
{
	PIT_setISRFlags(kPit_0);
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, 0xFFFFFFFF);
}
/*!
 * PIT1_IRQHandler
 * Interrupt service routine for PIT channel 1
 * */
void PIT1_IRQHandler(void)
{
	PIT_setISRFlags(kPit_1);
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_1, 0xFFFFFFFF);
}
/*!
 * PIT2_IRQHandler
 * Interrupt service routine for PIT channel 2
 * */
void PIT2_IRQHandler(void)
{
	PIT_setISRFlags(kPit_2);
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_2, 0xFFFFFFFF);
}

