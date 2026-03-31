#include "gpio_psc3_e_lqfp_80.h"
#include "mtb_hal.h"
#include "cybsp.h"
#include "i2c_master.h"
#include "GT911.h"

#define GT911_I2C_TIMEOUT_MS	10U

void GT911_INT_Input(void)
{
	/* Change the drive mode of P9.2 to Digital (hi-z, input buffer off) */
    Cy_GPIO_SetDrivemode(ARD_IO5_PORT, ARD_IO5_NUM, CY_GPIO_DM_HIGHZ);
}

void GT911_INT_Output(void)
{
	/* Change the drive mode of P9.2 to Digital (strong drive, input buffer off) */
    Cy_GPIO_SetDrivemode(ARD_IO5_PORT, ARD_IO5_NUM, CY_GPIO_DM_STRONG_IN_OFF);
}

void GT911_INT_Control(bool high_or_low)
{
	Cy_GPIO_Write(ARD_IO5_PORT, ARD_IO5_NUM, high_or_low);
}

void GT911_RST_Control(bool high_or_low)
{
	Cy_GPIO_Write(ARD_IO6_PORT, ARD_IO6_NUM, high_or_low);
}

void GT911_Delay(uint16_t ms)
{
	CyDelay(ms);
}

GT911_Status_t GT911_I2C_Init(void)
{
	return GT911_OK;
}

GT911_Status_t GT911_I2C_Write(uint8_t Addr, uint8_t *write_data, uint16_t write_length)
{
	cy_rslt_t result;

    result = master_write(ARD_I2C_HW, &ARD_I2C_context, (uint16_t)Addr, write_data, write_length, GT911_I2C_TIMEOUT_MS, true);
    if (result != CY_RSLT_SUCCESS)
    {
    	return GT911_Error;
    }

    return GT911_OK;
}

GT911_Status_t GT911_I2C_Read(uint8_t Addr, uint8_t *read_data, uint16_t read_length)
{
	cy_rslt_t result;

    result = master_read(ARD_I2C_HW, &ARD_I2C_context, (uint16_t)Addr, read_data, read_length, GT911_I2C_TIMEOUT_MS, true);
    if (result != CY_RSLT_SUCCESS)
    {
    	return GT911_Error;
    }

    return GT911_OK;
}
