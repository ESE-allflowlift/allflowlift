#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_VARS 6 // Max vars from PC

extern int last_3_errors[3];

extern int serialstate;
extern int serialstate_timeout;
extern int serialstate_counter;
extern int serialstate_vars[20];

extern uint16_t z_pomp_looptijd[2];
extern uint16_t z_pomp_inschakelingen[2];

extern unsigned int c_nivo_bovenste;
extern unsigned int c_nivo_onderste;
extern unsigned int c_nivo_uitschakel;
extern unsigned int c_nadraai;
extern unsigned int c_nadraai_hoogwater;
extern unsigned int c_looptijd;
extern unsigned int c_idnummer;

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

//	UBRRL = 51; // set UBRRL to 51 (9600 baud rate) U2X zero
//	UBRRL = 103; // set UBRRL to 51 (9600 baud rate) U2X set

	UBRRH = 0;
	UBRRL = 103;

	UCSRA = (1 << U2X);

//	UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN); // set RXCIE in UCSRB for receive interrupt enable
	UCSRB |= (1 << RXCIE); // set RXCIE in UCSRB for receive interrupt enable
	UCSRB |= (1 << RXEN) | (1 << TXEN); // Enable transmitter and receiver

//	UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

//	UCSRC |= (1 << URSEL) | (1 << UPM1); // set UPM1 in UCSRC for even parity

	sei(); // Enable global interrupts
}


/*
int usart_read_byte(void)
{
	// polling: when RXC in UCSRA is set: check if PE in UCSRA is set. Otherwise read UDR
}
*/

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
	receivedByte = UDR;

	if (serialstate == 0) // Receive- imitiately send process
	{
		// Interrupt: check PE in UCSRA is set. Otherwise read UDR
		if (receivedByte == 'A') // Capital A from pc = alive byte, send A+idnummer
		{
			while (usart_send_byte((char)'A') == 0); // Ack byte
			while (usart_send_byte((char)c_idnummer) == 0); // Id nummer
		}
		if (receivedByte == 'X') // Capital X from pc = send data
		{
			while (usart_send_byte((char)last_3_errors[0]) == 0);
			while (usart_send_byte((char)last_3_errors[1]) == 0);
			while (usart_send_byte((char)last_3_errors[2]) == 0);

			while (usart_send_byte((char)z_pomp_looptijd[0] >> 8) == 0);
			while (usart_send_byte((char)z_pomp_looptijd[0]) == 0);

			while (usart_send_byte((char)z_pomp_looptijd[1] >> 8) == 0);
			while (usart_send_byte((char)z_pomp_looptijd[1]) == 0);

			while (usart_send_byte((char)z_pomp_inschakelingen[0] >> 8) == 0);
			while (usart_send_byte((char)z_pomp_inschakelingen[0]) == 0);

			while (usart_send_byte((char)z_pomp_inschakelingen[1] >> 8) == 0);
			while (usart_send_byte((char)z_pomp_inschakelingen[1]) == 0);

			while (usart_send_byte((char)c_nivo_bovenste) == 0);
			while (usart_send_byte((char)c_nivo_onderste) == 0);
			while (usart_send_byte((char)c_nivo_uitschakel) == 0);
			while (usart_send_byte((char)c_nadraai) == 0);
			while (usart_send_byte((char)c_nadraai_hoogwater) == 0);
			while (usart_send_byte((char)c_looptijd) == 0);
			while (usart_send_byte((char)c_idnummer) == 0);
		}
		if (receivedByte == 'Y') // Capital Y from pc = waiting for new data from pc
		{
			while (usart_send_byte((char)'Y') == 0); // Ack byte
			serialstate = 1;
		}
	}
	else // Previous byte was Y, received data = variables from pc to mcu
	{
		serialstate_vars[serialstate_counter] = receivedByte;
		serialstate_counter++;
		
		if (serialstate_counter > MAX_VARS)
		{
			// Save vars to eeprom
			serialstate = 0;
			serialstate_timeout = 0;
			serialstate_counter = 0;
		}
	}
}

