/*
 * adc.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Matheus
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void ADC_Init();
bool ADC_Done();
uint16_t ADC_Read();




#endif /* ADC_H_ */
