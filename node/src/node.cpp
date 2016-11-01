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

#define LPC_UART LPC_USART0
#define BOARD_ADC_CH 0

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

static void Init_UART_PinMux(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
}


bool ADC_Done() {
	uint32_t rawSample = Chip_ADC_GetDataReg(LPC_ADC, 0 );

	return (bool) ADC_DR_DONE(rawSample); // Data valid flag;
}

int main(void) {

    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();

    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);

    Init_UART_PinMux();

    Chip_UART0_Init( LPC_UART );
    Chip_UART0_SetBaud( LPC_UART, 115200 );
    Chip_UART0_TXEnable( LPC_UART );

    Board_UARTPutSTR( "HelloWorld! \n");


    Chip_ADC_Init( LPC_ADC, 0 );


    Chip_ADC_SetClockRate( LPC_ADC, ADC_MAX_SAMPLE_RATE / 100 ); // Calibrarion requires around 500kHz
    Chip_ADC_StartCalibration( LPC_ADC );

    while( ! (Chip_ADC_IsCalibrationDone( LPC_ADC )) ) {}

    Chip_ADC_SetClockRate( LPC_ADC, ADC_MAX_SAMPLE_RATE );



	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 9, (IOCON_FUNC1 | IOCON_MODE_INACT |
										   IOCON_ADMODE_EN));

	Chip_ADC_SetupSequencer( LPC_ADC, ADC_SEQA_IDX,
			(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH) | ADC_SEQ_CTRL_HWTRIG_POLPOS  ) );

	Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX );
	Chip_ADC_SetSequencerBits(LPC_ADC, ADC_SEQA_IDX, ADC_SEQ_CTRL_START );


	uint32_t sample = 0;

	uint32_t flags = 0;

	char str[200];
    volatile static int i = 0 ;
    int limit = 999;
    // Enter an infinite loop, just incrementing a counter
    float conv = 0;
    while(1) {

    	while( !ADC_Done() ) {
    		//DEBUGSTR("ADC NOT DONE \r\n");
    		i++;
    	}


    	sample = ADC_DR_RESULT( Chip_ADC_GetDataReg( LPC_ADC, 0 ) );

    	Chip_ADC_ClearFlags( LPC_ADC, ADC_FLAGS_SEQA_INT_MASK );

    	conv = (3.3 / 4095) * sample;

    	sprintf(str, "%.4f V	 %d\r\n ", conv, sample );
    	Board_UARTPutSTR( str );

    	Chip_ADC_SetSequencerBits(LPC_ADC, ADC_SEQA_IDX, ADC_SEQ_CTRL_START );
    	//for( i = 0; i < limit; i++ ) {}

    }
    return 0 ;
}
