/*
 * dsp.cpp
 *
 *      Author: Matheus
 */
#include "board.h"
#include "dsp.h"
#include "adc.h"
#include "millis.h"
#include "esp.h"


#define N_CYCLES 100


inline float sample_to_v(uint16_t sample ) {
	return (float) (3.3 / (float) 4095) * sample;
}


void UpdatePeaks(uint16_t* target, uint16_t peak) {
	for( uint16_t i = 0; i < NUM_AVG_PEAKS; i++) {
		if( peak > target[i] ) {
			for( uint16_t n = i; n < NUM_AVG_PEAKS-1; n++ ) {//do swaps
				target[n + 1] = target[n];
			}
			target[ i ] = peak;
			break;
		}
	}
}

void InvalidatePeak(uint16_t* target) {
	// invalidate peak max
	for( uint16_t n = NUM_AVG_PEAKS-1; n > 0; n-- ) {
		target[n - 1] = target[n];
	}
	target[2] = 0;
}

float MeanPeaks(uint16_t* target) {
	uint16_t sumMax = 0;

	for( uint16_t i = 0; i < NUM_AVG_PEAKS; i++) {
		sumMax += target[i];
	}
	
	return (float) sumMax / NUM_AVG_PEAKS;
}

void StartAquisition() {
	bool state = 0;

	uint16_t current = 0,
			voltage = 0;

	uint64_t currentPeaksSum = 0,
			voltagePeaksSum = 0,
			offsetSum = 0;


	uint32_t sumCurrent = 0,
			sumVoltage = 0;
	
	uint16_t peaksCurrent[NUM_AVG_PEAKS] = { 0 },
			peaksVoltage[NUM_AVG_PEAKS] = { 0 };

	uint32_t i = 0;
	uint32_t cnt = 0;
	while(1) {
		ADC_RESULT result = ADC_Read();
		current = result.current;
		voltage = result.voltage;
		
		sumCurrent += current;
		sumVoltage += voltage;
		
		
		UpdatePeaks( peaksCurrent, current );
		UpdatePeaks( peaksVoltage, voltage );

		if( i == N_CYCLES) { // Each 6 complete cycles of the sine 60Hz
			float meanCurrent = sumCurrent / N_CYCLES,
				meanVoltage = sumVoltage / N_CYCLES;

			float currentMax = MeanPeaks( peaksCurrent );
			float voltageMax = MeanPeaks( peaksVoltage );


			float voltage = VOLTAGE( sample_to_v(voltageMax) );

			float offset = sample_to_v(meanCurrent);
			float peak = sample_to_v( currentMax );
			float pp = peak - offset;
			float amp = AMP( pp );
			float power = WATT( amp, voltage );


			currentPeaksSum += currentMax;
			voltagePeaksSum += voltageMax;
			offsetSum += meanCurrent;

			printf("Mean: %.2f %.4f V  %.2f MeanMax: %.4f PP: %.4f Power: %.3f Voltage: %.4f %.4f \n", meanCurrent, offset, currentMax, peak, peak - offset, power,
					voltageMax, voltage );

			// Reset variables
			sumCurrent = sumVoltage =  0;
			i = 0;

			// invalidate peak max
			InvalidatePeak(peaksCurrent);
			InvalidatePeak(peaksVoltage);


			cnt++;
		}


		if( cnt == SEND_THRESHOLD / 100 ) {
			float mCurrentPks = currentPeaksSum / cnt;
			float mVoltagePks = voltagePeaksSum / cnt;

			float meanCurrentPeak = sample_to_v( mCurrentPks );
			float meanVoltagePeak = sample_to_v( mVoltagePks );
			float meanOffset = sample_to_v( offsetSum / cnt );

			float meanVoltage = VOLTAGE( meanVoltagePeak );

			float pp = meanCurrentPeak - meanOffset;
			float meanPower = WATT( AMP( pp ), meanVoltage);



			PostParameters params;
			params["deviceId"] = UNIT_ID;
			params["power"] = meanPower;
			params["interval"] = MEAN_PW_INTERVAL_S;
			params["offset"] = meanOffset;
			params["voltage"] = meanVoltage;
			params["sample"] = mCurrentPks;




			printf("Sending HTTP post... MeanPower: %.4f", meanPower);

			esp.SendHTTPPost( SERVER_IP, SERVER_PORT, SERVER_PATH, params );

			currentPeaksSum = offsetSum = voltagePeaksSum = 0;
			cnt = 0;
		}


		i++;

		Delay_ms( DELAY_MS_SAMPLES );
	}
}





