/*
 * dsp.h
 *
 *      Author: Matheus
 */

#ifndef DSP_H_
#define DSP_H_

#include "definitions.h"
#include "lpc_types.h"

#define AMP(volt) 		(((float)volt/INPUT_GAIN) * 20)
#define WATT(amp, volt) 		(amp * volt)
#define VOLTAGE(volt)	((220*volt)/2.5)

float sample_to_v(uint16_t sample );

void StartAquisition();




#endif /* DSP_H_ */
