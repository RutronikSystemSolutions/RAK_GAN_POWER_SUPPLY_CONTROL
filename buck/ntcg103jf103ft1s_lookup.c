/*
 * ntcg103jf103ft1s_lookup.c
 *
 *  Created on: 2026-01-08
 *      Author: GDR
 */

#include "ntcg103jf103ft1s_lookup.h"

#define NTC_TABLE_SIZE 39

// ADC reading to temperature lookup table (temperatures in Celsius)
const int16_t ntc_adc_table[NTC_TABLE_SIZE] = {
	3890, 	// -40°C
	3831, 	// -35°C
	3758, 	// -30°C
	3672, 	// -25°C
	3570, 	// -20°C
	3451, 	// -15°C
	3315, 	// -10°C
	3164, 	// -5°C
	2997, 	// 0°C
	2819, 	// 5°C
	2631, 	// 10°C
	2438, 	// 15°C
	2242, 	// 20°C
	2048, 	// 25°C
	1859, 	// 30°C
	1678, 	// 35°C
	1508, 	// 40°C
	1349, 	// 45°C
	1203, 	// 50°C
	1070, 	// 55°C
	950, 	// 60°C
	842, 	// 65°C
	746, 	// 70°C
	661, 	// 75°C
	586, 	// 80°C
	519, 	// 85°C
	461, 	// 90°C
	409, 	// 95°C
	364, 	// 100°C
	324, 	// 105°C
	289, 	// 110°C
	259, 	// 115°C
	231, 	// 120°C
	208, 	// 125°C
	187, 	// 130°C
	168, 	// 135°C
	152, 	// 140°C
	137, 	// 145°C
	124 	// 150°C
};

const float ntc_temp_table[NTC_TABLE_SIZE] = {
    -40.00f, // -40.00°C
    -35.00f, // -35.00°C
    -30.00f, // -30.00°C
    -25.00f, // -25.00°C
    -20.00f, // -20.00°C
    -15.00f, // -15.00°C
    -10.00f, // -10.00°C
    -5.00f, // -5.00°C
    0.00f, // 0.00°C
    5.00f, // 5.00°C
    10.00f, // 10.00°C
    15.00f, // 15.00°C
    20.00f, // 20.00°C
    25.00f, // 25.00°C
    30.00f, // 30.00°C
    35.00f, // 35.00°C
    40.00f, // 40.00°C
    45.00f, // 45.00°C
    50.00f, // 50.00°C
    55.00f, // 55.00°C
    60.00f, // 60.00°C
    65.00f, // 65.00°C
    70.00f, // 70.00°C
    75.00f, // 75.00°C
    80.00f, // 80.00°C
    85.00f, // 85.00°C
    90.00f, // 90.00°C
    95.00f, // 95.00°C
    100.00f, // 100.00°C
    105.00f, // 105.00°C
    110.00f, // 110.00°C
    115.00f, // 115.00°C
    120.00f, // 120.00°C
    125.00f, // 125.00°C
    130.00f, // 130.00°C
    135.00f, // 135.00°C
    140.00f, // 140.00°C
    145.00f, // 145.00°C
    150.00f // 150.00°C
};

/**
 * Convert ADC reading to temperature using linear interpolation
 * @param adc_reading: Raw ADC reading
 * @return: Temperature in degrees Celsius (or NaN if out of range)
 */
float ntc_adc_to_temperature(uint16_t adc_reading) {
    // Check bounds
    if (adc_reading >= ntc_adc_table[0]) 
	{
        return ntc_temp_table[0];
    }

    if (adc_reading <= ntc_adc_table[NTC_TABLE_SIZE - 1]) 
	{
        return ntc_temp_table[NTC_TABLE_SIZE - 1];
    }

    // Binary search for the right interval
    int left = 0;
    int right = NTC_TABLE_SIZE - 1;

    while (right - left > 1) 
	{
        int mid = (left + right) / 2;
        if (ntc_adc_table[mid] < adc_reading) 
		{
            right = mid;
        } else
			{
            	left = mid;
        	}
    }

    // Linear interpolation between two points
    float adc_diff = ntc_adc_table[right] - ntc_adc_table[left];
    float temp_diff = ntc_temp_table[right] - ntc_temp_table[left];
    float adc_offset = adc_reading - ntc_adc_table[left];

    return ntc_temp_table[left] + (temp_diff * adc_offset / adc_diff);
}

// Example usage:
// uint16_t adc_value = read_adc();
// float temperature = ntc_adc_to_temperature(adc_value);
// printf("Temperature: %.2f°C\n", temperature);
