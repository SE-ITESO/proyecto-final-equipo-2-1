/*
 * gpio.c
 *
 *  Created on: 27 sep 2023
 *      Author: MIKE
 */

#include "gpio.h"
#include "fsl_gpio.h"

/*
 * Port A,B,C interrupt flags
 * */
static GPIO_x_ISR_flags g_A_intr_ISR_flag = {0};
static GPIO_x_ISR_flags g_C_intr_ISR_flag = {0};

typedef enum {
	False = 0u,
	True = 1u
}bool_t;

/*!
<<<<<<< HEAD
=======
 * GPIO_CallbackInit
 * Callback for GPIOB and GPIOC
 * */
void GPIO_CallbackInit(void (*gpio_Handler)(uint8_t))
{
}

/*!
>>>>>>> Display
 * GPIO_ISRFlagsDecode
 * Decoding of the interrupt flags of a specific port
 * and put them in a single variable
 *
 * @param * g_ISR_flag (g_A_intr_ISR_flag, g_B_intr_ISR_flag, etc.)
 * */
static inline uint8_t GPIO_ISRFlagsDecode(GPIO_x_ISR_flags * g_ISR_flag)
{
	uint8_t flags = 0u;
	flags |= g_ISR_flag->flag_0;
	flags |= (g_ISR_flag->flag_1) << 1u;
	flags |= (g_ISR_flag->flag_2) << 2u;
	flags |= (g_ISR_flag->flag_3) << 3u;
	flags |= (g_ISR_flag->flag_4) << 4u;
	flags |= (g_ISR_flag->flag_5) << 5u;
	flags |= (g_ISR_flag->flag_6) << 6u;
	flags |= (g_ISR_flag->flag_7) << 7u;
	return flags;
}

/*!
 * GPIO_GetISR_StatusFlags
 * Gets the status flags of a specific port
 * defaults flags to 0
 * @param Port (kGPIO_A, kGPIO_B, kGPIO_C)
 * */
uint8_t GPIO_GetISR_StatusFlags(GPIO_Port_t Port)
{
	uint8_t flags = 0u;
	switch(Port)
	{
	case kGPIO_A:
		flags = GPIO_ISRFlagsDecode(&g_A_intr_ISR_flag);
		break;
	case kGPIO_B:
		break;
	case kGPIO_C:
		flags = GPIO_ISRFlagsDecode(&g_C_intr_ISR_flag);
		break;
	case kGPIO_D:
		break;
	default:
		flags = 0u;
	}
	return flags;
}

/*!
 * GPIO_SetISR_StatusFlags
 * Sets the value of the interrput flags of a specific port
 *
 * @param mask (PTA1, PTC16, PTC17)
 * @param * g_ISR_flag (g_A_intr_ISR_flag, g_B_intr_ISR_flag, etc.)
 * */
void GPIO_SetISR_StatusFlags(uint32_t mask, GPIO_x_ISR_flags * g_ISR_flag)
{
	if(PTC16 == mask)
	{
		g_ISR_flag->flag_0 = True;
	}
	else if(PTC17 == mask)
	{
		g_ISR_flag->flag_1 = True;
	}
	else if(PTA1 == mask)
	{
		g_ISR_flag->flag_0 = True;
	}
}

/*!
 * GPIO_ClearISR_StatusFlags
 * Clears the interrupt flags of a specific port
 *
 * @param Port (kGPIO_A, kGPIO_B, kGPIO_C)
 * @param mask (PTBC10, PTC11, PTC6 , etc.)
 **/
void GPIO_ClearISR_StatusFlags(GPIO_Port_t Port, uint32_t mask)
{
	switch(Port)
	{
	case kGPIO_A:
		if(PTA1 == mask)
		{
			g_A_intr_ISR_flag.flag_0 = False;
		}
	break;
	case kGPIO_B:
	break;
	case kGPIO_C:
		if(PTC16 == mask)
		{
			g_C_intr_ISR_flag.flag_0 = False;
		}
		else
		{
			g_C_intr_ISR_flag.flag_1 = False;
		}
	break;
	case kGPIO_D:
	break;
	}
}


/*!
 * PORTA_IRQHandler
 * Interrupt Service routine for port A
 * */
void PORTA_IRQHandler(void)
{
	uint32_t Mask = 0;
	Mask = GPIO_PortGetInterruptFlags(GPIOA);
	GPIO_SetISR_StatusFlags(Mask, &g_A_intr_ISR_flag);
	GPIO_PortClearInterruptFlags(GPIOA, 0xFFFFFFFF);
}

/*!
 * PORTC_IRQHandler
 * Interrupt Service routine for port C
 * */
void PORTC_IRQHandler(void)
{
	uint32_t Mask = 0;
	Mask = GPIO_PortGetInterruptFlags(GPIOC);
	GPIO_SetISR_StatusFlags(Mask, &g_C_intr_ISR_flag);
	GPIO_PortClearInterruptFlags(GPIOC, 0xFFFFFFFF);
}

