/*
 * display.c
 *
 *  Created on: 16 nov. 2023
 *      Author: diego
 */

#include "display.h"
#include "LCD_nokia.h"


static uint8_t M0_op1 [] = "1-Microphone";
static uint8_t M0_op2 [] = "2-Record";
static uint8_t M0_op3 [] = "3-Play";

static uint8_t MManual_msg1 [] = "Microphone  mode";
static uint8_t Playing_msg [] = "Press b2 to start record";

static uint8_t Msg_Recorded[] = "Message     Recorded!";


static uint8_t RecordMode_Msg [] = "Record Mode";
static uint8_t Record_msg1 [] = "msg1";
static uint8_t Record_msg2 [] = "msg2";

static uint8_t PlayMode_msg []= "Play Mode";
static uint8_t PlayMode_msg1 []= "msg1";
static uint8_t PlayMode_msg2 []= "msg2";


void DISPLAY_MenuSelec(Menu_t MenuSel)
{
	/* Funcion que nos permite mostrar el menu que necesitamos dependiendo de l selector que le mandemos*/
	switch(MenuSel)
	{
	case kDisplay_M0:
		DISPLAY_Menu0();
	break;
	case kDisplay_MRealT:
		DISPLAY_Menu_RealT();
	break;
	case kDisplay_MPlay:
		DISPLAY_PlayMode();
	break;
	case kDisplay_MRecord:
		DISPLAY_RecordMode();
	break;
	case kDisplay_MRecording:
		DISPLAY_Recording_msg();
	}
}

/*!
 * DISPLAY_Menu0
 * Displays the Initial Menu in the LCD
 * */
void DISPLAY_Menu0(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 2);
	LCD_nokia_send_string(M0_op1);
	LCD_nokia_goto_xy(0, 3);
	LCD_nokia_send_string(M0_op2);
	LCD_nokia_goto_xy(0, 4);
	LCD_nokia_send_string(M0_op3);
}

/*!
 * DISPLAY_Menu_RealT
 * Displays the Microphone Menu in the LCD
 * */
void DISPLAY_Menu_RealT(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 0);
	LCD_nokia_send_string(MManual_msg1);
}

/*!
 * DISPLAY_Recording_msg
 * Displays the Recording Menu in the LCD
 * */
void DISPLAY_Recording_msg(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 1);
	LCD_nokia_send_string(Playing_msg);
}

/*!
 * DISPLAY_msg_recorded
 * Displays the Message recorded message in the LCD
 * */
void DISPLAY_msg_recorded(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 3);
	LCD_nokia_send_string(Msg_Recorded);
}

/*!
 * DISPLAY_RecordMode
 * Displays the Record Menu in the LCD
 * */
void DISPLAY_RecordMode(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 0);
	LCD_nokia_send_string(RecordMode_Msg);
	LCD_nokia_goto_xy(0, 2);
	LCD_nokia_send_string(Record_msg1);
	LCD_nokia_goto_xy(0, 4);
	LCD_nokia_send_string(Record_msg2);

}

/*!
 * DISPLAY_PlayMode
 * Displays the Play Menu in the LCD
 * */
void DISPLAY_PlayMode(void)
{
	LCD_nokia_clear();
	LCD_nokia_goto_xy(0, 0);
	LCD_nokia_send_string(PlayMode_msg);
	LCD_nokia_goto_xy(0, 2);
	LCD_nokia_send_string(PlayMode_msg1);
	LCD_nokia_goto_xy(0, 4);
	LCD_nokia_send_string(PlayMode_msg2);
}

