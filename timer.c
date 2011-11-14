#include <avr/io.h>
#include <avr/interrupt.h>

int main (void) {
	/* init */

	sei();

	DDRC = 0xFF;
	PORTC = 0x00;

	TCNT0 = 0x00;
	OCR0 = 2;
	TIFR = (1 << OCF0);
//	TCCR0 = (0 << COM01) | (1 << COM00); //clear on compare match
	TCCR0 = (1 << WGM01); //CTC mode
	TCCR0 = (1 << CS02) | (1 << CS00); //prescaler

	TIMSK = (1 << OCIE0);
	while (1) {}

}

ISR(TIMER0_COMP_vect) {
	PORTC ^= 0xFF;
}
