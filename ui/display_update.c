/*
 * display_update.c
 *
 *  Created on: 2026-03-13
 *      Author: GDR
 */

#include "cybsp.h"
#include "cycfg.h"
#include "lvgl.h"
#include "display_update.h"
#include "actions.h"
#include "screens.h"
#include "ui.h"
#include "buck.h"

#define SCREEN_UPDATE_RATE_MS	100U
// Pressing counter value
#define PRESSING_COUNTS   		15

extern float32_t buck_i_in_avg;
extern float32_t vin_avg;
extern float32_t iout_set;
extern float32_t buck_iout_avg;
extern float32_t vout_set;
extern enum supply_mode supp_mode;
extern bool buck_enable;
extern float32_t buck_temp_avg;
extern float32_t energy_out;

uint32_t screen_update_timestamp = 0;
enum volt_top volt_top_select = VOLT_SET;
enum curr_top curr_top_select = CURR_SET;
float v_in = 0;
uint32_t press_cnt = 0;

void action_inc_volt_by_step(lv_event_t *e) 
{
	press_cnt  = 0;
	if(vout_set + VOUT_SET_STEP <= VOUT_SET_MAX)
	{
		vout_set = vout_set + VOUT_SET_STEP;
	}
}

void action_dec_volt_by_step(lv_event_t *e) 
{
	press_cnt  = 0;
    if(vout_set - VOUT_SET_STEP >= VOUT_SET_MIN)
    {
		vout_set = vout_set - VOUT_SET_STEP;
	}
}

void action_inc_curr_by_step(lv_event_t *e) 
{
	press_cnt  = 0;
	if(iout_set + IOUT_SET_STEP <= IOUT_SET_MAX)
	{
		iout_set = iout_set + IOUT_SET_STEP;
	}
}

void action_dec_curr_by_step(lv_event_t *e) 
{
	press_cnt  = 0;
	if(iout_set - IOUT_SET_STEP >= 0)
	{
		iout_set = iout_set - IOUT_SET_STEP;
	}
}

void action_inv_pwr_ctrl(lv_event_t *e) 
{
	if(!buck_enable)
	{
		buck_enable = true;
		lv_label_set_text(objects.power_button_label, "OFF");
	}
	else 
	{
		buck_enable = false;
		lv_label_set_text(objects.power_button_label, "ON");
	}
}

void action_switch_screen_to_prop(lv_event_t *e) 
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED)
    {
        loadScreen(SCREEN_ID_PROPERTIES);
    }
}

void action_switch_volt_label(lv_event_t *e) 
{
	volt_top_select++;
	if(volt_top_select == LAST_VOLT_ITEM)
	{
		volt_top_select = VOLT_SET;
	}
}

void action_switch_curr_label(lv_event_t *e) 
{
	curr_top_select++;
	if(curr_top_select == LAST_CURR_ITEM)
	{
		curr_top_select = CURR_SET;
	}
}

void action_switch_screen_to_main(lv_event_t *e) 
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED)
    {
        loadScreen(SCREEN_ID_MAIN);
    }
}

void action_reset_energy_cnt(lv_event_t *e) 
{
	energy_out = 0;
}

void action_inc_volt_cont(lv_event_t *e) 
{
	press_cnt++;
	
	if(press_cnt >= PRESSING_COUNTS)
	{
		if(vout_set + VOUT_SET_STEP <= VOUT_SET_MAX)
		{
			vout_set = vout_set + VOUT_SET_STEP;
		}
	}
}

void action_dec_volt_cont(lv_event_t *e) 
{
	press_cnt++;
	
	if(press_cnt >= PRESSING_COUNTS)
	{
    	if(vout_set - VOUT_SET_STEP >= VOUT_SET_MIN)
    	{
			vout_set = vout_set - VOUT_SET_STEP;
		}
	}
}

void action_inc_curr_cont(lv_event_t *e) 
{
	press_cnt++;
	
	if(press_cnt >= PRESSING_COUNTS)
	{
		if(iout_set + IOUT_SET_STEP <= IOUT_SET_MAX)
		{
			iout_set = iout_set + IOUT_SET_STEP;
		}
	}
}

void action_dec_curr_cont(lv_event_t *e) 
{
	press_cnt++;
	
	if(press_cnt >= PRESSING_COUNTS)
	{
		if(iout_set - IOUT_SET_STEP >= 0)
		{
			iout_set = iout_set - IOUT_SET_STEP;
		}
	}
}

