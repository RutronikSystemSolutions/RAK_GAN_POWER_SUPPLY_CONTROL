/*
 * tle5571_adc_to_curr.h
 *
 *  Created on: 2026-03-17
 *      Author: GDR
 */

#ifndef BUCK_TLE5571_ADC_TO_CURR_H_
#define BUCK_TLE5571_ADC_TO_CURR_H_

#include <stdint.h>

/* DC Input Current Offset */ 
#define DCIN_I_OFFSET		  (2.5f)

float tle5571_adc_to_current(uint16_t adc_reading);

#endif /* BUCK_TLE5571_ADC_TO_CURR_H_ */
