#define F_CPU (8000000UL)

#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

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

uint16_t z_pomp_looptijd[2]; /* Totale looptijd per pomp (in min.)	double (1-65000) */
uint16_t z_pomp_inschakelingen[2]; /* Totaal aantal inschakelingen per pomp	uINT (0-255) */

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
unsigned int c_nadraai_hoogwater; /*	Nadraaitijd bij hoogwatersensor	uINT (tot 90sec)	t_nadraaitijd2 */
unsigned int c_looptijd;  /* Maximale looptijd	uINT (10-15 min)	t_looptijd (min) */
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



void setvars_shiftregister(void) {
	int shift[17];
	shiftregister_read(&shift[0], 2);

	b_cursor_up = shift[1]; // Pin 12 (1)
	b_cursor_down = shift[0]; // Pin 14 (1)
	b_cursor_left = shift[15]; // Pin 2 (2)
	b_cursor_right = shift[14]; // Pin 3 (2)
	
}

uint16_t EEMEM eeprom_z_pomp_looptijd_1;
uint16_t EEMEM eeprom_z_pomp_looptijd_2;
uint16_t EEMEM eeprom_z_pomp_inschakelingen_1;
uint16_t EEMEM eeprom_z_pomp_inschakelingen_2;

uint8_t EEMEM eeprom_c_nivo_bovenste;
uint8_t EEMEM eeprom_c_nivo_onderste; 
uint8_t EEMEM eeprom_c_nivo_uitschakel;	
uint8_t EEMEM eeprom_c_nadraai; 
uint8_t EEMEM eeprom_c_nadraai_hoogwater; 
uint8_t EEMEM eeprom_c_looptijd; 
uint8_t EEMEM eeprom_c_idnummer; 

void setvars_from_eeprom(void) {
	z_pomp_looptijd[0] = eeprom_read_word(&eeprom_z_pomp_looptijd_1);
	z_pomp_looptijd[1] = eeprom_read_word(&eeprom_z_pomp_looptijd_2);
	z_pomp_inschakelingen[0] = eeprom_read_word(&eeprom_z_pomp_inschakelingen_1);
	z_pomp_inschakelingen[1] = eeprom_read_word(&eeprom_z_pomp_inschakelingen_2);

	c_nivo_bovenste = eeprom_read_byte(&eeprom_c_nivo_bovenste);
	c_nivo_onderste = eeprom_read_byte(&eeprom_c_nivo_onderste);
	c_nivo_uitschakel = eeprom_read_byte(&eeprom_c_nivo_uitschakel);
	c_nadraai = eeprom_read_byte(&eeprom_c_nadraai);
	c_nadraai_hoogwater = eeprom_read_byte(&eeprom_c_nadraai_hoogwater);
	c_looptijd = eeprom_read_byte(&eeprom_c_looptijd);
	c_idnummer = eeprom_read_byte(&eeprom_c_idnummer);
}

void derde_regel_instellingen (char * tweede_regel, int event_code_i, uint8_t * custom_var, int * custom_temp, int min_val, int max_val, char * derde_regel, int * e_event_code, char * e_display_buffer_1, char * e_display_buffer_2) {
	strcpy (e_display_buffer_1, tweede_regel);
	*e_event_code = event_code_i;

	char tempstring[2];
	int tempint = *custom_var + *custom_temp;
						
	if (tempint > max_val) {
		*custom_temp = max_val - *custom_var;
		tempint = c_nivo_bovenste + *custom_temp;
	}
	if (tempint < min_val) {
		*custom_temp = min_val - *custom_var;
		tempint = *custom_var + *custom_temp;
	}

	itoa(tempint, tempstring, 10); // Convert integer to ascii with radix 10
	strcpy (e_display_buffer_2, derde_regel);
	strcpy (e_display_buffer_2+strlen(derde_regel), tempstring);
}

