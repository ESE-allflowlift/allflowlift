#include <avr/io.h>
#include <avr/interrupts.h>

/*
 Serial driver by Randy Wijnants, ES1V
*/

/* 
Serial settings:
 -9600 baud rate (0.2% error)
 -Even parity
 -1 Stop bit
 -8 byte frame size
 -Asynchronous data transfer
*/

void usart_init(void)
{
	cli(); // for interrupt driven: global interrupt flag cleared (interrupts globally disabled) when doing init.

	UBRRL = 51; // set UBRRL to 51 (9600 baud rate)

	UCSRB |= (1 << RXCIE); // set RXCIE in UCSRB for receive interrupt enable
	UCSRB = (1<<RXEN)|(1<<TXEN); // Enable transmitter and receiver

	UCSRC |= (1 << URSEL) | (1 << UPM1); // set UPM1 in UCSRC for even parity

	sei(); // Enable global interrupts
}


int usart_read_byte(void)
{
	// polling: when RXC in UCSRA is set: check if PE in UCSRA is set. Otherwise read UDR
}

int usart_send_byte(char byte)
{
	if ((UCSRA & (1 << UDRE)) != 0) // Write UDR only if UDRE in UCSRA is set
	{
		UDR = byte;
		return 1; // Send succesful
	}
	else
	{
		return 0; // transmit buffer not ready, byte not send
	}
}

ISR(USART_RXC_vect) // Receive Interrupt Vector
{ 
	char receivedByte;
	// Interrupt: check PE in UCSRA is set. Otherwise read UDR
	if ((UCSRA & (1 << PE)) == 0) // No parity error, assume correct byte
	{
		receivedByte = UDR;
		if (receivedByte == 'A') // Capital A is init byte
		{
			while (usart_send_byte('B') == 1) { };
			while (usart_send_byte('C') == 1) { };
		{
	}
} 


















