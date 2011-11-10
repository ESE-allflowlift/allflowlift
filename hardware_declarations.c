/** \file hardware_declarations.c
 * Pin declaraties.
 * Hierin worden macro's gedefineerd om de hardware pins een naam te geven en de bijbehorende functies. Ook worden hier timers en interrupt geinitialiseerd
 */

#include <avr/io.h>

/* Sourcing/Sinking */
#define ON 0
#define OFF 1

/*****************/
/* input macro's */
/*****************/

/* adc, nivosensor */
#define NIVOSENSOR PA0

/******************/
/* output macro's */
/******************/

/* motors */
#define MOTOR1 PC1
#define MOTOR2 PC2


void init_pins(void) {
	/* DDRA registers */
		/* Outputs */
		
		/* Inputs */
		DDRA |= (0 << NIVOSENSOR); /* Hier moet ADC van gemaakt worden */

	/* DDRB registers */
		/* Inputs */
		/* Outputs */
		DDRB |= (1 << MOTOR1);
		DDRB |= (1 << MOTOR2);
	/* DDRC registers */

		DDRC = 0xFF; /* Ledjes */
	/* DDRD registers */
		/* Outputs */

}

//read shift register en zet deze in variabelen
//set shift register variabelen dmv shift register
//read adc nivosensor
//set pomp

void set_pomp (int * pomp) {
	if (pomp[0] == 1) {
		PORTC |= (ON << MOTOR1);
	} else {
		PORTC |= (OFF << MOTOR1);
	}
	if (pomp[1] == 1) {
		PORTC |= (ON << MOTOR2);
	} else {
		PORTC |= (OFF << MOTOR2);
	}
}

