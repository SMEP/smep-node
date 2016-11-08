/*
 * adc.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: Matheus
 */

#include "adc.h"
#include "board.h"

#define ADC_CH 0



void ADC_Init() {
	Chip_ADC_Init( LPC_ADC, 0 );

	Chip_ADC_SetClockRate( LPC_ADC, ADC_MAX_SAMPLE_RATE / 100 ); // Calibration requires around 500 kHz
	Chip_ADC_StartCalibration( LPC_ADC );


	while( ! ( Chip_ADC_IsCalibrationDone( LPC_ADC ) ) ) {}

	Chip_ADC_SetClockRate( LPC_ADC, ADC_MAX_SAMPLE_RATE );

	Chip_IOCON_PinMuxSet( LPC_IOCON, 1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT | // Set P1.9 to ADC
										 	IOCON_ADMODE_EN));

	Chip_ADC_SetupSequencer( LPC_ADC, ADC_SEQA_IDX,
			(ADC_SEQ_CTRL_CHANSEL(ADC_CH) |  ADC_SEQ_CTRL_HWTRIG_POLPOS) );

	Chip_ADC_EnableSequencer( LPC_ADC, ADC_SEQA_IDX );
}

bool ADC_Done() {
	uint32_t rawSample = Chip_ADC_GetDataReg(LPC_ADC, 0 );

	return (bool) ADC_DR_DONE(rawSample); // Data valid flag;
}

ADC_RESULT ADC_Read() {
	ADC_RESULT result;

	Chip_ADC_SetSequencerBits( LPC_ADC, ADC_SEQA_IDX, ADC_SEQ_CTRL_START );

	while( !ADC_Done() ) { }

	result.current = ADC_DR_RESULT( Chip_ADC_GetDataReg( LPC_ADC, 0 ) );

	Chip_ADC_ClearFlags( LPC_ADC, ADC_FLAGS_SEQA_INT_MASK );

	return result;
}



