/** \file functions_fsm.c
 * Functies die worden aangeroepen vanuit het FSM
 *
 */


#ifndef f
void f_stop_pomp(int * s_pomp_active) {
	*(s_pomp_active+0) = 0;
	*(s_pomp_active+1) = 0;
}

#endif
