/*
 * Final porject
 * Sistemas Embebidos Basados en Microcontroladores
 *
 * Authors:
 * José Miguel Castellanos Padilla
 * Diego Moran Vergara
 *
 * Description:
 *
 * The system consists of an audio recorder that uses the SPI memory S25FL164K to store
 * the audio samples read from the ADC module. The sampling rate is 30kHz.
 *
 * This system is also capable of sending the sampled audio directly to the DAC.
 *
 * The system uses convolution to add different effects to the audio
 *
 * */

#include <stdint.h>
#include "adc.h"
#include "buttons.h"
#include "dma.h"
#include "dac.h"
#include "display.h"
#include "gpio.h"
#include "LCD_nokia.h"
#include "NVIC.h"
#include "pit.h"
#include "recorder.h"
#include "S25FL164K.h"
#include "SPI.h"
#include "system_clock.h"


//State machine data type
typedef struct{
	uint8_t out;
	uint8_t next[5];
}State_t;

//State machine Array of states
const State_t FSM_Moore [5] =
{
	{kDisplay_M0,{kDisplay_M0, kDisplay_MRealT,kDisplay_MPlay,kDisplay_MRecord,kDisplay_MRecording}},
	{kDisplay_MRealT,{kDisplay_M0, kDisplay_MRealT,kDisplay_MPlay,kDisplay_MRecord,kDisplay_MRecording}},
	{kDisplay_MPlay,{kDisplay_M0, kDisplay_MRealT,kDisplay_MPlay,kDisplay_MRecord,kDisplay_MRecording}},
	{kDisplay_MRecord,{kDisplay_M0, kDisplay_MRealT,kDisplay_MPlay,kDisplay_MRecord,kDisplay_MRecording}},
	{kDisplay_MRecording,{kDisplay_M0, kDisplay_MRealT,kDisplay_MPlay,kDisplay_MRecord,kDisplay_MRecording}}
};

int main(void)
{
 	SYSTEM_CLOCK_setup();
	CLOCK_SetSimSafeDivs();

	Menu_t current_state = kDisplay_M0;
	uint8_t input = 0u;						//Machine input
	uint8_t output = 0u;					//Machine output
	uint8_t last_output = 0u;				//Machine last output
	uint8_t btn = 0u;

	//Interrupt priorites setup
	NVIC_set_basepri_threshold(PRIORITY_6);
	NVIC_enable_interrupt_and_priotity(DMA_CH0_IRQ, PRIORITY_1);
	NVIC_enable_interrupt_and_priotity(DMA_CH1_IRQ, PRIORITY_1);
	NVIC_enable_interrupt_and_priotity(DMA_CH2_IRQ, PRIORITY_1);
	NVIC_enable_interrupt_and_priotity(PIT_CH0_IRQ, PRIORITY_2);
	NVIC_enable_interrupt_and_priotity(PIT_CH1_IRQ, PRIORITY_2);
	NVIC_enable_interrupt_and_priotity(PIT_CH2_IRQ, PRIORITY_2);
	NVIC_enable_interrupt_and_priotity(PORTA_IRQ, PRIORITY_3);
	NVIC_enable_interrupt_and_priotity(PORTB_IRQ, PRIORITY_3);
	NVIC_enable_interrupt_and_priotity(PORTC_IRQ, PRIORITY_3);
	NVIC_enable_interrupt_and_priotity(PORTD_IRQ, PRIORITY_3);
	NVIC_global_enable_interrupts;

	/*Initialization of peripherals*/
	BUTTONS_init();
	BUTTONS_interrupt_enable();
	PIT_init();
	SPI_config();
	LCD_nokia_init();
	DAC_setup();
	ADC_Setup();
	DMA_init();
	/*Initial view of the display*/
	DISPLAY_MenuSelec(kDisplay_M0);
	while (1)
	{
		//Getting the output value of the current SM state
		output = FSM_Moore[current_state].out;

		if(last_output != output)
		{
			last_output = output;
		}
		//If the Port A 1 switch has been depressed, go back to the main menu
		if(GPIO_GetISR_StatusFlags(kGPIO_A))
		{
			btn = BTN0;
			input = RECORDER_mode(output, btn);
			GPIO_ClearISR_StatusFlags(kGPIO_A, PTA1);
			btn = 0u;
		}


		//If the Port C switches has been depressed, check wich was depressed
		if(GPIO_GetISR_StatusFlags(kGPIO_C))
		{
			btn = GPIO_GetISR_StatusFlags(kGPIO_C);
			//B1 depressed
			if(B1_flag == btn)
			{
				btn = BTN1;
				input = RECORDER_mode(output, btn);
				GPIO_ClearISR_StatusFlags(kGPIO_C, PTC16);
				btn = 0u;

			}
			//B2 depressed
			else if(B2_flag == btn)
			{
				btn = BTN2;
				input = RECORDER_mode(output, btn);
				GPIO_ClearISR_StatusFlags(kGPIO_C, PTC17);
				btn = 0u;
			}
			//If something else occurs, clear the status flags and reset the btn state
			else
			{
				btn = 0u;
				GPIO_ClearISR_StatusFlags(kGPIO_C, PTC16);
				GPIO_ClearISR_StatusFlags(kGPIO_C, PTC17);
			}

		}
		/*Waiting for an audio to be transfered fom the ADC to memory*/
		if(DMA_get_ISR_Flags(kDMA_ADC_MEM))
		{
			RECORDER_SaveAudio();
			DMA_clear_ISR_Flags(kDMA_ADC_MEM);
		}
		/*Waiting for the DMA to transfer an audio to the DAC*/
		if(DMA_get_ISR_Flags(kDMA_MEM_DAC))
		{
			PIT_stopxTimer(kPit_0);
			DMA_clear_ISR_Flags(kDMA_MEM_DAC);
		}

		//change the state of the machine depending on the input
		current_state = FSM_Moore[current_state].next[input];
	}
	return 0;
}
