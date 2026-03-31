/*
 * i2c_master.c
 *
 *  Created on: 2026-03-10
 *      Author: GDR
 */

#include "i2c_master.h"

cy_stc_scb_i2c_context_t ARD_I2C_context;
cy_stc_sysint_t ARD_I2C_SCB_IRQ_cfg =
{
	.intrSrc = ARD_I2C_IRQ,
    .intrPriority = 2UL
};

cy_rslt_t ard_i2c_master_init(void)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;
    cy_en_scb_i2c_status_t initStatus;
	cy_en_sysint_status_t sysStatus;
	
	/* Initialize and enable the I2C in master mode. */
    initStatus = Cy_SCB_I2C_Init(ARD_I2C_HW, &ARD_I2C_config, &ARD_I2C_context);
    if(initStatus != CY_SCB_I2C_SUCCESS)
    {
        return (cy_rslt_t)initStatus;
    }

    /* Hook interrupt service routine. */
    sysStatus = Cy_SysInt_Init(&ARD_I2C_SCB_IRQ_cfg, &ARD_I2C_Interrupt);
    if(sysStatus != CY_SYSINT_SUCCESS)
    {
        return (cy_rslt_t)sysStatus;
    }

    /* Enable interrupt in NVIC. */
    NVIC_EnableIRQ((IRQn_Type) ARD_I2C_SCB_IRQ_cfg.intrSrc);

    /* Enable I2C master hardware. */
    Cy_SCB_I2C_Enable(ARD_I2C_HW);
    
    return result;
}

/*******************************************************************************
* Function Name: master_write
********************************************************************************
* Summary:
* This function writes data form I2C master to I2C slave.
*
* Parameters:
*  CySCB_Type *base - The pointer to the I2C SCB instance.
*  cy_stc_scb_i2c_context_t *context - The pointer to the context structure
*  uint16_t dev_addr - 7 bit right justified slave address.
*  const uint8_t *data - The byte to write to I2C slave.
*  uint16_t size - The size of data
*  uint32_t timeout - The time for which this function can block
*  bool send_stop - the bool for stop sending or not
*
* Return:
*  cy_rslt_t
*
*******************************************************************************/
cy_rslt_t master_write(CySCB_Type *base,
                              cy_stc_scb_i2c_context_t *context,
                              uint16_t dev_addr,
                              const uint8_t *data,
                              uint16_t size,
                              uint32_t timeout,
                              bool send_stop)
{
    cy_en_scb_i2c_status_t status = ((*context).state == CY_SCB_I2C_IDLE)
        ? Cy_SCB_I2C_MasterSendStart(base,
                                     dev_addr,
                                     CY_SCB_I2C_WRITE_XFER,
                                     timeout,
                                     context)
        : Cy_SCB_I2C_MasterSendReStart(base,
                                       dev_addr,
                                       CY_SCB_I2C_WRITE_XFER,
                                       timeout,
                                       context);

    if (status == CY_SCB_I2C_SUCCESS)
    {
        while (size > 0)
        {
            status = Cy_SCB_I2C_MasterWriteByte(base,
                                                *data,
                                                timeout,
                                                context);
            if (status != CY_SCB_I2C_SUCCESS)
            {
                break;
            }
            --size;
            ++data;
        }
    }

    if (send_stop)
    {
        /* SCB in I2C mode is very time sensitive.               */
        /* In practice we have to request STOP after each block, */
        /* otherwise it may break the transmission               */
        Cy_SCB_I2C_MasterSendStop(base, timeout, context);
    }

    return status;
}

/*******************************************************************************
* Function Name: master_read
********************************************************************************
* Summary:
* This function reads data form I2C slave.
*
* Parameters:
*  CySCB_Type *base - The pointer to the I2C SCB instance.
*  cy_stc_scb_i2c_context_t *context - The pointer to the context structure
*  uint16_t dev_addr - 7 bit right justified slave address.
*  uint8_t *data - The byte to read from I2C slave.
*  uint16_t size - The size of data
*  uint32_t timeout - The time for which this function can block
*  bool send_stop - the bool for stop sending or not
*
* Return:
*  cy_rslt_t
*
*******************************************************************************/
cy_rslt_t master_read(CySCB_Type *base,
                             cy_stc_scb_i2c_context_t *context,
                             uint16_t dev_addr,
                             uint8_t *data,
                             uint16_t size,
                             uint32_t timeout,
                             bool send_stop)
{
    cy_en_scb_i2c_command_t ack = CY_SCB_I2C_ACK;

    /* Start transaction, send dev_addr */
    cy_en_scb_i2c_status_t status = (*context).state == CY_SCB_I2C_IDLE
        ? Cy_SCB_I2C_MasterSendStart(base,
                                     dev_addr,
                                     CY_SCB_I2C_READ_XFER,
                                     timeout,
                                     context)
        : Cy_SCB_I2C_MasterSendReStart(base,
                                       dev_addr,
                                       CY_SCB_I2C_READ_XFER,
                                       timeout,
                                       context);

    if (status == CY_SCB_I2C_SUCCESS)
    {
        while (size > 0) {
            if (size == 1)
            {
                ack = CY_SCB_I2C_NAK;
            }
            status = Cy_SCB_I2C_MasterReadByte(base,
                                               ack,
                                               (uint8_t *)data,
                                               timeout,
                                               context);
            if (status != CY_SCB_I2C_SUCCESS)
            {
                break;
            }
            --size;
            ++data;
        }
    }

    if (send_stop)
    {
        /* SCB in I2C mode is very time sensitive.               */
        /* In practice we have to request STOP after each block, */
        /* otherwise it may break the transmission               */
        Cy_SCB_I2C_MasterSendStop(base, timeout, context);
    }
    return status;
}

/*******************************************************************************
* Function Name: mI2C_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_I2C_Interrupt() PDL driver function.
*
*******************************************************************************/
void ARD_I2C_Interrupt(void)
{
    Cy_SCB_I2C_Interrupt(ARD_I2C_HW, &ARD_I2C_context);
}
