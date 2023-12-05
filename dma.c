/*
 * dma.c
 *
 *  Created on: 15 nov 2023
 *      Author: MIKE
 */


#include "dma.h"
#include "fsl_dac.h"
#include "adc.h"

#define TCD_QUEUE_SIZE      3U
#define DMA_ALWAYS_EN_SOURCE1 (58u)
#define DMA_ALWAYS_EN_SOURCE2 (59u)
#define DMA_ALWAYS_EN_SOURCE3 (60u)
#define DMA_ADC_SOURCE (40u)
#define DAC0ADDRS DAC0
#define ADC0ADDRS (0x4003B010U)
#define SAMPLES_PER_SECOND (20000u)

#define ADC_DAC_CHANNEL (2u)
#define ADC_MEM_CHANNEL (1u)
#define MEM_DAC_CHANNEL (0u)

volatile bool g_Transfer_Done1 = false;
volatile bool g_Transfer_Done2 = false;


AT_QUICKACCESS_SECTION_DATA_ALIGN(edma_tcd_t tcdMemoryPoolPtr[TCD_QUEUE_SIZE + 1], sizeof(edma_tcd_t));

edma_handle_t g_EDMA_Handle1;
edma_handle_t g_EDMA_Handle2;
edma_handle_t g_EDMA_Handle3;
static edma_transfer_config_t transferConfig1;
static edma_transfer_config_t transferConfig2;
static edma_transfer_config_t transferConfig3;

/*!
 * EDMA_Callback1
 * Callback for the Memory to DAC DMA transfer
 * */
static void EDMA_Callback1(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
    	EDMA_ClearChannelStatusFlags(DMA0, MEM_DAC_CHANNEL, kEDMA_InterruptFlag);
    	g_Transfer_Done1 = true;
    }
}
/*!
 * EDMA_Callback2
 * Callback for the ADC to Memory DMA transfer
 * */
static void EDMA_Callback2(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
    	EDMA_ClearChannelStatusFlags(DMA0, ADC_MEM_CHANNEL, kEDMA_InterruptFlag);
    	g_Transfer_Done2 = true;
    }
}

/*!
 * EDMA_Callback3
 * Callback for the ADC to DAC DMA transfer
 * */
static void EDMA_Callback3(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    /* Clear Edma interrupt flag. */
    EDMA_ClearChannelStatusFlags(DMA0, ADC_DAC_CHANNEL, kEDMA_InterruptFlag);
    /* Setup transfer */
    EDMA_PrepareTransfer(
    		&transferConfig3,
			(void *)ADC0ADDRS,
			2u,
			DAC0ADDRS,
			2u,
			2u,
			SAMPLES_PER_SECOND,
			kEDMA_PeripheralToPeripheral);
    EDMA_SetTransferConfig(DMA0, ADC_DAC_CHANNEL, &transferConfig3, NULL);
    /* Enable transfer. */
    EDMA_StartTransfer(&g_EDMA_Handle3);
}

/*!
 * DMA_init
 * Initial setup of the DMAMUX and EDMA for channels 0,1,2
 * Channel 0 is mapped from Memory to DAC
 * Channel 1 is mapped from ADC to Memory
 * Channel 0 is mapped from ADC to DAC
 *
 * */
void DMA_init(void)
{
	edma_config_t userConfig;

	EDMA_GetDefaultConfig(&userConfig);
	EDMA_Init(DMA0, &userConfig);
	/*Configuration for the Memory to DAC EDMA */
	EDMA_CreateHandle(&g_EDMA_Handle1, DMA0, MEM_DAC_CHANNEL);
	EDMA_SetCallback(&g_EDMA_Handle1, EDMA_Callback1, NULL);
	EDMA_ResetChannel(g_EDMA_Handle1.base, g_EDMA_Handle1.channel);
	/*Configuration for the ADC to Memory EDMA */
	EDMA_CreateHandle(&g_EDMA_Handle2, DMA0, ADC_MEM_CHANNEL);
	EDMA_SetCallback(&g_EDMA_Handle2, EDMA_Callback2, NULL);
	EDMA_ResetChannel(g_EDMA_Handle2.base, g_EDMA_Handle2.channel);
	/*Configuration for the ADC to DAC EDMA */
	EDMA_CreateHandle(&g_EDMA_Handle3, DMA0, ADC_DAC_CHANNEL);
	EDMA_SetCallback(&g_EDMA_Handle3, EDMA_Callback3, NULL);
	EDMA_ResetChannel(g_EDMA_Handle3.base, g_EDMA_Handle3.channel);


	DMAMUX_Init(DMAMUX0);
	/*Configuration for the Memory to DAC DMA Mux channel*/
	DMAMUX_EnablePeriodTrigger(DMAMUX0, MEM_DAC_CHANNEL);
	DMAMUX_SetSource(DMAMUX0, MEM_DAC_CHANNEL, DMA_ALWAYS_EN_SOURCE1);
	DMAMUX_EnableChannel(DMAMUX0, MEM_DAC_CHANNEL);
	/*Configuration for the ADC to Memory DMA Mux channel*/
	DMAMUX_EnablePeriodTrigger(DMAMUX0, ADC_MEM_CHANNEL);
	DMAMUX_SetSource(DMAMUX0, ADC_MEM_CHANNEL, DMA_ALWAYS_EN_SOURCE2);
	DMAMUX_EnableChannel(DMAMUX0, ADC_MEM_CHANNEL);
	/*Configuration for the ADC to Memory DMA Mux channel*/
	DMAMUX_EnablePeriodTrigger(DMAMUX0, ADC_DAC_CHANNEL);
	DMAMUX_SetSource(DMAMUX0, ADC_DAC_CHANNEL, DMA_ALWAYS_EN_SOURCE3);
	DMAMUX_EnableChannel(DMAMUX0, ADC_DAC_CHANNEL);
	EnableIRQ(DMA2_IRQn);

}

