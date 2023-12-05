/*
 * S25FL164K.h
 *
 *  Created on: 12 nov 2023
 *      Author: MIKE
 */

#ifndef S25FL164K_H_
#define S25FL164K_H_

#include <stdint.h>

void MEMORY_Read(uint16_t* dataArr, uint32_t readAddrs,uint32_t dataLen);

void MEMORY_Write(uint16_t* dataArr,uint32_t writeAddrs, uint32_t dataLen);

uint8_t MEMORY_checkBusy(void);

void MEMORY_ReadSR(void);


#endif /* S25FL164K_H_ */
