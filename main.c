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

	f_stop_pomp((int*) &s_pomp_active);
	printf("%d - %d\n", s_pomp_active[0], s_pomp_active[1]);

	return 0;
}
