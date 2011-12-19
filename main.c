#define F_CPU (8000000UL)

#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h> // Voor strlen()
#include <stdlib.h> // Voor itoa()
#include <stdio.h> // Voor sprintf()

#include "display.h"
#include "shiftin.h"
#include "pin_driver.h"
#include "timer.h"
#include "adc.h"
#include "usart_driver.h"

#define TRUE 1
#define FALSE 0

#define NIVO_BREUK 1
#define NIVO_KORTSLUITING 1000
#define SERIAL_TIMEOUT_CYCLES 30 // Main loop cycles before serial_state is reset to zero


///////////////////////////////////////////////////////////////////////
/*

Legenda:
    - te doen
    * mee bezig
    X af
	H Hold up

TODO
 *	 Pijltjes in menu bij logs pomp1 en pomp2
X    nivosensor storing
X    Storingen menu
X    Privilege systeem (beheerder/normaal nivo). Toetsencombi (cursor up/down) zorgt voor beheerder modus. Timeout na X tijd.
X    Cursor duidelijker huidig menu
X    Polling fix
X	 Fix hand auto blijft actief
X    Pompen omstebeurt
X    Pomp knippert bij error in state normaal (state 4 naar 6)
X    Nivo error state (hoog?) - gaat automatisch adhv hoogwater sensor
X	 Pomp pas uit storing als hand/auto schakelaar op auto staat
X	 Pomp in storing zetten als hand/auto schakelaar voor pomp ingeschakeld is (automatisch word dan de andere pomp ingeschakeld door de state machine)
X	 Algemene foutmelding pas resetten als beide pompen niet in storing zijn
X    Serial protocol
H	 bij functie start_pomp gaat de pomp aan en direct weer uit
-	 Sla laatste 3 foutmeldingen op in eeprom
-	 Schakelaar hand/auto moet een toggle zijn, niet inhouden, storing ledje van pomp laten knipperen ter indicatie
-   max looptijd implementeren (pomp in storing zetten, andere pomp word automatisch ingeschakeld door state machine)
-	 Schakelaar hand/auto moet een toggle zijn, niet inhouden, storing ledje van pomp laten knipperen ter indicatie

*/
/////////////////////////////////////////////////////////////////////// 

// State machine pomp
int statehoog = 1;
int statenormaal = 1;

// State voor serial protocol
int serialstate = 0;
int serialstate_timeout = 0; // State timeout (after timeout reset serialstate to zero)
int serialstate_counter = 0; //Count serial bytes
int serialstate_vars[20];

// Display buffer pointers
char display_buffer[86]; // 86 voor elke \0 terminator
char * display_buffer_line[4];

char temp_p1[4]; // Pomp 1 in vierde regel
char temp_p2[4]; // Pomp 2 in vierde regel
char temp_error[6]; // Error in vierde regel

int prevent_infinity = 0; // Voorkom oneindige lus tussen f_start_pomp_1 en _2 !
int last_active_pomp = 0;

// 3 last error vars
int last_3_errors[3];

// Error vars
int e_nivo_breuk = FALSE; // Nivosensor voelerbreuk P1
int e_nivo_kortsluiting = FALSE; // Nivosensor kortsluiting

// Log vars
uint16_t z_pomp_looptijd[2]; // Totale looptijd per pomp (in min.)double (1-65000) 
uint16_t z_pomp_inschakelingen[2]; // Totaal aantal inschakelingen per pomp	uINT (0-255) 

// Sensor vars (input)
int s_nivo;	// Waarde nivosensor (0-1023)
int s_motor_stroom[2]; // Motor Overbelast
int s_motor_temp[2]; // Motor Oververhit 
int s_motor_fase[2]; // Motor Fase verkeerd
int s_hoogwater; // Hoogwater melding

// Bediening vars (input)
int b_hand_auto[2]; // 2x Handmatig of automatische stand (per pomp)
int b_inschakeling_hand[2]; // 2x Inschakeling hand (per pomp)
int b_reset[2]; // 2x Reset (per pomp)
int b_cursor_up; //	cursur knoppen	
int b_cursor_down; // ..
int b_cursor_left; // ..
int b_cursor_right; // ..

