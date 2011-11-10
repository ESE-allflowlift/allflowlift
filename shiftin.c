/* Shift register Driver */
/* 				  	     */
/* By Jelle and Randy    */
/* 					     */
#define F_CPU (8000000UL)

#include <avr/io.h>
#include <util/delay.h>

#define P_S PD7
#define P_CL PD5
#define P_PE PD0
#define P_MR PD3
#define DELAY_S 1


void shiftregister_read(int * pin_array, int number_of_shiftregisters)
{
	_delay_us(DELAY_S);

	PORTD &= ~(1 << P_PE); //PE Low
	_delay_us(DELAY_S);
	PORTD |= (1 << P_CL); // Set clock high
	_delay_us(DELAY_S);
	PORTD &= ~(1 << P_CL); // Set Clock Low
	PORTD |= (1 << P_PE); // PE high
	_delay_us(DELAY_S);

	*(pin_array) = (int) (PIND & (1 << P_S)); // Read serial pin into array

	_delay_us(10);

	int i;
	for (i=1; i<(number_of_shiftregisters*8); i++)
	{
		PORTD |= (1 << P_CL); // Set clock high
		_delay_us(DELAY_S);
		*(pin_array+i) = (int) (PIND & (1 << P_S)); // Read serial pin into array
		PORTD &= ~(1 << P_CL); // Set Clock Low
		_delay_us(DELAY_S);
	}
}

void shiftregister_init(void) 
{
	DDRD |= (1 << P_CL); // 7 CP Clock Puls (low to high triggered)
	DDRD |= (1 << P_PE); // 15 PE (Parallel enable input - active low)
	DDRD &= ~(1 << P_S); // 13 Serial out (serial in for MCU)

	PORTD &= ~(1 << P_CL); // Clock Low
	PORTD |= (1 << P_PE); // PE High
	_delay_us(DELAY_S);
}
