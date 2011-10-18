#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "variables.h"
#include "functions_fsm.h"
#include "hardware_declarations.h"

int main(void) {

	init_pins();
	
	s_pomp_active[0] = 1;
	s_pomp_active[1] = 1;
	printf("%d - %d\n", s_pomp_active[0], s_pomp_active[1]);

	f_pomp_seterror ((int*) &s_pomp_error
	f_stop_pomp((int*) &s_pomp_active);
	
printf("%d - %d\n", s_pomp_active[0], s_pomp_active[1]);

	return 0;
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

void f_pomp_seterror (*(int s_pomp_error), *(const int e_motor_fase), *(const int e_motor_stroom), *(const int e_motor_temp), *(const int b_handauto), *(const int b_reset) )

{
	if ((*(e_motor_fase+0) = 1 )|| ((*(e_motor_stroom+0) = 1 ) || (*(e_motor_temp+0) = 1)  || (*(b_handauto+0) = 1))
	{
	*(s_pomp_error+0) = 1; /*als fase of stroom of temp verkeerd is of motor staat in hand dan wordt de error variablele geset*/
	}
	if ((*(e_motor_fase+1) = 1) || (*(e_motor_stroom+1) = 1) || (*(e_motor_temp+1) = 1) || (*(b_handauto+1) = 1))
	{
	*(s_pomp_error+1) = 1; /*als fase of stroom of temp verkeerd is of motor staat in hand dan wordt de error variablele geset*/
	}
	if ((*(b_reset+0) = 1) && (*(e_motor_fase+0) = 0) && (*(e_motor_stroom+0) = 0) && (*(e_motor_temp+0) = 0) && (*(b_handauto+0) = 0))
	{
	*(s_pomp_error+0) = 0; /* geen error wel reset error melding wordt 0*/
	}	
	if ((*(b_reset+1) = 1) && (*(e_motor_fase+1) = 0) && (*(e_motor_stroom+1) = 0) && (*(e_motor_temp+1) = 0) && (*(b_handauto+1) = 0))
	{
	*(s_pomp_error+1) = 0; /* geen error wel reset error melding wordt 0*/
	}	

}

