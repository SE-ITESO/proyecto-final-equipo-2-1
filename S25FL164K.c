/*
 * S25FL164K.c
 *
 *  Created on: 12 nov 2023
 *      Author: MIKE
 */

#include "S25FL164K.h"
#include "SPI.h"
#include "stdint.h"
#include "fsl_dspi.h"

#define SECOND_BYTE (1U)
#define MAX_BYTES_PER_PAGE (256U)
#define PAGE_OFFSET (128U)
#define ONE_BYTE_MASK (0xFFU)
#define UPPER_BYTE_SHIFT (8U)
#define BLOCK_SIZE (0x10000) //64kB block size

#define READ_SR1_COMMAND (0x05)
#define READ_SR2_COMMAND (0x35)
#define READ_SR3_COMMAND (0x33)
#define BUSY_BIT_MASK (0x01)
#define WRITE_ENABLE_COMMAND (0x06)
#define PAGE_PROGRAM_COMMAND (0x02)
#define BLOCK_ERASE_COMMAND (0xD8)
#define READ_DATA_COMMAND (0x03)

static uint8_t auxarr[256];		//Auxiliary array to write in the memory

/*!
 * MEMORY_Read
 * Function to read the spi memory one byte at a time
 *
 * @param *dataAddrs Pointer to the address where read data will be stored
 * @param readAddrs Value of the address to be read in the memory
 * @param dataLen Length of the read data, each 16 bits equals 1 in length
 * */
void MEMORY_Read(uint16_t* dataArr, uint32_t readAddrs,uint32_t dataLen)
{
	uint8_t addrsArr[4];
	addrsArr[0] = READ_DATA_COMMAND;	//Read command for the memory
	addrsArr[1] = (readAddrs >> 16u) & 0xFF;
	addrsArr[2] = (readAddrs >> 8u) & 0xFF;
	addrsArr[3] = readAddrs & 0xFF;
	dataLen *= 2;

	dspi_half_duplex_transfer_t xfer;
	xfer.configFlags = kDSPI_MasterPcsContinuous | kDSPI_MasterCtar1 | kDSPI_MasterPcs1;
	xfer.isPcsAssertInTransfer = true;
	xfer.rxData = (uint8_t*)dataArr;
	xfer.rxDataSize = dataLen;
	xfer.txData = addrsArr;
	xfer.txDataSize = 4u;
	xfer.isTransmitFirst = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);
}

/*!
 * Fill_Arr
 * Auxiliary function to fill the 256byte array that will be written in memory
 *
 * @param offset The page offset to write in a new page
 * @param xferbytes The number of bytes to store in the array if less than 256B
 * @param *data Pointer to the array containing the data to be written
 * @param *array Pointer to the array containing the data to be sent
 * */
static void Fill_Arr(uint16_t offset,uint16_t xferBytes, uint16_t* data, uint8_t* array)
{
	uint16_t index = PAGE_OFFSET * offset;
	uint16_t index2 = 0;
	uint8_t auxByte = 0u;
	uint16_t* auxPtr = data;
	auxPtr += index;
	for(uint16_t i=xferBytes; i>0;i-=2)
	{
		auxByte = (uint8_t) *(auxPtr);
		array[index2] = auxByte;
		auxByte = (uint8_t) (*(auxPtr) >> UPPER_BYTE_SHIFT);
		array[index2 + 1] = auxByte;
		auxPtr++;
		index2+=2;
	}
}

/*!
 * MEMORY_BlockErase
 * Auxiliary function to erase memory blocks based on the length of the
 *
 * @param offset The page offset to write in a new page
 * @param xferbytes The number of bytes to store in the array if less than 256B
 * @param *data Pointer to the array containing the data to be written
 * @param *array Pointer to the array containing the data to be sent
 * */
static void MEMORY_BlockErase(uint32_t writeAddrs, uint32_t dataLen)
{
	uint8_t addrsArr[4];
	uint8_t write_En = WRITE_ENABLE_COMMAND;
	addrsArr[0]= BLOCK_ERASE_COMMAND;
	addrsArr[1] = (writeAddrs >> 16u) & 0xFF;
	addrsArr[2] = (writeAddrs) & 0xFF;
	addrsArr[3] = writeAddrs & 0xFF;
	uint16_t eraseBlocks = 0u;
	uint8_t data = 0u;
	dataLen*=2; //data length is multiplied by 2 due to data being 16-bit

	dspi_half_duplex_transfer_t xfer;
	xfer.configFlags = kDSPI_MasterPcsContinuous | kDSPI_MasterCtar1 | kDSPI_MasterPcs1;
	xfer.isTransmitFirst = true;

	if((BLOCK_SIZE >= dataLen))
	{
		eraseBlocks = 1u;
	}
	else
	{
		eraseBlocks = (dataLen / BLOCK_SIZE) + 1u;
	}

	for(uint16_t i=eraseBlocks; i>0;i--)
	{
		/*****************Sending the Write enable command prior to erasing the block******************/
		xfer.rxData = &data;
		xfer.rxDataSize = 0u;
		xfer.txData = &write_En;
		xfer.txDataSize = 1u;
		xfer.isPcsAssertInTransfer = false;
		DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);
		/******************Sending the memory address that will be erased**********************/
		addrsArr[1] = (writeAddrs >> 16u) & 0xFF;
		addrsArr[2] = (writeAddrs) & 0xFF;
		addrsArr[3] = writeAddrs & 0xFF;
		xfer.rxData = &data;
		xfer.rxDataSize = 0u;
		xfer.txData = addrsArr;
		xfer.txDataSize = 4u;
		xfer.isPcsAssertInTransfer = false;
		DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);

		writeAddrs += BLOCK_SIZE;
		while(MEMORY_checkBusy())
		{
		}
	}
}


