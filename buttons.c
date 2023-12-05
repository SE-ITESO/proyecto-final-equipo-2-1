/*
 * buttons.c
 *
 *  Created on: 27 sep 2023
 *      Author: MIKE
 */

#include "buttons.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

typedef enum{
	kBUTTONS_Menu = 0u,
	kBUTTONS_B1 = 1u,
	kBUTTONS_B2 = 2u
}BUTTONS_Menu_t;


#define MENU 1u
#define B1 16u
#define B2 17u


static const port_pin_config_t portSWConf = {
		kPORT_PullUp,
		kPORT_FastSlewRate,
		kPORT_PassiveFilterEnable,
		kPORT_OpenDrainDisable,
		kPORT_HighDriveStrength,
		kPORT_MuxAsGpio,
		kPORT_UnlockRegister
};

static const gpio_pin_config_t gpio_SWConf = {
		kGPIO_DigitalInput,
		1
};

/*!
 * BUTTONS_init
 * Initialization of the ports tha involve buttons
 * */
void BUTTONS_init(void)
{
	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortC);

	PORT_SetPinConfig(PORTA, MENU, &portSWConf);	//Menu Btn
	GPIO_PinInit(GPIOA, MENU, &gpio_SWConf);
	PORT_SetPinConfig(PORTC, B1, &portSWConf);		//B1 Btn
	GPIO_PinInit(GPIOC, B1, &gpio_SWConf);
	PORT_SetPinConfig(PORTC, B2, &portSWConf);		//B2 Btn
	GPIO_PinInit(GPIOC, B2, &gpio_SWConf);
}


/*!
 * BUTTONS_interrupt_enable
 * Interrupt enable of all buttons
 * */
void BUTTONS_interrupt_enable(void)
{
	//Interrupt config for the menu buttons
	PORT_SetPinInterruptConfig(PORTA, MENU, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTC, B1, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTC, B2, kPORT_InterruptFallingEdge);

	EnableIRQ(PORTA_IRQn);
	EnableIRQ(PORTC_IRQn);
	EnableIRQ(PORTA_IRQn);
}

