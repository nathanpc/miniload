/**
 *	control.h
 *	Controls the current.
 *
 *	@author Nathan Campos <nathanpc@dreamintech.net>
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "boolean.h"

#define VREF 2.9f

void control_init();
void set_current(bool increment);

#endif
