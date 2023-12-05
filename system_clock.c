/*
 * system_clock.c
 *
 *  Created on: 22 nov. 2023
 *      Author: diego
 */

#include "system_clock.h"
#include "fsl_clock.h"

static const osc_config_t osc_config =
{
		.freq = OSC_FREQ,
		.capLoad = (OSC_CAP0P), /* oscillator capacity load: 0pF*/
		.workMode = kOSC_ModeExt, /*Use external clock*/
		.oscerConfig = {
				.enableMode = kOSC_ErClkEnable, /*Enable external reference clock. disable externa reference clock stop mode*/
		}
};

static mcg_pll_config_t pll0Config  =
{
		.enableMode = MCG_PLL_DISABLE, /*MCGPLLCLK disable*/
		.prdiv = PRDIV , /*pll divided by 15*/
		.vdiv = VDIV,  /*vco divider : multiplied by 36*/
};

/*!
 * SYSTEM_CLOCK_setup
 * This function is used to set the clock frequency in 100MHz
 * */
void SYSTEM_CLOCK_setup()
{
	CLOCK_SetSimSafeDivs();
	CLOCK_InitOsc0(&osc_config);
	CLOCK_SetXtal0Freq(osc_config.freq);

	CLOCK_SetFbiMode(kMCG_Dmx32Default, kMCG_DrsLow, NULL);
	CLOCK_SetFbeMode(0, kMCG_Dmx32Default, kMCG_DrsLow, NULL);
	CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pll0Config);
	CLOCK_SetPeeMode();
}
