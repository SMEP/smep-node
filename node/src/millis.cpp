/*
 * millis.c
 *6
 *      Author: Matheus
 */
#include "board.h"
#include "millis.h"



volatile unsigned long _millis;


void Millis_Start(void) {
	SysTick_Config( SystemCoreClock / 1000);
}


void SysTick_Handler(void) {
	_millis++;
}


unsigned long Millis(void) {
	return _millis;
}


void Delay_ms( uint32_t delay ) {
	unsigned long now = Millis();

	while( (now + delay) > Millis() ) { }

}
