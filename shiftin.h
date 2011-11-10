/** \file functions_shiftregisters.h
 * Functies die aangeroepen worden iedere main lus
 *
 */
#ifndef SHIFTIN_H_
#define SHIFTIN_H_

void shiftregister_init(void);
void shiftregister_read(int * pin_array, int number_of_shiftregisters);

#endif
