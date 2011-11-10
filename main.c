#define F_CPU (8000000UL)

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "display.h"
#include "shiftin.h"

/*
*/

/** \file variables.h
 * Alle variabelen welke de fucties aanroepen/bewerken
 * 
 */

#define TRUE 1
#define FALSE 0

/* 
	Error Variables test randy 123
	Prefix: e_

*/


int e_nivo_breuk = FALSE; /* Nivosensor voelerbreuk P1	Bool Active 1 */
int e_nivo_kortsluiting = FALSE; /* Nivosensor kortsluiting	Bool Active 1 */
int e_motor_fase[2] = {FALSE, FALSE}; /* Motorfase verkeerd (per pomp)	Array Bool Active 1 */
int e_motor_stroom[2] = {FALSE, FALSE}; /* Motorstroom te hoog (per pomp)	Array Bool Active 1 */
int e_motor_temp[2] = {FALSE, FALSE}; /* Motortemp te hoog	Array Bool Active 1 */

/* 
	Log variables
	Prefix: z_
*/

/* Te doen: initializatie halen uit EEPROM  */
/* Te doen: rekening houden met looptijd, 65000 minuten is te kort */

long int z_pomp_looptijd[2]; /* Totale looptijd per pomp (in min.)	double (1-65000) */
long int z_pomp_inschakelingen[2]; /* Totaal aantal inschakelingen per pomp	uINT (0-255) */

/*
	Sensor variables (input)
	Prefix: s_
*/

int s_nivo;	/* Waarde nivosensor	uINT (0-1023) */
int s_motorPower[2];/* Motor Overbelast	Bool Active 1 */
int s_motorTemp[2]; /* Motor Oververhit	Bool Active 1 */
int s_motorFase[2]; /* Motor Fase verkeerd	Bool Active 0 (Of 1?) */
int s_hoogwater; /*Hoogwater melding	Bool Active 1 */

/*
	Bediening variables (input)
	Prefix: b_
*/

int b_handAuto[2]; /* 2x Handmatig of automatische stand (per pomp)	Array Bool
0=auto 1=hand */
int b_inschakelingHand[2]; /* 2x	Inschakeling hand (per pomp) Array Bool Active 1 */
int b_reset[2]; /* 2x Reset (per pomp) Array Bool Active 1  waneer de reset knop ingedrukt wordt moet deze hoog worden, hij word na bijv 1 seconden weer laag */
int b_cursor_up; /*	cursur knoppen	array bool active1 */
int b_cursor_down; /* cursur knoppen	array bool active1 */
int b_cursor_left;	/* cursur knoppen	array bool active1 */
int b_cursor_right; /*	cursur knoppen	array bool active1 */

/*
	Custom variables (input)
	Prefix: c_
*/

unsigned int c_nivo_bovenste; /* Bovenste inschakelnivo	uINT (in cm) */
unsigned int c_nivo_onderste; /* Onderste inschakelnivo	uINT (in cm) */
unsigned int c_nivo_uitschakel;	/* UItschakelnivo	CONST uINT (4 mA) */
unsigned int c_nadraai; /*	Nadraaitijd	uINT (3-10 sec)	t_nadraai (sec) */
unsigned int c_nadraai2; /*	Nadraaitijd bij hoogwatersensor	uINT (tot 90sec)	t_nadraaitijd2 */
unsigned int c_looptijd; /* Maximale looptijd	uINT (10-15 min)	t_looptijd (min) */
unsigned int c_idnummer; /*	id nummer van de pomp	uINT(0-65000) */

/*
	Set variables (Output/Actuator)
	Prefix: a_
*/

int a_pomp_active[2]; /* Pomp actief/in bedrijf	Bool (1=active) */
int a_pomp_error[2]; /* Pomp in storing?	Bool (1=active) */
int a_error; /* Algemene foutmelding	Bool (1=active) */
int a_standby; /* Standby melding	Bool (1=active) */
int a_hoogwateralarm; /*  */


