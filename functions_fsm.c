/** \file functions_fsm.c
 * Functies die worden aangeroepen vanuit het FSM
 *
 */




/** 
 * fuctie fsm_motoren
 * @param fsm voor aansturen van de pompen  
 * @param statehoog de state waar de fsm in moet komen
 */

void fsm_motoren (void)
{
	
	switch  (statehoog)
	{
		case 1: /* check hoogwater */
			if (f_hoogwater == 0)
			{
				fsm_normaal;
				f_reset_hoogwater_alarm;
			}
			else
			{
				statehoog = 2;
				a_hoogwateralarm = 1; //f_hoogwater_alarm;
				f_start_pomp;
				f_start_nadraai2timer;
				statenormaal = 0;
			}			
		break;
		
		case 2: /* hoogwater */		
			if (b_reset[0] = 1 || b_reset[1] = 1)
			{
				statehoog = 1;
					f_reset_hoogwater_alarm;
			}
			if (t_nadraai2 > c_nadraai2)	
			{
				statehoog = 4;
					f_stop_pomp;
			}
			if (s_pomp_error = 1)
			{
				statehoog = 3;
					f_stop_pomp;
					f_start_pomp;
			}
			else	
			{
				statehoog = 2;
			}
		break;
		

		case 3: /*pomp restart*/		
			if (b_reset[0] = 1 ||b_reset[1] = 1)
			{
				statehoog = 1;	
					f_reset_hoogwater_alarm;		
			}
			if ( t_nadraai2 > c_nadraai2 )
			{
				statehoog = 4;
					f_stop_pomp;
			}
			else 
			{
				statehoog = 3;
			}
		break;
		

		case 4: /*pomp uit*/		
			if (b_reset[0] = 1 ||b_reset[1] = 1)
			{
				statehoog = 1;	
					f_reset_hoogwater_alarm;		
			}
			if ( f_hoogwater = 1 )
			{
				statehoog = 2;
					f_hoogwater_alarm;
					f_start_pomp;		
					f_start_nadraai2timer;
			}
			else 
			{
				statehoog = 4;
			}
		break;
	
		default 
		{
			statehoog = 1;
		}
	}	
}

  /*-----------------  begin 'normale' fsm -------------------*/

/** 
 * fuctie fsm_normaal
 * @param fsm voor normale werking wordt  uitgevoerd als f hoogwater 0 is
 * @param statenormaal de state waar de fsm in moet komen
 */

