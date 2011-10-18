/** \file pin_declarations.c
 * Pin declaraties.
 * Hierin worden macro's gedefineerd om de hardware pins een naam te geven en de bijbehorende functies
 */


#define STANDBY1 PA0
#define STANDBY2 PA1

#define TESTPIN PD0

void init_pins(void) {
	/* DDRA registers */
		/* Outputs */
		
		/* Inputs */
		DDRA |= (0 << STANDBY1) || (0 << STANDBY2);

	/* DDRB registers */
		/* Inputs */
	/* DDRC registers */
	/* DDRD registers */
		/* Outputs */
		DDRD |= (1 << TESTPIN);

}
