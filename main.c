#include <stdio.h>
#define TRUE 1;
#define FALSE 0;




/* 
	Error Variables test randy 123
	Prefix: e_


bla bla blaaaaaaaa

*/

int e_nivo_breuk = FALSE;							 /* Nivosensor voelerbreuk P1	Bool Active 1 */
int e_nivo_kortsluiting = FALSE; 			 /* Nivosensor kortsluiting	Bool Active 1 */
int e_motor_fase[2] = {FALSE, FALSE};	 /* Motorfase verkeerd (per pomp)	Array Bool Active 1 */
int e_motor_stroom[2] = {FALSE, FALSE};/* Motorstroom te hoog (per pomp)	Array Bool Active 1 */
int e_motor_temp[2] = {FALSE, FALSE};	 /* Motortemp te hoog	Array Bool Active 1 */
int e_hoogwater = FALSE; 							 /* Hoogwatermelding	Bool Active 1 */

/* 
	Log variables
	Prefix: z_
*/

/* Te doen: initializatie halen uit EEPROM  */
/* Te doen: rekening houden met looptijd, 65000 minuten is te kort */

long int z_pomp_looptijd[2];				/* Totale looptijd per pomp (in min.)	double (1-65000) */
long int z_pomp_inschakelingen[2];	/* Totaal aantal inschakelingen per pomp	uINT (0-255) */

/*
	Sensor variables (input)
	Prefix: s_
*/

int s_nivo;					/* Waarde nivosensor	uINT (0-1023) */
int s_motorPower[2];/* Motor Overbelast	Bool Active 1 */
int s_motorTemp[2]; /* Motor Oververhit	Bool Active 1 */
int s_motorFase[2]; /* Motor Fase verkeerd	Bool Active 0 (Of 1?) */
int s_hoogwater;		/*Hoogwater melding	Bool Active 1 */

/*
	Bediening variables (input)
	Prefix: b_
*/

b_handAuto[0-1] 2x	Handmatig of automatische stand (per pomp)	Array Bool
0=hand
1=auto
b_inschakelingHand[0-1] 2x	Inschakeling hand (per pomp)	Array Bool
Active 1
b_reset[0-1] 2x	Reset (per pomp)	Array Bool
Active 1
b_cursur_up	cursur knoppen	array bool
active1
b_cursur_down	cursur knoppen	array bool
active1
b_cursur_left	cursur knoppen	array bool
active1
b_cursur_right	cursur knoppen	array bool
active1

5.3.5 	Custom variables (input)
Prefix: c_

Variable	Meaning	Datatype	Bijbehorend T variabele (timer)
c_nivo_bovenste	Bovenste inschakelnivo	uINT (in cm)	
c_nivo_onderste	Onderste inschakelnivo	uINT (in cm)	
c_nivo_uitschakel	UItschakelnivo	CONST uINT (4 mA)	
c_nadraai	Nadraaitijd	uINT (3-10 sec)	t_nadraai (sec)
c_nadraai2	Nadraaitijd bij hoogwatersensor	uINT (tot 90sec)	t_nadraaitijd2
c_looptijd	Maximale looptijd	uINT (10-15 min)	t_looptijd (min)
c_idnummer	id nummer van de pomp	uINT(0-65000)	





5.3.6 	Set variables (Output/Actuator)8
Prefix: s_

Variable	Meaning	Datatype
s_pomp_active[0-1]	Pomp actief/in bedrijf	Bool (1=active)
s_pomp_error[0-1]	Pomp in storing?	Bool (1=active)
zie e_hoogwater	Hoogwatermelding	
s_error	Algemene foutmelding	Bool (1=active)
s_standby	Standby melding	Bool (1=active)