void fsm_normaal (void)
{
	switch (statenormaal)
	{
		case 1: /*pomp uit normaal*/

			if (s_nivo > c_nivo_onderste)
			{
				statenormaal = 2;
					f_start_pomp;
			}
			else
			{
				statenormaal = 1;
			}
		break;
		
	
		case 2: 	/*pompaan*/

			if (s_nivo <= c_nivo_uitschakel)
			{
				statenormaal = 3;
					f_start_nadraaiTimer;
			}
			if (s_pomp_error => 1)
			{
				statenormaal = 4;
					f_stop_pomp;
			}
			else
			{
				statenormaal = 2;
			}
		break;
		

		case 3:	/*pomp nadraai*/

			if (t_nadraai > c_nadraai)
			{
				statenormaal = 1;
					f_stop_pomp	;
			}
			else
			{
				statenormaal = 3;
			}
		break;
		

		case 4:	/*pomp uit error*/

			if (s_nivo < c_nivo_onderste
			{
				statenormaal = 5;
			}
			if (s_nivo > c_nivo_onderste)
			{
				statenormaal = 6;	
					f_start_pomp
			}
			else
			{
				statenormaal = 4;
			}
		break;
		

		case 5:	/*wacht op nivo*/
			if (s_nivo > c_nivo_bovenste)
			{
				statenormaal = 6;
					f_start_pomp
			}
			else
			{
				statenormaal = 5;
			}
		break;
		

		case 6:	/*pomp aan (direct)*/
			if (s_nivo <= c_nivo_uitschakel)
			{
				statenormaal = 1;
					f_stop_pomp	;
			}
			else 
			{
				statenormaal = 6; 
			}
		break;
	}
}



/*----------------------- vanaf hier zijn het 'normale' functies*/

/** 
 * fuctie stop pomp 
 * @param a_pomp_active array van actieve pompen
 */

void f_stop_pomp(void) 
{
	f_eeprom_looptijd;
	(a_pomp_active+0) = 0;
	(a_pomp_active+1) = 0;
}

/** 
 * fuctie start pomp 
 * @param start allen een pomp zonder error, degene met de minste draai uren
 * @param a_pomp_active array van actieve pompen
 * @param z_pomp_looptijd array met looptijden van beide pompen
 * @param t_looptijd timer van maximale looptijd van de pomp
 * @param s_pomp_error staat er een pomp in error
 */

void f_start_pomp(void)
{
	(t_looptijd) = 0; 
			if ((z_pomp_looptijd+0) <= (z_pomp_looptijd+1) || (s_pomp_error+1 < 1) && (s_pomp_error+0 < 1))
			if ((z_pomp_looptijd+0) > (z_pomp_looptijd+1) || (s_pomp_error+0 < 1) && (s_pomp_error+1 < 1))
			{
				(a_pomp_active+1) = 1;	
			}								/*start pomp 2*/
				else;  /*allebij in error, geen pomp start*/
	f_eeprom_inschakelingen
}


	  /* er moet nog een functie start timer gemaakt worden */
	/* f_starttimer (maakt gebruik van hardware timer's en interupts*/




/**
 *functie f_eeprom_looptijd onthoud de looptijded 
 * @param t_looptijd huidige looptijd actieve pomp
 * @param z_pomp_looptijd looptijden van bijde pompen
 * @param s_pomp_error array van actieve pompen
 * @param a_pomp_active welke pomp loopt
 */

void f_eeprom_looptijd (void)
{	
/* te doen haal z_pomp_looptijd op uit het eeprom */

	if ((a_pomp_active+0) = 1 && (t_looptijd) > 0)
	if ((*(a_pomp_active+1) == 1) && (*(t_looptijd) > 0))
	if ((a_pomp_active+1) = 1 && (t_looptijd) > 0)
	{	
		(z_pomp_looptijd+1) = (t_looptijd) + (z_pomp_looptijd+1); 
	}
	else;
	(t_looptijd) = 0;

/* te doen sla z_pomp_looptijd op op het eeprom */

}

/**
 *functie f_eeprom_inschakelingen onthoud het aantal inschakelingen 
 * z_pomp_inschakelingen array looptijden van bijde pompen
 * s_pomp_error array van actieve pompen
 * 
 */

void f_eeprom_inschakelingen (void)
{
/* haal z_pomp_inschakelingen op uit het eeprom */	

	if ((s_pomp_active+0) = 1)
	{
		(z_pomp_inschakelingen+0)++
	} 
	if ((s_pomp_active+1) = 1)
	{
		(z_pomp_inschakelingen+1)++
	}
		else;

/* sla z_pomp_inschakelingen op op het eeprom */
}


/**
 *functie f_hoogwater_alarm set hoogwateralarm
 * s_hoogwater hoogwater gedetecteerd
 * s_hoogwateralarm hoogwater alarm
 */

void f_hoogwater_alarm (void)
{
	if ((s_hoogwater) = 1)
	{
		(a_hoogwateralarm) = 1
	} 
}



/**
 *functie f_hoogwater_alarm set hoogwateralarm
 * s_hoogwateralarm hoogwater alarm
 * 
 */

void f_reset_hoogwater_alarm (void)
{
	(a_hoogwateralarm) = 0 
}



/**
 *functie f_start_nadraaitimer start eerste nadraaitimer
 * 
 */

/*
void f_start_nadraaitimer (void)
{
	// eerst de timer resetten	
	// hier moet de timer gestart worden
}
*/

/**
 *functie f_start_nadraai2timer start eerste nadraaitimer
 * 
 */

/*
void f_start_nadraai2timer (void)
{
	// eerst de timer resetten	
	// hier moet de timer gestart worden
}
*/







