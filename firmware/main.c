/**
 *  main.c
 *  Electronic DC load firmware.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <stdio.h>
#include <string.h>
#include <msp430g2553.h>

// Devices.
#include "PCD8544.h"
#include "adc.h"
#include "control.h"

// Helpers.
#include "boolean.h"
#include "delay.h"
#include "fonts.h"
#include "ftoa.h"
#include "strings.h"

void print_current();
void print_digit(unsigned int pos, unsigned int digit);


/**
 *  The mains.
 */
void main() {
	WDTCTL = WDTPW + WDTHOLD;  // Disable WDT.
	BCSCTL1 = CALBC1_8MHZ;     // 1MHz clock.
	DCOCTL = CALDCO_8MHZ;
	BCSCTL2 &= ~(DIVS_3);      // SMCLK = DCO = 1MHz.

	// Setup for the 32kHz crystal.
	BCSCTL1 |= DIVA_3;  // ACLK / 8.
	BCSCTL3 |= XCAP_3;  // 12.5pF capacitor setting for 32768Hz crystal.

	// Setup the current control.
	control_init();

	// Setup ADC.
	adc_setup();

	// Setup the LCD stuff.
	lcd_setup();
	delay_ms(1);  // Just to make sure the LCD is ready
	lcd_init();
	lcd_clear();

	// Enable the interrupts.
	__enable_interrupt();
	// TODO: Test that LMP + GIE thing?

	while (TRUE) {
		//print_digit(0, 8);
		print_digit(1, 5);
		print_digit(2, 0);
		print_digit(3, 0);

		print_current();

		delay_ms(1000);
	}
}

/**
 *  Prints the load current.
 */
void print_current() {
	char vstr[10];
	ftoa(vstr, get_adc_sample(), 1);
	lcd_set_pos(0, 0);
	lcd_print(vstr);
}

/**
 *  Prints a big 18x24 digit in the screen.
 *
 *  @param pos The digit position.
 *  @param digit A number from 0 to 9.
 */
void print_digit(unsigned int pos, unsigned int digit) {
	for (unsigned int row = 0; row < 3; row++) {
		switch (pos) {
		case 0:
			lcd_set_pos(6, row + 2);
			break;
		case 1:
			lcd_set_pos(24, row + 2);
			break;
		case 2:
			lcd_set_pos(42, row + 2);
			break;
		case 3:
			lcd_set_pos(60, row + 2);
			break;
		}

		for (unsigned int col = 0; col < 18; col++) {
			lcd_command(0, big_font[digit][row][col]);
		}
	}
}
