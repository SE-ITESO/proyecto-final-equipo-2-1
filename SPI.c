
#include "SPI.h"
#include "fsl_dspi.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "clock_config.h"

void SPI_config(void)
{
	uint32_t srcClock_hz;

	dspi_master_config_t masterConfig;
	srcClock_hz = CLOCK_GetFreq(DSPI0_CLK_SRC);

	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);

	PORT_SetPinMux(PORTD, PIN0_IDX , kPORT_MuxAlt2);	/*SPI0 PCS0*/
	PORT_SetPinMux(PORTD, PIN1_IDX , kPORT_MuxAlt2);	/*SPI0 SCK*/
	PORT_SetPinMux(PORTD, PIN2_IDX , kPORT_MuxAlt2);	/*SPI0 MOSI*/
	PORT_SetPinMux(PORTD, PIN3_IDX , kPORT_MuxAlt2);	/*SPI0 MISO*/
	PORT_SetPinMux(PORTC, PIN3_IDX , kPORT_MuxAlt2);	/*SPI0 PCS1*/

	//Configuration for the SPI LCD
	masterConfig.whichCtar                                = kDSPI_Ctar0;
	masterConfig.ctarConfig.baudRate                      = LCD_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame                  = 8u;
	masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveHigh;
	masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseFirstEdge;
	masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 0u;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 0u;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 0u;

	masterConfig.whichPcs           = kDSPI_Pcs0;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
	
	masterConfig.enableContinuousSCK        = false;
	masterConfig.enableRxFifoOverWrite      = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

	DSPI_MasterInit(SPI0, &masterConfig, srcClock_hz);
	//Configuration for the spi memory

	masterConfig.whichCtar                                = kDSPI_Ctar1;
	masterConfig.ctarConfig.baudRate                      = MEMORY_BAUDRATE;
	masterConfig.ctarConfig.bitsPerFrame                  = 8u;
	masterConfig.ctarConfig.cpol                          = kDSPI_ClockPolarityActiveLow;
	masterConfig.ctarConfig.cpha                          = kDSPI_ClockPhaseSecondEdge;
	masterConfig.ctarConfig.direction                     = kDSPI_MsbFirst;
	masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = 0u;
	masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = 0u;
	masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 0u;

	masterConfig.whichPcs           = kDSPI_Pcs1;
	masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig.enableContinuousSCK        = false;
	masterConfig.enableRxFifoOverWrite      = false;
	masterConfig.enableModifiedTimingFormat = false;
	masterConfig.samplePoint                = kDSPI_SckToSin0Clock;

	DSPI_MasterInit(SPI0, &masterConfig, srcClock_hz);
}
