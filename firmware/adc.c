/**
 *  adc.c
 *  Handles the ADC stuff.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include "adc.h"
#include <msp430g2553.h>

// Helpers.
#include "boolean.h"
#include "delay.h"
#include "bitio.h"

// Settings.
#define ADC_CHANNELS 2

// Pins.
#define TMP   BIT6
#define PHOTO BIT7

unsigned int samples[ADC_CHANNELS];

/**
 *  Setup the pins and the ADC.
 */
void adc_setup() {
	// Turn P1.6 into a ADC.
	P1DIR &= ~(TMP + PHOTO);
	P1SEL |= (TMP + PHOTO);

	// Setup the ADC.
	ADC10CTL1 = INCH_7 + ADC10DIV_3 + CONSEQ_3 + SHS_0;
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + MSC + ADC10ON + ADC10IE; 
	ADC10AE0 = TMP + PHOTO; 
	ADC10DTC1 = ADC_CHANNELS;
	/*
	ADC10CTL1 = INCH_6 + ADC10DIV_3;  // Channel 6, ADC10CLK/4
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference.
	ADC10AE0 |= TMP;  // P1.6 ADC option.
	*/
}

/**
 *  Fetch the ADC samples.
 */
void fetch_adc_readings() {
	__delay_cycles(1000);
	ADC10CTL0 &= ~ENC;
	while (ADC10CTL1 & BUSY);
	ADC10SA = (unsigned int)samples;
	ADC10CTL0 |= ENC + ADC10SC;
}

/**
 *  Gets a sample.
 *
 *  @param index A array index.
 */
unsigned int get_adc_sample(unsigned int index) {
	return samples[index];
}

/**
 *  Interrupt service routine for the ADC.
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR () {
	__bic_SR_register_on_exit(CPUOFF);        // Return to active mode
}