// Custom variables (input)
unsigned int c_nivo_bovenste; // Bovenste inschakelnivo	uINT (in cm) 
unsigned int c_nivo_onderste; // Onderste inschakelnivo	uINT (in cm) 
unsigned int c_nivo_uitschakel;	// UItschakelnivo	CONST uINT (4 mA) 
unsigned int c_nadraai; // Nadraaitijd	uINT (3-10 sec)	t_nadraai (sec) 
unsigned int c_nadraai_hoogwater; // Nadraaitijd bij hoogwatersensor uINT (tot 90sec)
unsigned int c_looptijd;  // Maximale looptijd	uINT (10-15 min) t_looptijd (min) 
unsigned int c_idnummer; //	id nummer van de pomp	uINT(0-65000) 

// Timer variables
int t_nadraai; 
int t_nadraai_hoogwater;
int t_looptijd;

// Set variables (Output/Actuator)
int a_pomp_active[2]; // Pomp actief/in bedrijf	
int a_pomp_error[2]; // Pomp in storing
int a_error; // Algemene foutmelding
int a_standby; // Standby melding
int a_hoogwateralarm;

// EEPROM aliases
uint16_t EEMEM eeprom_z_pomp_looptijd_1 = 0;
uint16_t EEMEM eeprom_z_pomp_looptijd_2 = 0;
uint16_t EEMEM eeprom_z_pomp_inschakelingen_1 = 0;
uint16_t EEMEM eeprom_z_pomp_inschakelingen_2 = 0;

uint8_t EEMEM eeprom_c_nivo_bovenste = 200;
uint8_t EEMEM eeprom_c_nivo_onderste = 100;
uint8_t EEMEM eeprom_c_nivo_uitschakel = 50;	
uint8_t EEMEM eeprom_c_nadraai = 3; 
uint8_t EEMEM eeprom_c_nadraai_hoogwater = 10; 
uint8_t EEMEM eeprom_c_looptijd = 5; 
uint8_t EEMEM eeprom_c_idnummer = 99; 

