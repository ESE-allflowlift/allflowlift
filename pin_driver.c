/* Pin Driver         */
/* 				  	  */
/* 					  */

#include <avr/io.h>
#include <stdlib.h>

// OFF = 0 (source) OFF = 1 (sink)
#define OFF 0

void pindriver_setpin(char * pin, int macro, int i_port)
{
	char c_port = * pin;
	switch(c_port)
	{
		case 65: //Letter A
		{
			if (i_port == OFF) {
				PORTA &= ~(1 << (macro));
			} else {
				PORTA |= (1 << (macro));
			}
		break;
		}
		case 66: //Letter B
		{
			if (i_port == OFF) {
				PORTB &= ~(1 << (macro));
			} else {
				PORTB |= (1 << (macro));
			}
		break;
		}
		case 67: //Letter C
		{
			if (i_port == OFF) {
				PORTC &= ~(1 << (macro));
			} else {
				PORTC |= (1 << (macro));
			}
		break;
		}
		case 68: //Letter D
		{
			if (i_port == OFF) {
				PORTD &= ~(1 << (macro));
			} else {
				PORTD |= (1 << (macro));
			}
		break;
		}
	}
}


void pindriver_init(void) 
{
	DDRC |= (1 << PC1) | (1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7);
}

