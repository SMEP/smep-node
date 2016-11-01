/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "board.h"


#include <cr_section_macros.h>

#include "adc.h"
#include "millis.h"
#include "uart.h"
#include "WString.h"

#define LPC_UART LPC_USART0



static void Init_UART_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
}

inline void Init() {
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();

    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);


    Init_UART_PinMux();
    Board_Debug_Init(); // Init UART0 for Debbugging

    Millis_Start();
    UART_Init( 115200 );
    ADC_Init();
}

bool ReadResponse(String& resp, const int timeout ) {
	unsigned long int time = Millis();

	bool success = false;

	UART_IntDisable();

	while( (time + timeout) > Millis() ) {
		while( UART_Available() ) {
			resp += (char) UART_Read();

			if( resp.indexOf("OK") ) {
				success = true;
				break;
			} else if( resp.indexOf("FAIL") )
				break;
		}
	}

	UART_IntEnable();

	return success;
}


bool SendData( const uint8_t * data, const uint16_t size, const int timeout, String* ret) {
	bool success;
	String resp;


	UART_Send( data, size );

	success = ReadResponse(resp, timeout);

	char * t = (char * ) resp.c_str();
	Board_UARTPutSTR( t );



	if( ret != NULL )
		*ret = resp;


	return success;
}

int main(void) {
	Init();

    printf( "HelloWorld! \n");





	uint32_t sample = 0;

	uint32_t flags = 0;

	char str[200];
    volatile static int i = 0 ;
    int limit = 999;
    // Enter an infinite loop, just incrementing a counter
    float conv = 0;
    while(1) {

    	sample = ADC_Read();


    	conv = (3.3 / 4095) * sample;

    	sprintf(str, "%.4f V	 %d\r\n ", conv, sample );
    	Board_UARTPutSTR( str );

    	String cmd = "AT+CWLAP\r\n";


    	printf("Sending data..\n");
    	SendData( (const uint8_t*)cmd.c_str(), cmd.length(), 6000, NULL);

    	Delay_ms( 400 );



    }
    return 0 ;
}
