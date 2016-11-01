/*
 * uart.cpp
 *
 *  Created on: Nov 1, 2016
 *      Author: Matheus
 */

#include "uart.h"
#include "chip.h"
#include "WString.h"

#define LPC_USART       LPC_USART1
#define LPC_IRQNUM      USART1_4_IRQn
#define LPC_UARTHNDLR   USART1_4_IRQHandler

void UART_Init(const uint32_t baud) {
	/* UART signals on pins PIO0_14 (FUNC4, U1_TXD) and PIO0_13 (FUNC4, U1_RXD) */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 14, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 13, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	/* Use 48x expected UART baud rate for fractional baud mode. (Lower limit
	   is 16x.) */
	Chip_Clock_SetUSARTNBaseClockRate((115200 * 48), true);

	Chip_UARTN_Init( LPC_USART );
	Chip_UARTN_ConfigData(LPC_USART, UARTN_CFG_DATALEN_8 | UARTN_CFG_PARITY_NONE | UARTN_CFG_STOPLEN_1);
	Chip_UARTN_SetBaud(LPC_USART, baud);

	Chip_UARTN_Enable( LPC_USART );
	Chip_UARTN_TXEnable( LPC_USART );
}

int UART_Send(const void* data, int numBytes) {
	return Chip_UARTN_SendBlocking( LPC_USART, data, numBytes );
}

int UART_Send(String str) {
	return UART_Send( str.c_str(), str.length() );
}

bool UART_Available() {
	return Chip_UARTN_GetStatus( LPC_USART ) & UARTN_STAT_RXRDY;
}

uint8_t UART_Read() {
	return Chip_UARTN_ReadByte( LPC_USART );
}

void UART_IRQ_Init() {
}

void UART_IntEnable() {
}

void UART_IntDisable() {
}





