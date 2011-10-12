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
	*(s_pomp_active+0) = 0;
	*(s_pomp_active+1) = 0;
	f_eeprom_looptijd
}

/** 
 * fuctie start pomp 
 * @param s_pomp_active array van actieve pompen
 * @param z_pomp_looptijd array met looptijden van beide pompen
 * @param t_looptijd timer van maximale looptijd van de pomp
 * @param s_pomp_error staat er een pomp in error
 */

void f_start_pomp(int * s_pomp_active,const long int * z_pomp_looptijd,int * t_looptijd,int * s_pomp_error)
{
	// de error ding moet er nog bij	
	if (*(z_pomp_looptijd+0) <= *(z_pomp_looptijd+1))
	{
		*(s_pomp_active+0) = 1;
	}
	else 
	{
		*(s_pomp_active+1) = 1;	
	}
	*(t_looptijd) = 0;    /* er moet nog een functie start timer gemaakt worden */
	/* f_starttimer (maakt gebruik van hardware timer's en interupts*/
	
}
#endif
