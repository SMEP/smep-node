/*
 * millis.h
 *
 *      Author: Matheus
 */

#ifndef MILLIS_H_
#define MILLIS_H_

void 	Millis_Start(void);
unsigned long Millis(void);
void Delay_ms( uint32_t delay );




#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void);
#ifdef __cplusplus
}
#endif

#endif /* MILLIS_H_ */
