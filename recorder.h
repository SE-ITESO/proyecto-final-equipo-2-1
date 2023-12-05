/*
 * recorder.h
 *
 *  Created on: 1 dic. 2023
 *      Author: diego
 */

#ifndef RECORDER_H_
#define RECORDER_H_

#include "display.h"

#define MSG_LENG 90000u
#define BTN0 (1u)
#define BTN1 (2u)
#define BTN2 (3u)

typedef enum{
	kRECORDER_Msg1_Addrs = 0x000000u,
	kRECORDER_Msg2_Addrs = 0x030000u
}Recorder_msg_address_t;

typedef enum {
	kRECORDER_Msg1 = 0u,
	kRECORDER_Msg2 = 1u
}Recorder_msg_t;

typedef enum {
	kRECORDER_Adc = 0u,
	kRECORDER_Dac = 1u
}Recorder_transfer_t;



void RECORDER_RecordAudio(void);
void RECORDER_PlayMsg(Recorder_msg_t sel);
void RECORDER_SaveAudio(void);
Menu_t RECORDER_mode(Menu_t Sel, uint8_t btn);

#endif /* RECORDER_H_ */
