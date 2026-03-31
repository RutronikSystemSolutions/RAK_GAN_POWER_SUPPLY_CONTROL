/*
 * buck.h
 *
 *  Created on: 2025-10-30
 *      Author: GDR
 */

#ifndef BUCK_H_
#define BUCK_H_

#include "math.h" 
#include "arm_math.h"
#include "cycfg.h"
#include "cy_retarget_io.h"
#include "ntcg103jf103ft1s_lookup.h"
#include "tle5572_adc_to_curr.h"
#include "tle5571_adc_to_curr.h"

enum supply_mode 
{
	CV,
	CC
};

/* DC Output Current Limit*/ 
#define DCOUT_I_LIMIT_COMP	  (680)      /* current limit for CSG (2.1V - 21A)  */ 

/* Number of samples for averaging the parameters used for overload protection */
#define AVERAGING_SAMPLES     (4U)

/* Input Voltage */
#define VIN_COUNT             (980)       /* ADC count for input voltage - 24V*/
#define ADC_VIN_SCALE_VDC     ((6.2f)/(180.0f+6.2f))         /*[V/V] = [Ohm/Ohm]*/
#define ADC_VOUT_SCALE_VDC    ((6.2f)/(180.0f+6.2f))         /*[V/V] = [Ohm/Ohm]*/
#define ADC_VREF_GAIN         ((3.3f)/(3.3f))               /*[V/V], voltage-reference buffer gain (e.g. scaling 5.0V down to 3.3V)*/

/* DC Output Current Limit*/ 
#define DCOUT_I_LIMIT_A		  (21)      /* current limit  */ 

/* DC/DC Converter Temperature Limit °C*/ 
#define BUCK_TEMP_LIMIT_C	  (80)      /* Temperature limit (heatsink on) in °C */

/* Counters values for LED operation */
#define CLR_LED    		(0)
#define SET_FULL_LED    (1000)
#define SET_DIM_LED 	(100)

/*Output set definitions*/
#define VOUT_SET_MAX    	(48.0f)
#define VOUT_SET_MIN    	(2.5f)
#define VOUT_SET_STEP    	(0.1f)
#define IOUT_SET_MAX    	(25.0f)
#define IOUT_SET_STEP    	(0.1f)
#define IOUT_CC_HYS    		(0.7f)
#define VOUT_PID_KP    		(0.75f)
#define VOUT_PID_KI    		(0.0025f)
#define VOUT_PID_KD    		(0.0f)

extern float32_t buck_iout;
extern float32_t buck_i_in;
extern float32_t buck_temp;
extern float32_t vin_adc;
extern float32_t vout_adc;
extern float32_t vout_avg;
extern float32_t vpot_adc;
extern float32_t vpot_avg;
extern float32_t vout_set;
extern float32_t iout_set;
extern float32_t buck_iout_avg;
extern float32_t buck_i_in_avg;
extern float32_t buck_temp_avg;
extern float32_t vin_avg;
extern bool buck_fault;
extern bool v_adj;

/*******************************************************************************
* Function Name: fault_processing
*********************************************************************************
* Summary:
* This function is executes when a fault is detected. It disables
* the buck converter and changes the state.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
__STATIC_INLINE void fault_processing(void)
{
    /* Result variable */
    cy_rslt_t result;

	if(!buck_fault)
	{
		/* Disable the buck converter when protection condition passed. */
    	/* Stops the buck converter. */
    	result = BUCK_disable();
    
    	/*Set LED to bright RED*/
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, SET_FULL_LED);
		buck_fault = true;
    
    	if (result != CY_RSLT_SUCCESS)
    	{
        	CY_ASSERT(0);
    
		}
	}
    
}

__STATIC_INLINE void buck_fault_callback(void)
{
    /*Fault processing after detection of the fault*/
    fault_processing();
}

__STATIC_INLINE void buck_scheduled_adc_callback(void)
{
	static uint8_t sync = AVERAGING_SAMPLES;
	/* Read result from ADC result register. */
	vin_adc   = BUCK_Vin_get_result();
    buck_iout = tle5572_adc_to_current(BUCK_Iout_get_result());
    buck_i_in = tle5571_adc_to_current(BUCK_Iin_get_result());
    buck_temp = ntc_adc_to_temperature((uint16_t)BUCK_Temp_get_result());
    vout_adc  = BUCK_Vout_get_result();
    float vout  = vout_adc *(ADC_VREF_GAIN * CY_CFG_PWR_VDDA_MV * 1.0E-3f) / ((1 << 12U) * ADC_VOUT_SCALE_VDC);
    vpot_adc  = BUCK_Vpot_get_result();
    
    /*Moving Average calculation*/
    buck_iout_avg = (float32_t)((buck_iout_avg - ((buck_iout_avg - buck_iout) / AVERAGING_SAMPLES)));
    buck_i_in_avg = (float32_t)((buck_i_in_avg - ((buck_i_in_avg - buck_i_in) / AVERAGING_SAMPLES)));
    buck_temp_avg = (float32_t)((buck_temp_avg - ((buck_temp_avg - buck_temp) / AVERAGING_SAMPLES)));
    vin_avg       = (float32_t)((vin_avg       - ((vin_avg       - vin_adc)   / AVERAGING_SAMPLES)));
    vout_avg      = (float32_t)((vout_avg      - ((vout_avg       - vout)   / AVERAGING_SAMPLES)));
    vpot_avg      = (float32_t)((vpot_avg      - ((vpot_avg       -  vpot_adc)   / AVERAGING_SAMPLES)));
    
    /* Check for vin voltage, output currents and temperature range */
    if(	(vin_avg < BUCK_Vin_MIN) 				|| 
    	(vin_avg > BUCK_Vin_MAX) 				||
       	/*(buck_iout_avg > DCOUT_I_LIMIT_A)		|| */
       	(buck_temp_avg > BUCK_TEMP_LIMIT_C)
       )
    {
		printf("PROTECTION TRIGGERED Vin avg ADC: %f.2, Iout avg: %f.2 A, Temp avg: %f.2 degC\r\n\n", vin_avg, buck_iout_avg, buck_temp_avg);
        /*Fault processing after detection of the fault*/
        fault_processing();
    }
    
    sync--;
    if(!sync)
    {
		sync = AVERAGING_SAMPLES;
		v_adj = true;
	}
}

/*******************************************************************************
* Function Name: comparator_prot_intr_handler
*********************************************************************************
* Summary:
* This is the comparator protection interrupt handler. In this function, protection
* logic for input current is implemented.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
__STATIC_INLINE void comparator_prot_intr_handler(void)
{
    /* Getting the trigger source of comparator interrupt */
    uint32_t interrupt = Cy_HPPASS_Comp_GetInterruptStatus();
    
    static bool skip_first_int = false;

    /* Clears the comparator interrupt. */
    Cy_HPPASS_Comp_ClearInterrupt(interrupt);

    /*Fault processing after detection of the fault*/
    if(skip_first_int) //transient process?
    {
		fault_processing();
	}
	
	skip_first_int = true;
}

#endif /* BUCK_H_ */
