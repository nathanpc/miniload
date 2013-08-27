/**
 *	control.c
 *	Controls the current.
 *
 *	@author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <msp430g2553.h>
#include <stdint.h>

#include "control.h"

#include "boolean.h"
#include "delay.h"
#include "bitio.h"

#define PWM_PIN  BIT2

/**
 *	Setup the PWM pin.
 */
void control_init() {
	P2DIR |= PWM_PIN;
	P2SEL |= PWM_PIN;      // Set P2.2 to TA1.1

	TA1CCR0  = 100 - 1;    // PWM period.
	TA1CCTL1 = OUTMOD_7;   // CCR1 Reset/Set.
	TA1CCR1  = 0;          // CCR1 PWM duty cycle.
	TA1CTL   = TASSEL_2 + MC_1;

	TA1CCR1  = 50;  //////////////////////
}

/**
 *	Sets a voltage to the op-amp to be converted into a current.
 *
 *	@param increment Increment.
 */
void set_current(bool increment) {
	/*if (vregs.curr_onscreen == LM317) {
		if (increment) {
			// Assuming the PWM max is 100.
			if (vregs.pwm[LM317] < 100) {
				//vregs.voltages[LM317] += 0.1;
				vregs.pwm[LM317] += 1;
			}
		} else {
			// Assuming we won't go negative.
			if (vregs.pwm[LM317] > 0) {
				//vregs.voltages[LM317] -= 0.1;
				vregs.pwm[LM317] -= 1;
			}
		}

		if (vregs.state[LM317]) {
			TA1CCR1 = vregs.pwm[LM317];
		}
	}*/
}
