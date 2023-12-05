/*
 * system_clock.h
 *
 *  Created on: 22 nov. 2023
 *      Author: diego
 */

#ifndef SYSTEM_CLOCK_H_
#define SYSTEM_CLOCK_H_

#define OSC_CAP0P 0u
#define OSC_FREQ 50000000u
#define MCG_PLL_DISABLE 0u
#define PRDIV 0xeu
#define VDIV 0xCu


void SYSTEM_CLOCK_setup();

#endif /* SYSTEM_CLOCK_H_ */
