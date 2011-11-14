/* Display Driver     */
/* 				  	  */
/* By Jelle and Randy */
/* 					  */
/* MAX_CHARS = max characters for display */
/* Support for all common HD44780 Displays */

#define MAX_CHARS 20
#define DATA_DELAY 1

#define F_CPU (8000000UL)

#include <avr/io.h>
#include <util/delay.h> 


void function_8bit(int value) 
{
	_delay_ms(DATA_DELAY);
	PORTB = value;
}

/* Set register bit */
void function_rs(int value) 
{
	_delay_ms(DATA_DELAY);
	if (value == 0) {
		PORTD &= ~(1 << PD2);
	}
	if (value == 1) {
		PORTD |= (1 << PD2);
	}
}

/* Flip clock (from high to low, and back to high) */

void function_clock(void) 
{
	_delay_ms(DATA_DELAY);
	PORTD &= ~(1 << PD4);
	PORTD |= (1 << PD4);
}

void display_line (char * string, int line) 
{

	function_rs(0); /* Command mode */
			function_8bit(0xC0);

	switch (line)
	{
		case 0: /* Set cursor to line */
			function_8bit(0x80);
			break;
		case 1: 
			function_8bit(0xC0);
			break;
		case 2: 
			function_8bit(0x94);
			//function_8bit(0x90); // For 16 chars displays
			break;
		case 3: 
			function_8bit(0xd4);
			// function_8bit(0xD0); // For 16 chars displays
			break;
	}
	function_clock();

	function_rs(1);/* Character mode */
	int i;		   /* counter voor for loop */
	char symbol;   /* symbol bevat huidige karakter uit string */
	int check = 0;
	for (i=0; i < MAX_CHARS; i++) /* max 16 characters per line */
	{
		symbol = *(string+i);
		if (symbol == '\0') { check = 1; } /* break @ end of string */
		if (check == 0) {
			function_8bit(symbol);
		} else {
			function_8bit(' ');
		}
			function_clock();
	}
}


void display_init(void) 
{
	DDRB |= 0xFF; /* 8 Bit interface */

	DDRD |= (1 << PD2); /* 4. Register select */
	DDRD |= (1 << PD4); /* 6. Clock enable */

	PORTD |= (1 << PD4); /* Set Clock High (falling edge triggered) */

	function_rs(0);
	function_8bit(0x0C); /* Display on, cursor off, underline off */
	function_clock();

	function_8bit(0x3c); /* 8bit interface, 2 line mode, 5x10 format */
	function_clock();
	
	function_8bit(0x01); /* clear display */
	function_clock();
}

