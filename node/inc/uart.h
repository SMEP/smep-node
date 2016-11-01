/*
 * uart.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Matheus
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

class String; // Forward declaration


void UART_Init( const uint32_t baud );
int UART_Send(const void * data, int numBytes );
int UART_Send( String cmd );
bool UART_Available();
uint8_t UART_Read();
void UART_IRQ_Init();
void UART_IntEnable();
void UART_IntDisable();



#endif /* UART_H_ */
