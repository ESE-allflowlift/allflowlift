#include <avr/io.h>
#include <avr/interrupt.h>

#include "pin_driver.h"

extern int a_standby;

void timerdriver_init (void) {
	sei();

	TCNT0 = 0x00;
	OCR0 = 255;
	TIFR = (1 << OCF0);
	TCCR0 = (1 << COM00); //clear on compare match
	TCCR0 = (1 << WGM01); //CTC mode
	TCCR0 = (1 << CS02) | (1 << CS00); //prescaler

	TIMSK = (1 << OCIE0);

}

ISR (TIMER0_COMP_vect) {
	TCNT0 = 0x00;
	switch (a_standby) 
	{
		case 0:
		{
			//a_standby = 1;
			//pindriver_setpin("C", PC4, 1);
		break;
		}
		case 1:
		{
			//a_standby = 0;
			//pindriver_setpin("C", PC4, 0);
		break;
		}
	}
}
