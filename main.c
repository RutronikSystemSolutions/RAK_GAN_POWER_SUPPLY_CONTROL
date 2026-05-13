/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the RAK-GAN Power Supply Control 
*             Application for ModusToolbox.
*
* Related Document: See README.md
*
* Created on: 2026-05-06
* Company: Rutronik Elektronische Bauelemente GmbH
* Address: Jonavos g. 30, Kaunas 44262, Lithuania
* Author: GDR
*
*******************************************************************************
* Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*
* Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
* including the software is for testing purposes only and,
* because it has limited functions and limited resilience, is not suitable
* for permanent use under real conditions. If the evaluation board is
* nevertheless used under real conditions, this is done at one’s responsibility;
* any liability of Rutronik is insofar excluded
*******************************************************************************/


/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cy_pdl.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cycfg_peripherals.h"
#include "cycfg_pwrconv.h"
#include "mtb_hal.h"
#include "cycfg.h"
#include "buck.h"
#include "i2c_master.h"
#include "GT911.h"
#include "spi_master.h"
#include "ssd1309.h"
#include "lvgl.h"
#include "lvgl_if.h"
#include "ui.h"
#include "display_update.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define LVGL_LOOP_MS       (10u)
#define VOLT_SET_POT_EN		0
/*******************************************************************************
* Global Variables
*******************************************************************************/
uint32_t sys_time_ms = 0;
float32_t energy_out = 0;
float32_t buck_iout;
float32_t buck_i_in;
float32_t buck_temp;
float32_t vin_adc;
float32_t vout_adc;
float32_t vout_avg;
float32_t vout_set = 3.0;
float32_t iout_set = 10.0f;
float32_t vpot_adc;
float32_t vpot_avg;
float32_t buck_iout_avg = 0;
float32_t buck_i_in_avg = 0;
float32_t buck_temp_avg = 0;
float32_t vin_avg = VIN_COUNT;
bool buck_fault = false;
bool v_adj = false;
bool buck_enable = false;

enum supply_mode supp_mode = CV;

/* For the Retarget -IO (Debug UART) usage */
static cy_stc_scb_uart_context_t    DEBUG_UART_context;           /** UART context */
static mtb_hal_uart_t               DEBUG_UART_hal_obj;

GT911_Config_t gt911_config =
{
		.Number_Of_Touch_Support = 1,
		.ReverseX = false,
		.ReverseY = false,
		.SoftwareNoiseReduction = true,
		.SwithX2Y = true,
		.X_Resolution = 128,
		.Y_Resolution = 128
};

/* Interrupt configuration structure of button GPIO. */
cy_stc_sysint_t button_press_intr_config =
{
    .intrSrc = USER_BUTTON_IRQ,
    .intrPriority = 3UL,
};

/* The user CSG comparator interrupt configuration structure */
cy_stc_sysint_t comp_prot_intrCfg =
{
     .intrSrc = pass_interrupt_csg_cmps_IRQn,
     .intrPriority = 1u
};

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/* Interrupt handler for button interrupt. */
void button_press_intr_handler(void);
cy_rslt_t hw_init(void);

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function name: button_press_intr_handler
*********************************************************************************
* Summary:
* This is the button interrupt handler that control the converter state machine.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void button_press_intr_handler(void)
{
 	cy_rslt_t result;

    /* Clears the GPIO interrupt. */
    Cy_GPIO_ClearInterrupt(USER_BUTTON_PORT, USER_BUTTON_NUM);

	/* Reset fault */
	if(buck_fault)
	{
	 	result = BUCK_enable(); /* Set target and enable HW */
        if (result != CY_RSLT_SUCCESS)
        {
			CY_ASSERT(0);
        }
		buck_fault = false;
		result= BUCK_start(); /* Start HW */
        if (result != CY_RSLT_SUCCESS)
        {
			CY_ASSERT(0);
        }
    	while (0UL != BUCK_get_state(MTB_PWRCONV_STATE_RAMP)){}; /* Wait while it is ramping to the target */

		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, SET_DIM_LED);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, CLR_LED);
	}
}