/*!
 * DMA_ADC_DAC
 * This function is used to set the transfer configuration
 * for the DMA transfer from ADC to DAC
 * */
void DMA_ADC_DAC(void)
{
	EDMA_PrepareTransfer(
			&transferConfig3,
			(void *)ADC0ADDRS,
			2u,
			DAC0ADDRS,
			2u,
			2u,
			SAMPLES_PER_SECOND,
			kEDMA_PeripheralToPeripheral);
	EDMA_SubmitTransfer(&g_EDMA_Handle3, &transferConfig3);

	EDMA_EnableChannelInterrupts(DMA0, ADC_DAC_CHANNEL, kEDMA_MajorInterruptEnable);
	//EDMA_EnableChannelRequest(DMA0, ADC_DAC_CHANNEL);

	EDMA_StartTransfer(&g_EDMA_Handle3);
	EDMA_TriggerChannelStart(DMA0, ADC_DAC_CHANNEL);
}

/*!
 * DMA_MEM_DAC
 * This function is used to set the transfer configuration
 * for the DMA transfer from Memory to DAC
 * */
void DMA_MEM_DAC(uint16_t * sendDataAddrs, uint32_t dataLen)
{
	EDMA_PrepareTransfer(
			&transferConfig1,
			sendDataAddrs,
			2u,
			DAC0ADDRS,
			2u,
			2u,
			SAMPLES_PER_SECOND,
			kEDMA_MemoryToPeripheral);
	EDMA_SubmitTransfer(&g_EDMA_Handle1, &transferConfig1);

	EDMA_EnableChannelInterrupts(DMA0, MEM_DAC_CHANNEL, kEDMA_MajorInterruptEnable);

	EDMA_StartTransfer(&g_EDMA_Handle1);
	EDMA_TriggerChannelStart(DMA0, MEM_DAC_CHANNEL);
}

/*!
 * DMA_ADC_MEM
 * This function is used to set the transfer configuration
 * for the DMA transfer from ADC to DAC
 * */
void DMA_ADC_MEM(uint16_t * receiveDataAddrs, uint32_t dataLen)
{
	EDMA_PrepareTransfer(
			&transferConfig2,
			(void *)ADC0ADDRS,
			2u,
			receiveDataAddrs,
			2u,
			2u,
			SAMPLES_PER_SECOND,
			kEDMA_PeripheralToMemory);
	EDMA_SubmitTransfer(&g_EDMA_Handle2, &transferConfig2);

	EDMA_EnableChannelInterrupts(DMA0, ADC_MEM_CHANNEL, kEDMA_MajorInterruptEnable);

	EDMA_StartTransfer(&g_EDMA_Handle2);
	EDMA_TriggerChannelStart(DMA0, ADC_MEM_CHANNEL);

}

/*!
 * DMA_StopTransfer
 * This function is used to stop transfers from the 3 channels
 * */
void DMA_StopTransfer(void)
{
	EDMA_AbortTransfer(&g_EDMA_Handle3);
	EDMA_AbortTransfer(&g_EDMA_Handle2);
	EDMA_AbortTransfer(&g_EDMA_Handle1);
}

/*!
 * DMA_get_ISR_Flags
 * This function is used to get the status of the dma transfer done flags
 *
 * @retval 1 if transfer is done, and 0 if transfer is not done
 *
 * @param channel The DMA channel ISR flag to get
 * */
uint8_t DMA_get_ISR_Flags(DMA_ISRFlags_t channel)
{
	uint8_t retval = 0u;
	switch(channel)
	{
	case kDMA_ADC_MEM:
		retval = g_Transfer_Done2;
	break;
	case kDMA_MEM_DAC:
		retval = g_Transfer_Done1;
	break;
	default:
	break;
	}
	return retval;
}

/*!
 * DMA_clear_ISR_Flags
 * This function is used to clear the status of the dma transfer done flags
 *
 * @param channel The DMA channel ISR flag to clear
 * */
void DMA_clear_ISR_Flags(DMA_ISRFlags_t channel)
{
	switch(channel)
	{
	case kDMA_ADC_MEM:
		g_Transfer_Done2 = false;
	break;
	case kDMA_MEM_DAC:
		g_Transfer_Done1 = false;
	break;
	default:
	break;
	}
}
