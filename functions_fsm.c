/** \file functions_fsm.c
 * Functies die worden aangeroepen vanuit het FSM
 *
 */


#ifndef f

/** 
 * fuctie stop pomp 
 * @param a_pomp_active array van actieve pompen
 */

void f_stop_pomp(int * a_pomp_active) 
{
<<<<<<< HEAD
	/* f_eeprom_looptijd; */
	*(s_pomp_active+0) = 0;
	*(s_pomp_active+1) = 0;
=======
	f_eeprom_looptijd;
	*(a_pomp_active+0) = 0;
	*(a_pomp_active+1) = 0;
>>>>>>> aece3b5f72de1d54e81513651e1e867d546c8112

}

/** 
 * fuctie start pomp 
 * @param start allen een pomp zonder error, degene met de minste draai uren
 * @param a_pomp_active array van actieve pompen
 * @param z_pomp_looptijd array met looptijden van beide pompen
 * @param t_looptijd timer van maximale looptijd van de pomp
 * @param s_pomp_error staat er een pomp in error
 */

void f_start_pomp(int * a_pomp_active,const long int * z_pomp_looptijd,int * t_looptijd,const int * s_pomp_error)
{
	*(t_looptijd) = 0; 
			if (*(z_pomp_looptijd+0) <= *(z_pomp_looptijd+1) || (* s_pomp_error+1 < 1) && (* s_pomp_error+0 < 1))
			{
				*(a_pomp_active+0) = 1;
			}								/*start pomp 1*/
			if (*(z_pomp_looptijd+0) > *(z_pomp_looptijd+1) || (* s_pomp_error+0 < 1) && (* s_pomp_error+1 < 1))
			{
				*(a_pomp_active+1) = 1;	
			}								/*start pomp 2*/
				else;  /*allebij in error, geen pomp start*/
<<<<<<< HEAD
	/* f_eeprom_inschakelingen */
=======
	f_eeprom_inschakelingen
}

>>>>>>> aece3b5f72de1d54e81513651e1e867d546c8112
	  /* er moet nog een functie start timer gemaakt worden */
	/* f_starttimer (maakt gebruik van hardware timer's en interupts*/




/**
 *functie f_eeprom_looptijd onthoud de looptijded 
 * @param t_looptijd huidige looptijd actieve pomp
 * @param z_pomp_looptijd looptijden van bijde pompen
 * @param s_pomp_error array van actieve pompen
 * @param a_pomp_active welke pomp loopt
 */

void f_eeprom_looptijd (const int * a_pomp_active,long int * z_pomp_looptijd,int * t_looptijd)
{	
	if (*(a_pomp_active+0) = 1 && *(t_looptijd) > 0)
	{
	*(z_pomp_looptijd+0) = *(t_looptijd+0) + *(z_pomp_looptijd+0); 
	}
	if (*(a_pomp_active+1) = 1 && *(t_looptijd) > 0)
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
 *functie f_hoogwater_alarm set hoogwateralarm
 * s_hoogwater hoogwater gedetecteerd
 * s_hoogwateralarm hoogwater alarm
 */

void f_hoogwater_alarm (const int * s_hoogwater,int * a_hoogwateralarm)
{
	if (*(s_hoogwater) = 1)
	{
	*(a_hoogwateralarm) = 1
	} 
}



/**
 *functie f_hoogwater_alarm set hoogwateralarm
 * s_hoogwateralarm hoogwater alarm
 * 
 */

void f_reset_hoogwater_alarm (int * s_hoogwateralarm)
{
	*(a_hoogwateralarm) = 0 
}



/**
 *functie f_start_nadraaitimer start eerste nadraaitimer
 * 
 */

void f_start_nadraaitimer (/*???????*/)
{
	// eerst de timer resetten	
	// hier moet de timer gestart worden
}


/**
 *functie f_start_nadraai2timer start eerste nadraaitimer
 * 
 */

void f_start_nadraai2timer (/*???????*/)
{
	// eerst de timer resetten	
	// hier moet de timer gestart worden
}




#endif




