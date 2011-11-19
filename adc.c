#include <avr/io.h>

extern int s_nivo;

void adc_init(void) {
	ADCSRA |= (1 << ADEN); // Enable ADC
//	ADMUX |=i // Set MUX bits in ADMUX for ADC channel selection
// Set ADIE in ADCSRA for ADC conversion complete interrupt
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Set ADPS2 and ADPS1 for 64 prescaler (CPU clock 8Mhz)
}

void adc_read(void) {
	ADCSRA |= (1 << ADSC); // Start Conversion
	while ((ADCSRA & (1 << ADIF)) == 0) { } // while conversion in progress

	s_nivo = ADCL;
	s_nivo |= (ADCH << 8);
	
	// Eerst ADCL en en ADCH lezen
	// ADSC = 0 and ADIF = 1 > conversie compleeit
}
