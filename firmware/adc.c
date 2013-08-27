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

// Pins.
#define CURR_SENSE BIT6

// Parameters.
#define AVG_READS 20
#define VREF 2.9f

unsigned int sample = 0;

/**
 *  Setup the pins and the ADC.
 */
void adc_setup() {
	// Turn P1.6 into a ADC.
	P1DIR &= ~(CURR_SENSE);
	P1SEL |= (CURR_SENSE);

	ADC10CTL1 = INCH_6 + ADC10DIV_3;  // Channel 6, ADC10CLK/4
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference.
	ADC10AE0 |= CURR_SENSE;  // P1.6 ADC option.
}

/**
 *  Fetch the ADC samples.
 */
void fetch_adc_readings() {
	__delay_cycles(1000);             // Wait for ADC Ref to settle
	ADC10CTL0 |= ENC + ADC10SC;       // Sampling and conversion start
    __bis_SR_register(CPUOFF + GIE);  // LPM0 with interrupts enabled    
    sample = ADC10MEM;
}

/**
 *  Gets a sample.
 *
 *  @param index A array index.
 */
float get_adc_sample() {
	float readings[AVG_READS];
	float avg_read = 0.0;

	// Get the values.
	for (unsigned int i = 0; i < AVG_READS; i++) {
		fetch_adc_readings();
		readings[i] = ((sample * VREF) / 1024) * 100;
	}

	// Sum everything.
	for (unsigned int read = 0; read < AVG_READS; read++) {
		avg_read += readings[read];
	}

	// Average the readings.
	return avg_read / AVG_READS;
}

/**
 *  Interrupt service routine for the ADC.
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR () {
	__bic_SR_register_on_exit(CPUOFF);        // Return to active mode
}
