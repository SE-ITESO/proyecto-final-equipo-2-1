/*
 * buttons.h
 *
 *  Created on: 27 sep 2023
 *      Author: MIKE
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "fsl_port.h"
#include "fsl_gpio.h"

/*
 * B0  A1
 * B1  D3
 * B2  C2
 * */


void BUTTONS_init(void);
void BUTTONS_interrupt_enable(void);



#endif /* BUTTONS_H_ */
