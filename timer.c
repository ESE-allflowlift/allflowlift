#include <avr/io.h>
#include <avr/interrupt.h>

#include "pin_driver.h"

extern int t_nadraai;
extern int t_nadraai_hoogwater;
extern int t_looptijd;

int compare_matches = 0; // Number of compare matches
int compare_matches_in_one_second;

void timerdriver_init (void) {
	sei(); // Enable global interrupts

	double cpu_clock = 8000000; // CPU frequentie in Hz
	int prescaler = 1024; // Timer prescaler
	int compare_value = 255; // Timer compare register

	float compare_match_every = ((1/cpu_clock) / prescaler) * compare_value; // In seconds
	compare_matches_in_one_second = 1 / compare_match_every;

	TCNT0 = 0x00; // Initial TCNT0 value
	OCR0 = compare_value; // Compare register
	TIFR = (1 << OCF0);
	TCCR0 = (1 << COM00); //clear on compare match
	TCCR0 = (1 << WGM01); //CTC mode
	TCCR0 = (1 << CS02) | (1 << CS00); //prescaler

	TIMSK = (1 << OCIE0); //Enable compare match interrupt

}

ISR (TIMER0_COMP_vect) {
	TCNT0 = 0x00; // Force clear
	compare_matches++; // Count compare matches
	if (compare_matches >= compare_matches_in_one_second) // One second has passed (include margin for error)
	{
		compare_matches = 0; // Reset compare matches
		t_nadraai++;
		t_nadraai_hoogwater++;
		t_looptijd++;
	}
}
