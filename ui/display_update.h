/*
 * display_update.h
 *
 *  Created on: 2026-03-13
 *      Author: GDR
 */

#ifndef UI_DISPLAY_UPDATE_H_
#define UI_DISPLAY_UPDATE_H_

#include "cybsp.h"

void update_disp_widgets(uint32_t time_ms);

enum volt_top 
{
	VOLT_SET,
	VOLT_IN,
	POWER_IN,
	LAST_VOLT_ITEM
	
};

enum curr_top 
{
	CURR_SET,
	CURR_IN,
	POWER_OUT,
	LAST_CURR_ITEM
};


#endif /* UI_DISPLAY_UPDATE_H_ */
