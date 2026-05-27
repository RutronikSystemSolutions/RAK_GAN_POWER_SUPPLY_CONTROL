/*
 * tle5571_adc_to_curr.c
 *
 *  Created on: 2026-03-17
 *      Author: GDR
 */




#include "tle5571_adc_to_curr.h"

float tle5571_adc_to_current(uint16_t adc_reading)
{
	float approx_current = 0;
	
	approx_current = (float)(adc_reading - 1991.6)/28.53;
	approx_current = approx_current - DCIN_I_OFFSET;
	if( approx_current  < 0 ) { approx_current = 0; }
	
	return approx_current;
}