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

#include "definitions.h"

#include "adc.h"
#include "millis.h"
#include "uart.h"
#include "dsp.h"
#include "esp.h"
#include "WString.h"


#define LPC_UART LPC_USART0

ESP8266 esp;

static void Init_UART_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
}

static void InitESP() {
	Board_UARTPutSTR( "Conectando AP..." );
	esp.SetMux( true );

	esp.ConnectAP( AP_SSID, AP_PW );

	Board_UARTPutSTR( "AP Conectado!" );
}

static inline void Init() {
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


int main(void) {
	Init();

    printf( "HelloWorld! \n");


    InitESP();





	uint32_t sample = 0;

	uint32_t flags = 0;

	char str[200];
    volatile static int i = 0 ;
    int limit = 999;

    float conv = 0;


    StartAquisition();

    while(1) {} // Ooops should not be here

   /* while(1) {

    	sample = ADC_Read();


    	conv = (3.3 / 4095) * sample;

    	sprintf(str, "%.4f V	 %d\r\n ", conv, sample );
    	Board_UARTPutSTR( str );

    	String cmd = "AT+CWLAP\r\n";


    	printf("Sending data..\n");
    	SendData( (const uint8_t*)cmd.c_str(), cmd.length(), 6000, NULL);

    	Delay_ms( 400 );



    }*/
    return 0 ;
}
