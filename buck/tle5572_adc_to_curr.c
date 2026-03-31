/*
 * tle5572_adc_to_curr.c
 *
 *  Created on: 2026-01-09
 *      Author: GDR
 */

#include "tle5572_adc_to_curr.h"

float tle5572_adc_to_current(uint16_t adc_reading)
{
	float approx_current = 0;
	
	approx_current = (float)(adc_reading - 1998.7)/25.517;
	approx_current = approx_current - DCOUT_I_OFFSET;
	if( approx_current  < 0 ) { approx_current = 0; }
	
	return approx_current;
}