/*!
 * MEMORY_Write
 * Function to write the SPI memory one page (256 bytes) each
 * write
 *
 * @param *dataArr Pointer to the address that contains the data
 * 			that will be stored
 * @param readAddrs Value of the address to be written in the memory
 * @param dataLen Length of the write data, each 16 bits equals 1 in length
 * */
void MEMORY_Write(uint16_t* dataArr,uint32_t writeAddrs, uint32_t dataLen)
{
	uint8_t addrsArr[4];
	addrsArr[0]= 0x02;			//Page program command for the memory
	uint8_t write_En = WRITE_ENABLE_COMMAND;	//Write enable command for the memory
	uint8_t data = 0u;			//Dummy data when no received data is needed
	uint16_t xfernum = 0u;		//The number of transfers to be done
	uint16_t xfercount = 0u;	//The trasfer count
	uint16_t xferSize = 256u;	//Number of bytes to transfer, normally 256 bytes but the last transfer could be less
	uint32_t bytesCount = 0u;	//Number of current transfered bytes
	dataLen *= 2;

	dspi_half_duplex_transfer_t xfer;
	xfer.configFlags = kDSPI_MasterPcsContinuous | kDSPI_MasterCtar1 | kDSPI_MasterPcs1;
	xfer.isTransmitFirst = true;

	xfernum = (dataLen / MAX_BYTES_PER_PAGE) + 1u;

	MEMORY_BlockErase(writeAddrs, dataLen);

	for(uint16_t i=xfernum; i>0; i--)
	{
		if((dataLen - bytesCount) < MAX_BYTES_PER_PAGE)
		{
			xferSize = dataLen - bytesCount;
		}
		else
		{
			xferSize = MAX_BYTES_PER_PAGE;

		}

		Fill_Arr(xfercount,xferSize, dataArr, auxarr);
		/*****************Sending the Write enable command prior to writing to the memory******************/
		xfer.rxData = &data;
		xfer.rxDataSize = 0u;
		xfer.txData = &write_En;
		xfer.txDataSize = 1u;
		xfer.isPcsAssertInTransfer = false;
		DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);
		/******************Sending the memory address where data will be written***************************/
		xfer.rxData = &data;
		xfer.rxDataSize = 0u;
		xfer.txData = addrsArr;
		xfer.txDataSize = 4u;
		xfer.isPcsAssertInTransfer = true;
		DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);
		/******************Sending the data to the memory***************************/
		xfer.rxData = &data;
		xfer.rxDataSize = 0u;
		xfer.txData = auxarr;
		xfer.txDataSize = xferSize;
		xfer.isPcsAssertInTransfer = false;
		DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);

		bytesCount += 256;
		xfercount++;
		writeAddrs+=MAX_BYTES_PER_PAGE;
		addrsArr[1] = (writeAddrs >> 16u) & 0xFF;
		addrsArr[2] = (writeAddrs >> 8u) & 0xFF;
		addrsArr[3] = writeAddrs & 0xFF;

		while(MEMORY_checkBusy())
		{
		}
	}

}

/*!
 * MEMORY_checkBusy
 *
 * Performs a reading of the Status Register 1 of the memory
 * and returns a 1 if the status busy bit is active, meaning that the memory is busy
 *
 * @retval status of the busy bit in the Status register 1
 *
 * */
uint8_t MEMORY_checkBusy(void)
{
	uint8_t readSR1 = READ_SR1_COMMAND;
	uint8_t data = 0u;
	uint8_t Mask = 0u;
	uint8_t retval = 0u;

	dspi_half_duplex_transfer_t xfer;
	xfer.configFlags = kDSPI_MasterPcsContinuous | kDSPI_MasterCtar1 | kDSPI_MasterPcs1;
	xfer.isTransmitFirst = true;

	xfer.rxData = &data;
	xfer.rxDataSize = 1u;
	xfer.txData = &readSR1;
	xfer.txDataSize = 1u;
	xfer.isPcsAssertInTransfer = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);

	Mask = data && BUSY_BIT_MASK;

	if(Mask)
	{
		retval = 1u;
	}

	return retval;
}

/*!
 * MEMORY_ReadSR
 * This function is used to read the memory Status Registers
 * to know their current values
 * */
void MEMORY_ReadSR(void)
{
	uint8_t readSR1 = READ_SR1_COMMAND;
	uint8_t readSR2 = READ_SR2_COMMAND;
	uint8_t readSR3 = READ_SR3_COMMAND;
	uint8_t data = 0u;

	dspi_half_duplex_transfer_t xfer;
	xfer.configFlags = kDSPI_MasterPcsContinuous | kDSPI_MasterCtar1 | kDSPI_MasterPcs1;
	xfer.isTransmitFirst = true;

	xfer.rxData = &data;
	xfer.rxDataSize = 1u;
	xfer.txData = &readSR1;
	xfer.txDataSize = 1u;
	xfer.isPcsAssertInTransfer = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);

	xfer.rxData = &data;
	xfer.rxDataSize = 1u;
	xfer.txData = &readSR2;
	xfer.txDataSize = 1u;
	xfer.isPcsAssertInTransfer = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);

	xfer.rxData = &data;
	xfer.rxDataSize = 1u;
	xfer.txData = &readSR3;
	xfer.txDataSize = 1u;
	xfer.isPcsAssertInTransfer = true;
	DSPI_MasterHalfDuplexTransferBlocking(SPI0, &xfer);
}
