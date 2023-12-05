/*
 * pit.h
 *
 *  Created on: 4 oct 2023
 *      Author: MIKE
 */

#ifndef PIT_H_
#define PIT_H_

#include <stdint.h>

typedef enum {
	kPit_0 = 0u,
	kPit_1 = 1u,
	kPit_2 = 2u
}PIT_CHNL_t;

#define PERIOD_10kHz 100u
#define PERIOD_20kHz 50u

void PIT_init(void);
void PIT_startxTimer(PIT_CHNL_t chnl, uint32_t delayUs);
void PIT_stopxTimer(PIT_CHNL_t chnl);
void PIT_setISRFlags(PIT_CHNL_t pitChnl);
void PIT_clearISRFlags(PIT_CHNL_t pitChnl);
uint8_t PIT_getISRFlags(PIT_CHNL_t pitChnl);

#endif /* PIT_H_ */
