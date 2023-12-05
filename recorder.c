/*
 * recorder.c
 *
 *  Created on: 1 dic. 2023
 *      Author: diego
 */

#include "pit.h"
#include "recorder.h"
#include "S25FL164K.h"
#include "dma.h"

#define RECORDED_SAMPLES (90000u)

typedef enum{
	False = 0u,
	True = 1u
}Bool_t;

typedef enum{
	Microphone_mode = 0u,
	Record_mode = 1u,
	Play_mode =2u
}Mode_t;

static uint8_t flag1 = False;
static uint8_t flag2 = False;

//Array that contains the audio samples
static uint16_t msg [RECORDED_SAMPLES];

static Recorder_msg_t g_msg_sel = kRECORDER_Msg1;

/*!
 * RECORDER_RecordAudio
 * This function is used to set de DMA to transfer data from the ADC
 * to store it into data memory
 *
 * */
void RECORDER_RecordAudio(void)
{
	DMA_ADC_MEM(msg, (RECORDED_SAMPLES*2));
	PIT_startxTimer(kPit_1, PERIOD_10kHz);
}

/*!
 * RECORDER_MicrophoneMode
 * This function is used to set the microphone working mode of the system
 * */
void RECORDER_MicrophoneMode(void)
{
	DMA_ADC_DAC();
	PIT_startxTimer(kPit_2, PERIOD_20kHz);
}

/*!
 * RECORDER_PlayMsg
 * This function is used to play an audio, if the audio is already in
 * data memory, it just plays it, otherwise it loads the audio from the
 * SPI memory and then plays the audio
 *
 * @param sel The audio to play
 * */
void RECORDER_PlayMsg(Recorder_msg_t sel)
{
	DMA_MEM_DAC(msg, (RECORDED_SAMPLES*2));
	switch (sel)
	{
	case kRECORDER_Msg1:
		if (flag1)
		{
			PIT_startxTimer(kPit_0, PERIOD_10kHz);
		}
		else
		{
			MEMORY_Read(msg, kRECORDER_Msg1_Addrs, MSG_LENG);
			PIT_startxTimer(kPit_0, PERIOD_10kHz);
			flag1 = True;
			flag2 = False;
		}
	break;
	case kRECORDER_Msg2:
		if (flag2)
		{
			PIT_startxTimer(kPit_0, PERIOD_10kHz);
		}
		else
		{
			MEMORY_Read(msg, kRECORDER_Msg2_Addrs,MSG_LENG);
			PIT_startxTimer(kPit_0, PERIOD_10kHz);
			flag2 = True;
			flag1 = False;
		}
	break;
	}
}

/*!
 * RECORDER_SaveAudio
 * When the Dma has ended the transfer between ADC and memory,
 * this function is called to store the audio into the SPI memory
 * */
void RECORDER_SaveAudio(void)
{
	PIT_stopxTimer(kPit_1);
	switch(g_msg_sel)
	{
	case kRECORDER_Msg1:
		flag1 = True;
		flag2 = False;
		MEMORY_Write(msg, kRECORDER_Msg1_Addrs, RECORDED_SAMPLES);
		DISPLAY_msg_recorded();
	break;
	case kRECORDER_Msg2:
		flag2 = True;
		flag1 = False;
		MEMORY_Write(msg, kRECORDER_Msg2_Addrs, RECORDED_SAMPLES);
		DISPLAY_msg_recorded();
	break;
	}
}

/*!
 * RECORDER_Stop
 * This is an auxiliary function used to stop the microphone working mode
 * */
static void RECORDER_Stop()
{
	PIT_stopxTimer(kPit_2);
	DMA_StopTransfer();
}

/*!
 * RECORDER_mode
 * This function is used to know the current state of the State machine
 * and perform an action when a specific button has been depressed
 *
 * @retval The new state of the state machine
 *
 * @param Sel The current state of the State machine
 * @param btn The button that was depressed
 * */
Menu_t RECORDER_mode(Menu_t Sel, uint8_t btn)
{
	Menu_t retval = 0u;
	switch(Sel)
	{
	case kDisplay_M0:
		DISPLAY_MenuSelec(kDisplay_M0);
		if(BTN0 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_MRealT);
			retval = kDisplay_MRealT;
			RECORDER_MicrophoneMode();
		}
		else if(BTN1 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_MRecord);
			retval = kDisplay_MRecord;
		}
		else if(BTN2 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_MPlay);
			retval = kDisplay_MPlay;
		}
	break;
	case kDisplay_MRealT:
		DISPLAY_MenuSelec(kDisplay_MRealT);
		if(BTN0 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_M0);
			retval = kDisplay_M0;
			RECORDER_Stop();
		}
		else if(BTN1 == btn)
		{
			retval = kDisplay_MRealT;
		}
		else if(BTN2 == btn)
		{
			retval = kDisplay_MRealT;
		}
	break;
	case kDisplay_MPlay:
		DISPLAY_MenuSelec(kDisplay_MPlay);
		if(BTN0 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_M0);
			retval = kDisplay_M0;
		}
		else if(BTN1 == btn)
		{
			g_msg_sel = kRECORDER_Msg1;
			RECORDER_PlayMsg(g_msg_sel);
			retval = kDisplay_MPlay;
		}
		else if(BTN2 == btn)
		{
			g_msg_sel = kRECORDER_Msg2;
			RECORDER_PlayMsg(g_msg_sel);
			retval = kDisplay_MPlay;
		}
	break;
	case kDisplay_MRecord:
		DISPLAY_MenuSelec(kDisplay_MRecord);
		if(BTN0 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_M0);
			retval = kDisplay_M0;
		}
		else if(BTN1 == btn)
		{
			g_msg_sel = kRECORDER_Msg1;
			DISPLAY_MenuSelec(kDisplay_MRecording);
			retval = kDisplay_MRecording;
		}
		else if(BTN2 == btn)
		{
			g_msg_sel = kRECORDER_Msg2;
			DISPLAY_MenuSelec(kDisplay_MRecording);
			retval = kDisplay_MRecording;
		}
	break;
	case kDisplay_MRecording:
		DISPLAY_MenuSelec(kDisplay_MRecording);
		if(BTN0 == btn)
		{
			DISPLAY_MenuSelec(kDisplay_MRecord);
			retval = kDisplay_MRecord;
		}
		else if(BTN1 == btn)
		{
			retval = kDisplay_MRecording;
		}
		else if(BTN2 == btn)
		{
			RECORDER_RecordAudio();
			retval = kDisplay_MRecording;
		}
	break;
	default:
	break;
	}
	return retval;
}
