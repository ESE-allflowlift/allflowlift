#define F_CPU (10000000UL)

#include <avr/io.h>
#include <util/delay.h>


#include "functions_fsm.h"
#include "hardware_declarations.h"

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
int b_cursur_up; /*	cursur knoppen	array bool active1 */
int b_cursur_down; /* cursur knoppen	array bool active1 */
int b_cursur_left;	/* cursur knoppen	array bool active1 */
int b_cursur_right; /*	cursur knoppen	array bool active1 */

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

int main() {	
	init_pins();
	
	a_pomp_active[0] = 1;
	a_pomp_active[1] = 1;
	set_pomp (&a_pomp_active[0]);
	_delay_ms (500);

	a_pomp_active[0] = 0;
	a_pomp_active[1] = 0;
	set_pomp (&a_pomp_active[0]);
	_delay_ms (500);

	return 0;
}
/* 	printf("%d - %d\n", s_pomp_active[0], s_pomp_active[1]); */


/* 	f_pomp_seterror ((int*) &s_pomp_error */
	
/* printf("%d - %d\n", s_pomp_active[0], s_pomp_active[1]); */




/**
 *functie f_pomp_seterror zet pomp in error als dat nodig is 
 * @param s_pomp_error	array van pomp in error
 * @param e_motor_fase array motor fase verkeerd
 * @param e_motor_stroom array motor stroom verkeerd
 * @param e_motor_temp array motor tempratuur verkeerd
 * @param b_handauto array motor op handmatig
 * @param b_reset array van resetknoppen
 */