// Lees eeprom en zet global variabelen
void setvars_from_eeprom(void) 
{
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

void setvars_shiftregister(void) 
{
	int shift[17];
	shiftregister_read(&shift[0], 2);

	// Shift register 1
	b_cursor_down = shift[0]; // Pin 14 (1)
	b_cursor_up = shift[1]; // Pin 12 (1)
	b_reset[1] = shift[2]; // Pin 11 (1)
	b_inschakeling_hand[1] = shift[3]; // Pin 10 (1)
	b_hand_auto[1] = shift[4]; // Pin 5 (1)
	b_reset[0] = shift[5]; // Pin 4 (1)
	b_inschakeling_hand[0] = shift[6]; // Pin 3 (1)
	b_hand_auto[0] = shift[7]; // Pin 2 (1)

	// Shift register 2
	s_motor_temp[1] = shift[8]; // Pin 14 (2)
	s_motor_stroom[1] = shift[9]; // Pin 12 (2)
	s_motor_fase[1] = shift[10]; // Pin 11 (2)
	s_motor_temp[0] = shift[11]; // Pin 10 (2)
	s_motor_stroom[0] = shift[12]; // Pin 5 (2)
	s_motor_fase[0] = shift[13]; // Pin 4 (2)
	b_cursor_right = shift[14]; // Pin 3 (2)
	b_cursor_left = shift[15]; // Pin 2 (2)

	s_hoogwater = (PINA & (1 << PA1)) && 1;
}

// Routine die vaker word aangeroepen in de display state machine (voor de derde regel te setten, min/max waardes te zetten en de event code in te stellen)
void derde_regel_instellingen (char * tweede_regel, int event_code_i, uint8_t * custom_var, int * custom_temp, int min_val, int max_val, char * derde_regel, int * e_event_code, char * e_display_buffer_1, char * e_display_buffer_2) 
{
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

// Set pins
void setvars_actuators(void) 
{
	if (a_pomp_active[0] != 0) 
	{
		pindriver_setpin("C", PC1, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC1, 0);
	}
	
	if (a_pomp_active[1] != 0) 
	{
		pindriver_setpin("C", PC2, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC2, 0);
	}

	if (a_pomp_error[0] != 0) 
	{
		pindriver_setpin("C", PC3, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC3, 0);
	}

	if (a_pomp_error[1] != 0) 
	{
		pindriver_setpin("C", PC4, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC4, 0);
	}

	if (a_hoogwateralarm != 0) 
	{
		pindriver_setpin("C", PC5, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC5, 0);
	}

	if (a_error != 0) 
	{
		pindriver_setpin("C", PC6, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC6, 0);
	}

	if (a_standby != 0) 
	{
		pindriver_setpin("C", PC7, 1);
	} 
	else 
	{
		pindriver_setpin("C", PC7, 0);
	}
}

/** 
 * Stop pomp 
 */
void f_stop_pomp(void) 
{
	// Check welke pomp in bedrijf 
	if (a_pomp_active[0] != 0) // Pomp 1 in bedrijf
	{
		eeprom_write_word(&eeprom_z_pomp_looptijd_1, z_pomp_looptijd[0] + t_looptijd);
		a_pomp_active[0] = 0; // Zet pomp 1 uit
	} 
	if (a_pomp_active[1] != 0)  // Pomp 2 in bedrijf
	{
		eeprom_write_word(&eeprom_z_pomp_looptijd_2, z_pomp_looptijd[1] + t_looptijd);
		a_pomp_active[1] = 0; // Zet pomp 2 uit
	}
}

void f_start_pomp_1(void)
{
	prevent_infinity++;

	if (a_pomp_error[0] == 0 && b_hand_auto[0] == 0) // Pomp 1 niet in error en pomp 1 in auto stand
	{
		a_pomp_active[0] = 1;
		t_looptijd = 0; // Reset timer om looptijd pomp bij te houden
		eeprom_write_word(&eeprom_z_pomp_inschakelingen_1, z_pomp_inschakelingen[0] + 1);
		last_active_pomp = 0;
	} 
	else 
	{
		if (prevent_infinity <= 2) 
		{
			f_start_pomp_2();
		}
	}
}

void f_start_pomp_2(void)
{
	prevent_infinity++;

	if (a_pomp_error[1] == 0 && b_hand_auto[1] == 0) // Pomp 2 niet in error en pomp 2 in auto stand
	{
		a_pomp_active[1] = 1;
		t_looptijd = 0; // Reset timer om looptijd pomp bij te houden
		eeprom_write_word(&eeprom_z_pomp_inschakelingen_2, z_pomp_inschakelingen[1] + 1);
		last_active_pomp = 1;
	} 
	else 
	{
		if (prevent_infinity <= 2) 
		{
			f_start_pomp_1();
		}
	}
}

/** 
 * functie start pomp 
 */
void f_start_pomp(void)
{
	prevent_infinity = 0;
	// Start pomp welke niet in storing is en minste looptijd. Als allebei in error, geen pomp starten
	if (last_active_pomp == 1) // Pomp 2 heeft als laatste gelopen, pomp 1 inschakelen
	{
		f_start_pomp_1();
	}
	else // Pomp 2 inschakelen
	{
		f_start_pomp_2();
	}
}


/**
 * Handmatige bediening pompen
 */
void f_pomp_handmatig(void) 
{
	if (b_hand_auto[0] != 0) // Pomp 1 in handmatig stand
	{
		if (b_inschakeling_hand[0] == 0) // Pomp 1 uit
		{
			a_pomp_active[0] = 0;
		} 
		else // Pomp 1 aan
		{ 
			a_pomp_active[0] = 1;
		}
	} else {
		a_pomp_active[0] = 0;
	}
	if (b_hand_auto[1] != 0) // Pomp 2 in handmatig stand
	{
		if (b_inschakeling_hand[1] == 0) // Pomp 2 uit
		{
			a_pomp_active[1] = 0;
		} 
		else // Pomp 2 aan
		{ 
			a_pomp_active[1] = 1;
		}
	} else {
		a_pomp_active[1] = 0;
	}
}

/**
 * Schuif eerste 2 elementen op in last_3_errors array en zet error_code op eerste plaats
 * @param[in] error code welke op index 0 komt te staan in de array
 * @param[out] last_3_errors[]
 */
void f_last_3_errors_array(int error_code)
{
	// Alleen error_code plaatsen in array als deze niet al voorkomt in de array
	if (last_3_errors[0] != error_code && last_3_errors[1] != error_code && last_3_errors[2] != error_code) {   
		last_3_errors[2] = last_3_errors[1];
		last_3_errors[1] = last_3_errors[0];
		last_3_errors[0] = error_code;
	}
}

char error_string[20];

void error_message(int error_code)
{
	switch (error_code)
	{
		case 1: strcpy (error_string, "Nivo breuk"); break;
		case 2: strcpy (error_string, "Nivo kortsluiting"); break;
		case 3: strcpy (error_string, "Pomp 1 stroom"); break;
		case 4: strcpy (error_string, "Pomp 2 stroom"); break;
		case 5: strcpy (error_string, "Pomp 1 temp"); break;
		case 6: strcpy (error_string, "Pomp 2 temp"); break;
		case 7: strcpy (error_string, "Pomp 1 fase"); break;
		case 8: strcpy (error_string, "Pomp 2 fase"); break;
		case 9: strcpy (error_string, "Hoogwater"); break;
		default: strcpy (error_string, "                   "); break;
	}
}

void f_last_3_errors(void)
{
	int error_code = 0; // error_code voor iedere statement

	// Ga iedere error af en kijk of deze geset is. Zo ja, plaatsen in last_3_errors array als het eerste element van deze array niet gelijk is aan de foutmelding (preventie dubbele meldingen). Word er voldaan aan deze conditie dan middels functie de error in de array plaatsen en de eerste 2 eerst opschuiven (doet aparte functie).
	error_code = 1;
	if (e_nivo_breuk != 0) { f_last_3_errors_array(error_code); }
	error_code = 2;
	if (e_nivo_kortsluiting != 0) { f_last_3_errors_array(error_code); }
	error_code = 3;
	if (s_motor_stroom[0] != 0) { f_last_3_errors_array(error_code); }
	error_code = 4;
	if (s_motor_stroom[1] != 0) { f_last_3_errors_array(error_code); }
	error_code = 5;
	if (s_motor_temp[0] != 0) { f_last_3_errors_array(error_code); }
	error_code = 6;
	if (s_motor_temp[1] != 0) { f_last_3_errors_array(error_code); }
	error_code = 7;
	if (s_motor_fase[0] != 0) { f_last_3_errors_array(error_code); }
	error_code = 8;
	if (s_motor_fase[1] != 0) { f_last_3_errors_array(error_code); }
	error_code = 9;
	if (s_hoogwater != 0) { f_last_3_errors_array(error_code); }
}

void f_pomp_seterror(void) 
{
	// Alleen een reset mag de storing per pomp laten verwijderen
	if (b_reset[0] != 0) 
	{
		a_pomp_error[0] = 0;
	}
	if (b_reset[1] != 0) 
	{
		a_pomp_error[1] = 0;
	}

	// Als een pomp een van de drie storingen heeft, a_pomp_error setten
	if ((s_motor_fase[0] != 0) || (s_motor_stroom[0] != 0) || (s_motor_temp[0] != 0) || (b_hand_auto[0] != 0)) 
	{
		a_pomp_error[0] = 1;
		a_error = 1; // Set algemene foutmelding
		a_standby = 0;
	}
	if ((s_motor_fase[1] != 0) || (s_motor_stroom[1] != 0) || (s_motor_temp[1] != 0) || (b_hand_auto[1] != 0))
	{
		a_pomp_error[1] = 1;
		a_error = 1;
		a_standby = 0;
	}

	if (a_pomp_error[0] == 0 && a_pomp_error[1] == 0) // Standby melding als geen pomp in storing is
	{
		a_standby = 1;
		a_error = 0; // Reset algemene foutmelding
	}
}

/**
 *functie f_pomp_seterror zet pomp in error als dat nodig is 
 * @param a_pomp_error	array van pomp in error
 * @param e_motor_fase array motor fase verkeerd
 * @param e_motor_stroom array motor stroom verkeerd
 * @param e_motor_temp array motor tempratuur verkeerd
 * @param b_handauto array motor op handmatig
 * @param b_reset array van resetknoppen
 */

void f_update_status_vars(void) 
{
	if (a_pomp_active[0] != 0) 
	{
		strcpy (temp_p1, "aan");
	}
	else
	{
		strcpy (temp_p1, "uit");
	}

	if (a_pomp_active[1] != 0) 
	{
		strcpy (temp_p2, "aan");
	}
	else
	{
		strcpy (temp_p2, "uit");
	}

	if (a_error != 0)
	{
		strcpy (temp_error, "ja");
	}
	else
	{
		strcpy (temp_error, "geen");
	}
}

void state_machine(void)
{
	f_pomp_handmatig(); // Handmatige besturing pompen
	adc_read(); // Read ADC s_nivo
	if (NIVO_BREUK > s_nivo) // Nivo breuk
	{
		e_nivo_breuk = TRUE;
		e_nivo_kortsluiting = FALSE;
	} 
	else if (NIVO_KORTSLUITING < s_nivo) // Nivo kortsluiting
	{ 
		e_nivo_kortsluiting = TRUE;
		e_nivo_breuk = FALSE;
	}
	else // Reset breuk en kortsluiting
	{
		e_nivo_breuk = FALSE;
		e_nivo_kortsluiting = FALSE;
	}

	f_pomp_seterror(); // Zet pompen in error als dat nodig is
	f_update_status_vars();

	sprintf(display_buffer_line[3], "P1:%s P2:%s E:%s", temp_p1, temp_p2, temp_error);
	display_line(display_buffer_line[3],3); // Update vierde regel
	setvars_actuators(); // Set actuators according to globals

	f_last_3_errors();

	switch  (statehoog)
	{
		case 1: /* check hoogwater */
			if (s_hoogwater == 0)
			{
				a_hoogwateralarm = 0; /* reset hoogwater alarm */

				///////////////
				// State normal
				///////////////
				switch (statenormaal)
				{
					case 1: /*pomp uit*/
						if (s_nivo > c_nivo_onderste)
						{
							statenormaal = 2;
							f_start_pomp();
						}
					break;
				
					case 2: 	/*pompaan*/
						if (s_nivo <= c_nivo_uitschakel)
						{
							statenormaal = 3;
							t_nadraai = 0; // Start nadraaitimer
						}
						if (a_error != 0) 
						{
							statenormaal = 4;
							f_stop_pomp();
						}
					break;

					case 3:	/*pomp nadraai*/
						if (t_nadraai > (c_nadraai)) // * 60
						{
							statenormaal = 1;
							f_stop_pomp();
						}
						if (a_error != 0) 
						{
							statenormaal = 4;
							f_stop_pomp();
						}
					break;

					case 4:	/*pomp in error*/
						if (s_nivo < c_nivo_onderste)
						{
							statenormaal = 5;
						}
						if (s_nivo > c_nivo_onderste)
						{
							statenormaal = 6;	
							f_start_pomp();
						}
					break;

					case 5:	/*wacht op nivo*/
						if (s_nivo > c_nivo_bovenste)
						{
							statenormaal = 6;
							f_start_pomp();
						}
					break;

					case 6:	/*pomp aan (direct)*/
						if (s_nivo <= c_nivo_uitschakel)
						{
							statenormaal = 1;
							f_stop_pomp();
						}
					break;
				}
				///////////////////
				// End State normal
				///////////////////
			}
			else /* hoogwater sensor != 0 */
			{
				statehoog = 2;
				a_hoogwateralarm = 1; //s_hoogwater_alarm;
				f_start_pomp();
				t_nadraai_hoogwater = 0; // Start nadraai hoogwater timer
			}			
		break;

		case 2: /* hoogwater */		
			if (b_reset[0] != 0 || b_reset[1] != 0)
			{
				statehoog = 1;
				statenormaal = 1;
				f_stop_pomp();
			}
			if (t_nadraai_hoogwater > (c_nadraai_hoogwater)) // * 60	
			{
				statehoog = 4;
				f_stop_pomp();
			}
			if (a_error != 0)
			{
				statehoog = 3;
				f_stop_pomp();
				t_nadraai_hoogwater = 0; // Start nadraai hoogwater timer
				f_start_pomp();
			}
		break;

		case 3: /*pomp restart*/		
			if (b_reset[0] != 0 || b_reset[1] != 0)
			{
				statehoog = 1;	
				statenormaal = 1;
				f_stop_pomp();
			}
			if (t_nadraai_hoogwater > (c_nadraai_hoogwater)) // * 60
			{
				statehoog = 4;
				f_stop_pomp();
			}
		break;

		case 4: /*pomp uit*/		
			if (b_reset[0] != 0 || b_reset[1] != 0)
			{
				statehoog = 1;	
				statenormaal = 1;
				f_stop_pomp();
			}
			if (s_hoogwater != 0)
			{
				statehoog = 2;
				a_hoogwateralarm = 1;
				f_start_pomp();		
				t_nadraai_hoogwater = 0; // Start nadraai hoogwater timer
			}
		break;
	}
}

int main(void) {
	_delay_ms (10); // Safeguard

	// Init drivers
	setvars_from_eeprom(); // Load vars from eeprom into globals
	pindriver_init(); // Init pin driver
	timerdriver_init(); // Init timer driver
	adc_init(); // Init ADC
	usart_init(); // Init Usart

	// Init PA1 (hoogwater)
	DDRA &= ~(1 << PA1);

	// Init external peripheral
	display_init(); // Init display driver
	shiftregister_init(); // Init shiftregister driver

	// Welcome message
	display_line("====================",0);
	display_line("Project Allflowlift ",1);
	display_line("Firmware version 0.1",2);
	display_line("======= ES1V2 ======",3);
	_delay_ms (1000);

	// +1 is nodig om sterretje (actieve regel) niet te wijzigen iedere loop
	display_buffer_line[0] = (char *) &display_buffer + 1;
	display_buffer_line[1] = (char *) &display_buffer + 21 + 1;
	display_buffer_line[2] = (char *) &display_buffer + 43 + 1;
	display_buffer_line[3] = (char *) &display_buffer + 65; // Hier niet nodig, 4e regel kan niet geselecteerd worden

	// Init vars
	int event_submenuC = 0; // Simuleer event van submenu C naar submenu B (EEPROM SAVE)
	int event_code = 0; // Event code voor afhandeling

	int cursor_stateA = 0; // State machine 1e regel
	int cursor_stateB = 0; // State machine 2e regel
	int cursor_stateC_temp = 0; // State machine 3e regel (word alleen gebruikt voor waardes) 
	char affect_state = 'A'; // Cursor state machine

	int count_state_routine = 0; // State machine pompen counter

	int privilege_level = 0;
	int privilege_timeout = 0;

	// Infinite loop
	while (1) 
	{
		setvars_shiftregister(); // Set globals from input switches

		setvars_actuators();

		// Execute state machine pompen om een interval
		count_state_routine++;
		if (count_state_routine > 5)
		{
			count_state_routine = 0;
			state_machine();
		}

		// Serialstate timeout handler
		if (serialstate != 0)
		{
			serialstate_timeout++;
			if (serialstate_timeout > SERIAL_TIMEOUT_CYCLES)
			{
				serialstate = 0;
				serialstate_timeout = 0;
				serialstate_counter = 0;
			}
		}

		// Process event
		if (event_submenuC == 1) 
		{
			event_submenuC = 0; // Reset event
			switch (event_code) // Process event code
			{
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

		// Cursor state machine

		if (b_cursor_left != 0 && b_cursor_right != 0 && b_cursor_down != 0) // Privilege check
		{
			privilege_level = 1; 
			privilege_timeout = 0; // Reset timeout
		}
		else 
		{
			if (b_cursor_up != 0 || b_cursor_down != 0 || b_cursor_left != 0 || b_cursor_right != 0) // Reset privilege timeout if any cursor keys are pressed to prevent unexpected privilege drop
			{
				privilege_timeout = 0;
			}
			privilege_timeout++;
			if (privilege_timeout > 200)
			{
				privilege_timeout = 0;
				privilege_level = 0;
			}

			switch (affect_state)
			{
				case 'A':
					if (b_cursor_right != 0) 
					{
						cursor_stateA++;
					}
					if (b_cursor_left != 0) 
					{
						cursor_stateA--;
					}

					display_line(display_buffer_line[0]-1,0);
					display_line(display_buffer_line[1]-1,1);
					display_line(display_buffer_line[2]-1,2);
				break;
				case 'B':
					if (b_cursor_right != 0) 
					{
						cursor_stateB++;
						setvars_from_eeprom(); // Lees eeprom waardes 
					}
					if (b_cursor_left != 0) 
					{
						cursor_stateB--;
						setvars_from_eeprom(); // Lees eeprom waardes 
					}

					display_line(display_buffer_line[1]-1,1);
					display_line(display_buffer_line[2]-1,2);
				break;
				case 'C':
					if (b_cursor_right != 0) 
					{
						cursor_stateC_temp++;
					}
					if (b_cursor_left != 0) 
					{
						cursor_stateC_temp--;
					}

					display_line(display_buffer_line[2]-1,2);
				break;
			}

			if (b_cursor_down != 0)
			{
				affect_state++;
				if (affect_state == 'C') 
				{ 
					if (privilege_level == 0 && cursor_stateA == 0)
					{
						affect_state = 'B'; // Not allowed
					} 
					else // Allowed
					{
					cursor_stateC_temp = 0; // Reset van submenu B naar submenu C
					}
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
		} // End privilege check

		// Update cursor state machine
		switch (affect_state)
		{
			case 'A':
				*((char*)display_buffer_line[0]-1) = '*';
				*((char*)display_buffer_line[1]-1) = ' '; 
				*((char*)display_buffer_line[2]-1) = ' ';
			break;
			case 'B':
				*((char*)display_buffer_line[0]-1) = ' ';
				*((char*)display_buffer_line[1]-1) = '*'; 
				*((char*)display_buffer_line[2]-1) = ' ';

				display_line(display_buffer_line[0]-1,0);
			break;
			case 'C':
				*((char*)display_buffer_line[0]-1) = ' ';
				*((char*)display_buffer_line[1]-1) = ' '; 
				*((char*)display_buffer_line[2]-1) = '*';

				display_line(display_buffer_line[0]-1,0);
				display_line(display_buffer_line[1]-1,1);
			break;
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
				strcpy (display_buffer_line[0], "Instellingen      >");

				// Submenu B Boundary
				if (cursor_stateB > 6) { cursor_stateB = 6; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				setvars_from_eeprom(); // Lees eeprom waardes als men in het instellingen menu gaat
				switch (cursor_stateB)
				{
					case 0: // Looptijd bij vloeistofsensor
						derde_regel_instellingen ("Loopt. vl. sens.  >", 100, (uint8_t*) &c_looptijd, &cursor_stateC_temp, 1, 90, "Sec. (1-90): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 1: // bovenste inschakelnivo
						derde_regel_instellingen ("Bov. insch. nivo <>", 101, (uint8_t*) &c_nivo_bovenste, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 2: // onderste inschakelnivo
						derde_regel_instellingen ("Ond. insch. nivo <>", 102, (uint8_t*) &c_nivo_onderste, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 3: // uitschakel nivo
						derde_regel_instellingen ("Uitschakel nivo  <>", 103, (uint8_t*) &c_nivo_uitschakel, &cursor_stateC_temp, 0, 255, "Cm. (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 4: // nadraai inschakelnivo
						derde_regel_instellingen ("Nadraaitijd Pomp <>", 104, (uint8_t*) &c_nadraai, &cursor_stateC_temp, 3, 10, "Sec. (3-10): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 5: // nadraai bij hoogwater inschakelnivo
						derde_regel_instellingen ("Nadraai. Hoogwa. <>", 105, (uint8_t*) &c_nadraai_hoogwater, &cursor_stateC_temp, 1, 90, "Sec. (1-90): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					case 6: // Id nummer
						derde_regel_instellingen ("ID Nummer         <", 106, (uint8_t*) &c_idnummer, &cursor_stateC_temp, 0, 255, "ID (0-255): ", &event_code, display_buffer_line[1], display_buffer_line[2]);
					break;
					}
			break;
			case 1: // Storingsmeldingen menu
				strcpy (display_buffer_line[0], "Storingen        <>");
				strcpy (display_buffer_line[2], "                   ");

				// Submenu B Boundary
				if (cursor_stateB > 2) { cursor_stateB = 2; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				
				switch (cursor_stateB)
				{
					case 0: // Globale meldingen
						error_message(last_3_errors[0]);
						snprintf(display_buffer_line[1], 20, "1:%s",error_string);
					break;
					case 1: // Pomp 1 specifiek
						error_message(last_3_errors[1]);
						snprintf(display_buffer_line[1], 20, "2:%s",error_string);
					break;
					case 2: // Globale meldingen
						error_message(last_3_errors[2]);
						snprintf(display_buffer_line[1], 20, "3:%s",error_string);
					break;
				}
			break;
			case 2: // Logs menu
				strcpy (display_buffer_line[0], "Logs              <");

				// Submenu B Boundary
				if (cursor_stateB > 2) { cursor_stateB = 2; }
				if (cursor_stateB < 0) { cursor_stateB = 0; }		
				switch (cursor_stateB)
				{
					case 0: // Pomp 1
					{
						strcpy (display_buffer_line[1], "Pomp 1            >");

						// Submenu C Boundary
						if (cursor_stateC_temp > 1) { cursor_stateC_temp = 1; }
						if (cursor_stateC_temp < 0) { cursor_stateC_temp = 0; }		
						switch (cursor_stateC_temp) 
						{
							case 0: // Looptijd
							{
								sprintf(display_buffer_line[2], "Looptijd: %d", z_pomp_looptijd[0]);
							}
							break;
							case 1: // Aantal inschakelingen
							{
								sprintf(display_buffer_line[2], "Inschakel.: %d", z_pomp_inschakelingen[0]);
							}
							break;
						}
					}
					break;
					case 1: // Pomp 2
					{
						strcpy (display_buffer_line[1], "Pomp 2           <>");

						// Submenu C Boundary
						if (cursor_stateC_temp > 1) { cursor_stateC_temp = 1; }
						if (cursor_stateC_temp < 0) { cursor_stateC_temp = 0; }		
						switch (cursor_stateC_temp) 
						{
							case 0: // Looptijd
							{
								sprintf(display_buffer_line[2], "Looptijd: %d", z_pomp_looptijd[1]);
							}
							break;
							case 1: // Aantal inschakelingen
							{
								sprintf(display_buffer_line[2], "Inschakel.: %d", z_pomp_inschakelingen[1]);
							}
							break;
						}
					}
					break;
					case 2: // state machine status
					{
						strcpy (display_buffer_line[1], "FSM Status        <");

						snprintf(display_buffer_line[2], 20, "N:%d SH:%d SN:%d T:%d", s_nivo, statehoog, statenormaal, t_nadraai);
					}
					break;
				}
			break;
		}
	}	
}


