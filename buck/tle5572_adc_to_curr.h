/*
 * tle5572_adc_to_curr.h
 *
 *  Created on: 2026-01-09
 *      Author: GDR
 */

#ifndef TLE5572_ADC_TO_CURR_H_
#define TLE5572_ADC_TO_CURR_H_

#include <stdint.h>

/* DC Output Current Offset */ 
#define DCOUT_I_OFFSET		  (2.5f)

float tle5572_adc_to_current(uint16_t adc_reading);

#endif /* TLE5572_ADC_TO_CURR_H_ */