void f_pomp_seterror (int * s_pomp_error, const int * e_motor_fase, const int * e_motor_stroom, const int * e_motor_temp, const int * b_handauto, const int * b_reset) {
	if ((*(e_motor_fase+0) == 1 )|| ((*(e_motor_stroom+0) == 1 ) || (*(e_motor_temp+0) == 1)  || (*(b_handauto+0) == 1)))
	{
	*(s_pomp_error+0) = 1; /*als fase of stroom of temp verkeerd is of motor staat in hand dan wordt de error variablele geset*/
	}
	if ((*(e_motor_fase+1) == 1) || (*(e_motor_stroom+1) == 1) || (*(e_motor_temp+1) == 1) || (*(b_handauto+1) == 1))
	{
	*(s_pomp_error+1) = 1; /*als fase of stroom of temp verkeerd is of motor staat in hand dan wordt de error variablele geset*/
	}
	if ((*(b_reset+0) == 1) && (*(e_motor_fase+0) == 0) && (*(e_motor_stroom+0) == 0) && (*(e_motor_temp+0) == 0) && (*(b_handauto+0) == 0))
	{
	*(s_pomp_error+0) = 0; /* geen error wel reset error melding wordt 0*/
	}	
	if ((*(b_reset+1) == 1) && (*(e_motor_fase+1) == 0) && (*(e_motor_stroom+1) == 0) && (*(e_motor_temp+1) == 0) && (*(b_handauto+1) == 0))
	{
	*(s_pomp_error+1) = 0; /* geen error wel reset error melding wordt 0*/
	}	

}

void setvars_shiftregister(void) {
	int shift[17];
	shiftregister_read(&shift[0], 2);

	b_cursor_up = shift[1]; // Pin 12 (1)
	b_cursor_down = shift[0]; // Pin 14 (1)
	b_cursor_left = shift[15]; // Pin 2 (2)
	b_cursor_right = shift[14]; // Pin 3 (2)
	
}

int main(void) {	
	//init_pins();

	_delay_ms (10);
	display_init();
	shiftregister_init();

	display_line("====================",0);
	display_line("Project Allflowlift ",1);
	display_line("Firmware version 0.1",2);
	display_line("======= ES1V2 ======",3);
	_delay_ms (1000);

	char display_buffer[80]; //
	char * display_buffer_line[4];
	display_buffer_line[0] = (char *) &display_buffer + 0;
	display_buffer_line[1] = (char *) &display_buffer + 20;
	display_buffer_line[2] = (char *) &display_buffer + 40;
	display_buffer_line[3] = (char *) &display_buffer + 60;

	int cursor_stateA = 0;
	int cursor_stateB = 0;
	int cursor_stateC = 0;
	int cursor_stateD = 0;
	char affect_state = 'A';

	while (1) {
		setvars_shiftregister(); // Set vars via shiftregister (sensors & bedieing)

		// Cursor state machine
		switch (affect_state)
		{
			case 'A':
				display_line("                    ", 0);
				_delay_ms(100);
				display_line(display_buffer_line[0],0);
				if (b_cursor_right != 0) 
				{
					cursor_stateA++;
				}
				if (b_cursor_left != 0) 
				{
					cursor_stateA--;
				}
			break;
			case 'B':
				display_line("                    ", 1);
				_delay_ms(100);				
				display_line(display_buffer_line[1],1);
				if (b_cursor_right != 0) 
				{
					cursor_stateB++;
				}
				if (b_cursor_left != 0) 
				{
					cursor_stateB--;
				}
	break;
			case 'C':
				display_line("                    ", 2);
				_delay_ms(100);				
				display_line(display_buffer_line[2],2);
				if (b_cursor_right != 0) 
				{
					cursor_stateC++;
				}
				if (b_cursor_left != 0) 
				{
					cursor_stateC--;
				}
	break;
		}

		if (b_cursor_down != 0)
		{
			affect_state++;
		}
		if (b_cursor_up != 0)
		{
			affect_state--;
		}

		if (affect_state > 'C') { affect_state = 'C'; }
		if (affect_state < 'A') { affect_state = 'A'; }

		// Print display state machine
		if (cursor_stateA > 2) { cursor_stateA = 2; }
		if (cursor_stateA < 0) { cursor_stateA = 0; }
		switch (cursor_stateA)
		{
			case 0: // Instellingen menu
				strcpy (display_buffer_line[0], "   Instellingen >   ");
				switch (cursor_stateB)
				{
					case 0: // Looptijd bij vloeistofsesnro
						strcpy (display_buffer_line[1], "  Loopt. vl. sens. >");
					break;
					case 1: // Bovenste inschakelnivo
						strcpy (display_buffer_line[1], "< Bov. Insch. nivo >");
					break;
				}
			break;
			case 1: // <Storingsmeldingen> menu
				strcpy (display_buffer_line[0], "   < Storingen >    ");
			break;
			case 2: // Logs menu
				strcpy (display_buffer_line[0], "       < Logs       ");
			break;
		}
		

		_delay_ms(300);
	}

}

/**
 *functie f_pomp_seterror zet pomp in error als dat nodig is 
 * @param s_pomp_error	array van pomp in error
 * @param e_motor_fase array motor fase verkeerd
 * @param e_motor_stroom array motor stroom verkeerd
 * @param e_motor_temp array motor tempratuur verkeerd
 * @param b_handauto array motor op handmatig
 * @param b_reset array van resetknoppen
 */

