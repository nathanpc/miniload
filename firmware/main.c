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
#include "rotary_encoder.h"
#include "adc.h"
#include "control.h"

// Helpers.
#include "boolean.h"
#include "delay.h"
#include "fonts.h"
#include "ftoa.h"
#include "strings.h"

// Interrupt pins.
#define BT_INT   BIT3
#define RE_A_INT BIT5
#define RE_B_INT BIT7

void setup_interrupts();
void handle_re_rotation();
void print_current();
void print_digit(unsigned int pos, unsigned int digit);

unsigned int counter = 0;


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

	// Setup interrupts.
	setup_interrupts();

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
 *	Setup the interrupts stuff.
 */
void setup_interrupts() {
	P1DIR &= ~(BT_INT + RE_A_INT + RE_B_INT);

	P1IES &= ~(BT_INT + RE_A_INT + RE_B_INT);  // Set the interrupt to be from LOW to HIGH.
	P1IFG &= ~(BT_INT + RE_A_INT + RE_B_INT);  // P1.3 and P1.7 IFG cleared
	P1IE |= (BT_INT + RE_A_INT + RE_B_INT);    // Set P1.3 and P1.7 as interrupt.
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

/**
 *  Handle rotary encoder rotation.
 */
void handle_re_rotation() {
	int8_t rotation = rotary_encoder_rotation();
	if (rotation == 1) {
		counter++;
	} else if (rotation == -1) {
		if (counter > 0) {
			counter--;
		}
	}
	
	lcd_set_pos(0, 5);
	char str[20];
	sprintf(str, "%d", counter);
	lcd_print(str);
}


//
//	Interrupts
//

/**
 *	Interrupt service routine for P1.
 */
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR() {
	switch(P1IFG & (BT_INT + RE_A_INT)) {
		case BT_INT:
			// Handle the input array interrupt.
			P1IFG &= ~BT_INT;  // P1.3 IFG cleared.
			
			// Handle the button press.
			
			while ((P1IN & BT_INT) == BT_INT) {}
			break;
		case RE_A_INT:
			// Handle the rotary encoder A interrupt.
			P1IFG &= ~RE_A_INT;  // P1.7 IFG cleared.

			handle_re_rotation();

			break;
		case RE_B_INT:
			// Handle the rotary encoder A interrupt.
			P1IFG &= ~RE_B_INT;  // P1.7 IFG cleared.

			handle_re_rotation();

			break;
		default:
			P1IFG = 0;
			break;
	}

	P1IFG = 0;
}
