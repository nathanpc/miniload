/**
 *  adc.h
 *  Handles the ADC stuff.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#ifndef N_ADC_H_
#define N_ADC_H_

void adc_setup();

void fetch_adc_readings();
unsigned int get_adc_sample(unsigned int index);

#endif