int main(void) {

	setvars_from_eeprom();

	_delay_ms (10);
	display_init();
	shiftregister_init();

	display_line("====================",0);
	display_line("Project Allflowlift ",1);
	display_line("Firmware version 0.1",2);
	display_line("======= ES1V2 ======",3);
	_delay_ms (1000);


	char display_buffer[86]; // 86 voor elke \0 terminator
	char * display_buffer_line[4];
	display_buffer_line[0] = (char *) &display_buffer;
	display_buffer_line[1] = (char *) &display_buffer + 21;
	display_buffer_line[2] = (char *) &display_buffer + 43;
	display_buffer_line[3] = (char *) &display_buffer + 65;

	strcpy(display_buffer_line[3], "P1:aan P2:uit E:geen");

	int event_submenuC = 0; //Simuleer event van submenu C naar submenu B (EEPROM SAVE)
	int event_code = 0; //Event code voor afhandeling

	int cursor_stateA = 0; //State machine 1e regel
	int cursor_stateB = 0; //State machine 2e regel
	int cursor_stateC = 0; //State machine 3e regel //Niet nodig?
	int cursor_stateD = 0; //Niet nodig?
	char affect_state = 'A'; //Cursor state machine
	int cursor_stateC_temp = 0; 

	while (1) {
		setvars_shiftregister(); // Set vars via shiftregister (sensors & bedieing)
		
		if (event_submenuC == 1) { // Process event
			event_submenuC = 0; // Reset event
			switch (event_code) { // Process event code
				case 100:
					eeprom_write_byte(&eeprom_c_looptijd, (c_looptijd + cursor_stateC_temp));
				break;
				case 101:
					eeprom_write_byte(&eeprom_c_nivo_bovenste, (c_nivo_bovenste + cursor_stateC_temp));
				break;
				case 102:
					eeprom_write_byte(&eeprom_c_nivo_onderste, (c_nivo_onderste + cursor_stateC_temp));
				break;
				case 103:
					eeprom_write_byte(&eeprom_c_nivo_uitschakel, (c_nivo_uitschakel + cursor_stateC_temp));
				break;
				case 104:
					eeprom_write_byte(&eeprom_c_nadraai, (c_nadraai + cursor_stateC_temp));
				break;
				case 105:
					eeprom_write_byte(&eeprom_c_nadraai_hoogwater, (c_nadraai_hoogwater + cursor_stateC_temp));
				break;
				case 106:
					eeprom_write_byte(&eeprom_c_idnummer, (c_idnummer + cursor_stateC_temp));
				break;
			}
			setvars_from_eeprom(); // Lees eeprom waardes als men in het instellingen menu gaat
			cursor_stateC_temp = 0; // Reset Temp var
		}
		display_line(display_buffer_line[0],0);
		display_line(display_buffer_line[1],1);
		display_line(display_buffer_line[2],2);
		display_line(display_buffer_line[3],3);
		

		// Cursor en knipper state machine
		switch (affect_state)
		{
			case 'A':
				display_line("                    ", 0);
				_delay_ms(10);
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
				_delay_ms(10);
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
				_delay_ms(10);
				display_line(display_buffer_line[2],2);
				if (b_cursor_right != 0) 
				{
					cursor_stateC_temp++;
				}
				if (b_cursor_left != 0) 
				{
					cursor_stateC_temp--;
				}
			break;
		}

		if (b_cursor_down != 0)
		{
			affect_state++;
			if (affect_state == 'C') { 
				cursor_stateC_temp = 0; // Reset van submenu B naar submenu C
			}
		}
		if (b_cursor_up != 0)
		{
			affect_state--;
			if (affect_state == 'B') { 
				// cursor_stateC_temp hier niet resetten! anders word eeprom niet juist opgeslagen
				event_submenuC = 1; //Simuleer event voor transitie van submenu C naar B (EEPROM SAVE)
			}
		}

		// Affect state boundary
		if (affect_state > 'C') { affect_state = 'C'; }
		if (affect_state < 'A') { affect_state = 'A'; }

		// Print display state machine

		// Main Menu Boundary
		if (cursor_stateA > 2) { cursor_stateA = 2; }
		if (cursor_stateA < 0) { cursor_stateA = 0; }
		switch (cursor_stateA)
		{
			case 0: // Instellingen menu
				strcpy (display_buffer_line[0], "   Instellingen >   ");

				// Submenu B Boundary
				if (cursor_stateB > 6) { cursor_stateB = 6; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				setvars_from_eeprom(); // Lees eeprom waardes als men in het instellingen menu gaat
				switch (cursor_stateB)
				{
					case 0: // Looptijd bij vloeistofsensor
						derde_regel_instellingen (" Loopt. vl. sens.  >", 100, (uint8_t*) &c_looptijd, &cursor_stateC_temp, 1, 90, "Sec. (1-90): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 1: // bovenste inschakelnivo
						derde_regel_instellingen ("< Bov. insch. nivo >", 101, (uint8_t*) &c_nivo_bovenste, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 2: // onderste inschakelnivo
						derde_regel_instellingen ("< Ond. insch. nivo >", 102, (uint8_t*) &c_nivo_onderste, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 3: // uitschakel nivo
						derde_regel_instellingen ("< Uitschakel  nivo >", 103, (uint8_t*) &c_nivo_uitschakel, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 4: // nadraai inschakelnivo
						derde_regel_instellingen ("< Nadraaitijd Pomp >", 104, (uint8_t*) &c_nadraai, &cursor_stateC_temp, 3, 10, "Sec. (3-10): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 5: // nadraai bij hoogwater inschakelnivo
						derde_regel_instellingen ("< Nadraai. Hoogwa. >", 105, (uint8_t*) &c_nadraai_hoogwater, &cursor_stateC_temp, 1, 90, "Sec. (1-90): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 6: // Id nummer
						derde_regel_instellingen ("< ID Nummer         ", 106, (uint8_t*) &c_idnummer, &cursor_stateC_temp, 0, 255, "ID (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					}
			break;
			case 1: // Storingsmeldingen menu
				strcpy (display_buffer_line[0], "   < Storingen >    ");

				// Submenu B Boundary
				if (cursor_stateB > 2) { cursor_stateB = 2; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				switch (cursor_stateB)
				{
					case 0: // Globale meldingen
						strcpy (display_buffer_line[1], " Globale meldingen >");
					break;
					case 1: // Pomp 1 specifiek
						strcpy (display_buffer_line[1], "< Pomp 1 specifiek >");
					break;
					case 2: // Globale meldingen
						strcpy (display_buffer_line[1], "< Pomp 2 specifiek  ");
					break;
				}

			break;
			case 2: // Logs menu
				strcpy (display_buffer_line[0], "       < Logs       ");

				// Submenu B Boundary
				if (cursor_stateB > 1) { cursor_stateB = 1; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				switch (cursor_stateB)
				{
					case 0: // Pomp 1
					{
						strcpy (display_buffer_line[1], "      Pomp 1 >      ");

						// Submenu C Boundary
						if (cursor_stateC_temp > 1) { cursor_stateC_temp = 1; }
						if (cursor_stateC_temp < 0) { cursor_stateC_temp = 0; }		
						switch (cursor_stateC_temp) 
						{
							case 0: // Looptijd
							{
								char tempstring[5];
								itoa (z_pomp_looptijd[0], tempstring, 10);
								strcpy (display_buffer_line[2], "Looptijd: ");
								strcpy (display_buffer_line[2]+11, tempstring);
							}
							break;
							case 1: // Aantal inschakelingen
							{
								char tempstring[5];
								itoa (z_pomp_inschakelingen[0], tempstring, 10);
								strcpy (display_buffer_line[2], "Inschakel.: ");
								strcpy (display_buffer_line[2]+13, tempstring);
							}
							break;
						}
					}
					break;
					case 1: // Pomp 2
					{
						strcpy (display_buffer_line[1], "     < Pomp 2       ");

						// Submenu C Boundary
						if (cursor_stateC_temp > 1) { cursor_stateC_temp = 1; }
						if (cursor_stateC_temp < 0) { cursor_stateC_temp = 0; }		
						switch (cursor_stateC_temp) 
						{
							case 0: // Looptijd
							{
								char tempstring[5];
								itoa (z_pomp_looptijd[1], tempstring, 10);
								strcpy (display_buffer_line[2], "Looptijd: ");
								strcpy (display_buffer_line[2]+11, tempstring);
							}
							break;
							case 1: // Aantal inschakelingen
							{
								char tempstring[5];
								itoa (z_pomp_inschakelingen[1], tempstring, 10);
								strcpy (display_buffer_line[2], "Inschakel.: ");
								strcpy (display_buffer_line[2]+13, tempstring);
							}
							break;
						}
					}
					break;
				}
			break;
		}
		
		_delay_ms(10);
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

