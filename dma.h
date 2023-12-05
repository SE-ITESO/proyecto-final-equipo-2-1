/*
 * dma.h
 *
 *  Created on: 15 nov 2023
 *      Author: MIKE
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include "fsl_dmamux.h"
#include "fsl_edma.h"

typedef enum{
	kDMA_ADC_MEM = 0u,
	kDMA_MEM_DAC = 1u,
}DMA_ISRFlags_t;

void DMA_init(void);
void DMA_ADC_DAC(void);
void DMA_MEM_DAC(uint16_t * sendDataAddrs, uint32_t dataLen);
void DMA_ADC_MEM(uint16_t * receiveDataAddrs, uint32_t dataLen);
uint8_t DMA_get_ISR_Flags(DMA_ISRFlags_t channel);
void DMA_clear_ISR_Flags(DMA_ISRFlags_t channel);
void DMA_StopTransfer(void);

#endif /* DMA_H_ */
