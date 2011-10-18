/** \file functions_fsm.c
 * Functies die worden aangeroepen vanuit het FSM
 *
 */


#ifndef f

/** 
 * fuctie stop pomp 
 * @param s_pomp_active array van actieve pompen
 */

void f_stop_pomp(int * s_pomp_active) 
{
	f_eeprom_looptijd;
	*(s_pomp_active+0) = 0;
	*(s_pomp_active+1) = 0;

}

/** 
 * fuctie start pomp 
 * @param start allen een pomp zonder error, degene met de minste draai uren
 * @param s_pomp_active array van actieve pompen
 * @param z_pomp_looptijd array met looptijden van beide pompen
 * @param t_looptijd timer van maximale looptijd van de pomp
 * @param s_pomp_error staat er een pomp in error
 */

void f_start_pomp(int * s_pomp_active,const long int * z_pomp_looptijd,int * t_looptijd,const int * s_pomp_error)
{
	*(t_looptijd) = 0; 
			if (*(z_pomp_looptijd+0) <= *(z_pomp_looptijd+1) || (* s_pomp_error+1 < 1) && (* s_pomp_error+0 < 1))
			{
				*(s_pomp_active+0) = 1;
			}								/*start pomp 1*/
			if (*(z_pomp_looptijd+0) > *(z_pomp_looptijd+1) || (* s_pomp_error+0 < 1) && (* s_pomp_error+1 < 1))
			{
				*(s_pomp_active+1) = 1;	
			}								/*start pomp 2*/
				else;  /*allebij in error, geen pomp start*/
	f_eeprom_inschakelingen
	  /* er moet nog een functie start timer gemaakt worden */
	/* f_starttimer (maakt gebruik van hardware timer's en interupts*/




/**
 *functie f_eeprom_looptijd onthoud de looptijded 
 * @param t_looptijd huidige looptijd actieve pomp
 * @param z_pomp_looptijd looptijden van bijde pompen
 * @param s_pomp_error array van actieve pompen
 * 
 */

void f_eeprom_looptijd (const int * s_pomp_active,long int * z_pomp_looptijd,int * t_looptijd)
{	
	if (*(s_pomp_active+0) = 1 && *(t_looptijd) > 0)
	{
	*(z_pomp_looptijd+0) = *(t_looptijd+0) + *(z_pomp_looptijd+0); 
	}
	if (*(s_pomp_active+1) = 1 && *(t_looptijd) > 0)
	{	
	*(z_pomp_looptijd+1) = *(t_looptijd) + *(z_pomp_looptijd+1); 
	}
		else;
	*(t_looptijd) = 0;
/* sla z_pomp_looptijd op op het eeprom */
}

/**
 *functie f_eeprom_inschakelingen onthoud het aantal inschakelingen 
 * t_looptijd huidige looptijd actieve pomp
 * z_pomp_inschakelingen array looptijden van bijde pompen
 * s_pomp_error array van actieve pompen
 * 
 */

void f_eeprom_looptijd (const int * s_pomp_active,int * z_pomp_inschakelingen)
{
	if (*(s_pomp_active+0) = 1)
	{
	*(z_pomp_inschakelingen+0)++
	} 
	if (*(s_pomp_active+1) = 1)
	{
	*(z_pomp_inschakelingen+1)++
	}
		else;
/* sla z_pomp_inschakelingen op op het eeprom */
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

void f_pomp_seterror (*(int s_pomp_error), *(const int e_motor_fase), *(const int e_motor_stroom), *(const int e_motor_temp), *(const int b_inschakelingHand), *(const int b_reset) )

{
	if (*(e_motor_fase+0) = 1 || (*(e_motor_stroom+0)) = 1  || *(e_motor_temp+0) = 1  || (*(b_handauto+0) = 1)
	{
	*(s_pomp_error+0) = 1;
	}
	if (*(e_motor_fase+1) = 1 || (*(e_motor_stroom+1) = 1 || *(e_motor_temp+1) = 1 || *(b_handauto+1) = 1)
	{
	*(s_pomp_error+1) = 1;
	}
		else;

	if (*(b_reset+0) = 1)
	{
	*(s_pomp_error+0) = 0;
	*(b_reset+0) = 0;
	}	
	if (*(b_reset+1) = 1)
	{
	*(s_pomp_error+1) = 0;
	*(b_reset+1) = 0;
	}	
		else;
}









#endif