void update_disp_widgets(uint32_t time_ms)
{
	uint32_t current_time = time_ms;
	
	if(current_time >= screen_update_timestamp + SCREEN_UPDATE_RATE_MS)
	{
		screen_update_timestamp = current_time;
		
		/*Main Screen*/
		lv_obj_t *screen = lv_scr_act();
		if(screen == objects.main)
		{
			/*Voltage Labels*/
			switch(volt_top_select)
			{
				case VOLT_SET:
					lv_label_set_text_fmt(objects.volt_info_label, "set %.1fV", (float)vout_set);
					break;
				case VOLT_IN:
				{
					float v_in = vin_avg *(ADC_VREF_GAIN * CY_CFG_PWR_VDDA_MV * 1.0E-3f) / ((1 << 12U) * ADC_VIN_SCALE_VDC);
					lv_label_set_text_fmt(objects.volt_info_label, "Uin %.1fV", v_in);
					break;
				}
				case POWER_IN:
				{
					float pow_input =  (vin_avg *(ADC_VREF_GAIN * CY_CFG_PWR_VDDA_MV * 1.0E-3f) / ((1 << 12U) * ADC_VIN_SCALE_VDC)) * buck_i_in_avg;
					lv_label_set_text_fmt(objects.volt_info_label, "Pin %.1fW", pow_input);
					break;
				}
				case LAST_VOLT_ITEM:
					break;
				default: 
					lv_label_set_text_fmt(objects.volt_info_label, "set %.1fV", (float)vout_set);
			}
			lv_label_set_text_fmt(objects.u_out, "%.1fV", (float)vout_avg);
			
			/*Current Labels*/
			switch(curr_top_select)
			{
				case CURR_SET:
					lv_label_set_text_fmt(objects.curr_info_label, "set %.1fA", (float)iout_set);
					break;
				case CURR_IN:
				{
					lv_label_set_text_fmt(objects.curr_info_label, "Iin %.1fA", (float)buck_i_in_avg);
					break;
				}
				case POWER_OUT:
				{
					float pow_output =  vout_avg * buck_iout_avg;
					lv_label_set_text_fmt(objects.curr_info_label, "Pout %.1fW", pow_output);
					break;
				}
				case LAST_CURR_ITEM:
					break;
				default: 
					lv_label_set_text_fmt(objects.curr_info_label, "set %.1fA", (float)iout_set);
			}
			lv_label_set_text_fmt(objects.i_out, "%.1fA", (float)buck_iout_avg);
			
			/*Mode Label*/
			switch(supp_mode)
			{
				case CV:
					lv_label_set_text(objects.mode_label, "CV");
					break;
				case CC:
				{
					lv_label_set_text(objects.mode_label, "CC");
					break;
				}
			}
		}
		
		/*Properties screen*/
		if(screen == objects.properties)
		{
			lv_label_set_text_fmt(objects.info_temp_label, "%.1f°C", (float)buck_temp_avg);
			float v_in = vin_avg *(ADC_VREF_GAIN * CY_CFG_PWR_VDDA_MV * 1.0E-3f) / ((1 << 12U) * ADC_VIN_SCALE_VDC);
			lv_label_set_text_fmt(objects.info_uin_label, "Uin %.1fV", v_in);
			lv_label_set_text_fmt(objects.info_i_in_label, "Iin %.1fA", (float)buck_i_in_avg);
			float pow_input =  (vin_avg *(ADC_VREF_GAIN * CY_CFG_PWR_VDDA_MV * 1.0E-3f) / ((1 << 12U) * ADC_VIN_SCALE_VDC)) * buck_i_in_avg;
			lv_label_set_text_fmt(objects.info_pwr_label, "%.1fW", pow_input);
			float pow_output =  vout_avg * buck_iout_avg;
			lv_label_set_text_fmt(objects.info_pwrout_label, "%.1fW", pow_output);
			float efficiency =  pow_output / pow_input * 100;
			if(efficiency > 100){efficiency = 100;}
			lv_label_set_text_fmt(objects.info_eff_label, "%.1f%%", efficiency);
			lv_label_set_text_fmt(objects.info_energy_label, "%.1fWh", energy_out);
			
		}
	}
}