void scheduler_callback(void)
{
    BUCK_ramp();
	BUCK_scheduled_adc_trigger();
	lv_tick_inc(LVGL_LOOP_MS);
	sys_time_ms += 10;

	energy_out = energy_out + (0.01/3600) * vout_avg * buck_iout_avg;
	//ui_tick();
}

cy_rslt_t hw_init(void)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initializes the timer used for status LED. */
    result = Cy_TCPWM_PWM_Init(PWM_LED_RED_HW, PWM_LED_RED_NUM, &PWM_LED_RED_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initializes the timer used for status LED. */
    result = Cy_TCPWM_PWM_Init(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, &PWM_LED_GREEN_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initializes the timer used for status LED. */
    result = Cy_TCPWM_PWM_Init(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, &PWM_LED_BLUE_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

	Cy_TCPWM_PWM_Enable(PWM_LED_RED_HW, PWM_LED_RED_NUM);
	Cy_TCPWM_PWM_Enable(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM);
	Cy_TCPWM_PWM_Enable(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM);

    /* Starts the TCPWM for LEDs. */
    Cy_TCPWM_TriggerStart_Single(PWM_LED_RED_HW, PWM_LED_RED_NUM);
    Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, SET_DIM_LED);
    Cy_TCPWM_TriggerStart_Single(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM);
    Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
    Cy_TCPWM_TriggerStart_Single(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM);
    Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);

    /*Initializes the interrupt for button action. */
    result = Cy_SysInt_Init(&button_press_intr_config, button_press_intr_handler);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /*Initializes the interrupt for comparator based input current protection. */
    result = Cy_SysInt_Init(&comp_prot_intrCfg, comparator_prot_intr_handler);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

	/* Clears all pending interrupts before configuring interrupts. */
    NVIC_ClearPendingIRQ(button_press_intr_config.intrSrc);
	NVIC_ClearPendingIRQ(comp_prot_intrCfg.intrSrc);

    /* Enables all interrupts. */
    NVIC_EnableIRQ(button_press_intr_config.intrSrc);
    NVIC_EnableIRQ(comp_prot_intrCfg.intrSrc);

	/* Enable the Power Input*/
	CyDelay(1000 /*Wait for power supply voltage to settle*/);
    Cy_GPIO_Set(POW_EN_PORT, POW_EN_NUM);
	CyDelay(100 /*Wait for input voltage to settle*/);

	/*Set LED to GREEN*/
	Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, CLR_LED);
	Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, SET_DIM_LED);

	/* Enable CSG-based protection */
    /* Connect CSG slice 3 comparator output to the BUCK1 HW protection trigger */
    HPPASS_config.trigLevel[0].compMsk = CY_HPPASS_INTR_CSG_3_CMP;

	/*Initializes the HPPASS */
    if (CY_RSLT_SUCCESS != Cy_HPPASS_Init(&HPPASS_config))
    {
        CY_ASSERT(0);
    }

    /*Initializes the buck converter 1 */
    if (CY_RSLT_SUCCESS != BUCK_init())
    {
        CY_ASSERT(0);
    }

    /* Sets the DAC value for buck converter */
    Cy_HPPASS_DAC_SetValue(BUCK_IOUT_LIMIT_SLICE_IDX, DCOUT_I_LIMIT_COMP);

    /*Start the DAC for buck converter */
    Cy_HPPASS_DAC_Start(BUCK_IOUT_LIMIT_SLICE_IDX, CY_HPPASS_DAC_FW);

    /* Clear the comparator interrupt */
    Cy_HPPASS_Comp_ClearInterrupt(CY_HPPASS_INTR_CSG_3_CMP);

    /* Configures the comparator interrupt */
    Cy_HPPASS_Comp_SetInterruptMask(CY_HPPASS_INTR_CSG_3_CMP);

	return result;
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function. It sets up a timer to trigger a periodic interrupt.
* The main while loop checks for the status of a flag set by the interrupt and
* toggles an LED at 1Hz to create an LED blinky. Will be achieving the 1Hz Blink
* rate. The while loop also checks whether the 'Enter' key was pressed and
* stops/restarts LED blinking.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
	arm_pid_instance_f32 PID;
	bool buck_enabled = false;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

	result = ard_i2c_master_init();
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

	result = ard_spi_master_init();
    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

	hw_init();

    /* Configure retarget-io to use the debug UART port */
    result = (cy_rslt_t)Cy_SCB_UART_Init(DEBUG_UART_HW, &DEBUG_UART_config, &DEBUG_UART_context);

    /* UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    Cy_SCB_UART_Enable(DEBUG_UART_HW);

    /* Setup the HAL UART */
    result = mtb_hal_uart_setup(&DEBUG_UART_hal_obj, &DEBUG_UART_hal_config, &DEBUG_UART_context, NULL);

    /* HAL UART init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cy_retarget_io_init(&DEBUG_UART_hal_obj);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
 
    /* Setup a simple scheduled event timer */
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_LF, (uint32_t)(0.01 * 32768)); /* Set the desired 10ms SysTick period: 0.01s * CLK_LF frequency in Hz */
    (void) Cy_SysTick_SetCallback(0UL, scheduler_callback); /* Bing the callback */
    Cy_SysTick_Enable(); /* Enable the scheduled events generation */

    if(GT911_Init(gt911_config) != GT911_OK)
    {
    	CY_ASSERT(0);
    }

    result =  ssd1309_init_spi();
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

    /* Initialize LVGL and set up the essential components required for LVGL. */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    /*GUI*/
    ui_init();
 
	/*Load PID values and initialise*/
	PID.Kp = VOUT_PID_KP;         // Proportional gain
    PID.Ki = VOUT_PID_KI;         // Integral gain
    PID.Kd = VOUT_PID_KD;         // Derivative gain
    arm_pid_init_f32(&PID, 1); // 1 = reset state
 
    for (;;)
    {
    	/*LVGL Execution*/
    	lv_task_handler();
		update_disp_widgets(sys_time_ms);
		
		if(buck_enable != buck_enabled)
		{
			if(buck_enable)
			{
				BUCK_enable(); /* Set target and enable HW */
    			BUCK_start(); /* Start HW */
    			while (0UL != BUCK_get_state(MTB_PWRCONV_STATE_RAMP)){}; /* Wait while it is ramping to the target */
			    buck_enabled = buck_enable;
			}
			else
			{
				BUCK_disable();
			    buck_enabled = buck_enable;
				buck_iout_avg = 0;
				vout_avg = 0;
			}
		}

		if( v_adj && buck_enabled )
    	{
			if(buck_iout_avg > iout_set) 
			{
				supp_mode = CC;
				float32_t error = iout_set - buck_iout_avg;
				float32_t volt_target = vout_avg + arm_pid_f32(&PID, error);
				if(volt_target < VOUT_SET_MIN)
				{volt_target = VOUT_SET_MIN;}
				if(volt_target > VOUT_SET_MAX)
				{volt_target = VOUT_SET_MAX;}
				BUCK_set_target(volt_target);
			}
			else if (buck_iout_avg < (iout_set-IOUT_CC_HYS))
			{
				supp_mode = CV;
				arm_pid_init_f32(&PID, 1);//reset PID
#if VOLT_SET_POT_EN ==1
				vout_set = vpot_avg * VOUT_SET_MAX / (1 << 12U);
#endif
				if(fabs(vout_avg - vout_set) > VOUT_SET_STEP)
    			{
					BUCK_set_target(vout_set);
				}
			}
			else 
			{
				supp_mode = CV;
#if VOLT_SET_POT_EN == 1
				vout_set = vpot_avg * VOUT_SET_MAX / (1 << 12U);
#endif
				if(vout_set < vout_avg)
				{
					BUCK_set_target(vout_set);
				}
			}
	
			v_adj = false;
		}
    }
}


/* [] END OF FILE */
