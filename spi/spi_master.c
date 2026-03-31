/*
 * spi_master.c
 *
 *  Created on: 2026-03-10
 *      Author: GDR
 */


#include "spi_master.h"
#include "cy_syslib.h"

cy_stc_scb_spi_context_t ARD_SPI_context;
const cy_stc_sysint_t spiIntrConfig =
{
    .intrSrc      = SPI_INTR_NUM,
    .intrPriority = SPI_INTR_PRIORITY,
};

cy_rslt_t ard_spi_master_init(void)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;
	cy_en_scb_spi_status_t spi_rslt;
	
	/* Configure SPI block */
    spi_rslt = Cy_SCB_SPI_Init(ARD_SPI_HW, &ARD_SPI_config, &ARD_SPI_context);
    if(spi_rslt != CY_SCB_SPI_SUCCESS)
    {
		return (cy_rslt_t)spi_rslt;
	}
	
	/* Hook interrupt service routine and enable interrupt */
	(void) Cy_SysInt_Init(&spiIntrConfig, &ARD_SPI_Isr);
	NVIC_EnableIRQ(SPI_INTR_NUM);

    /* Enable SPI master block. */
    Cy_SCB_SPI_Enable(ARD_SPI_HW);
	
	return result;
}

cy_rslt_t spi_master_write( cy_stc_scb_spi_context_t* context, void* txBuffer, uint32_t size)
{
	cy_rslt_t result = CY_RSLT_SUCCESS;
	cy_en_scb_spi_status_t spi_rslt;

	spi_rslt = Cy_SCB_SPI_Transfer(ARD_SPI_HW, txBuffer, NULL, size, context);
	if(spi_rslt != CY_SCB_SPI_SUCCESS) 	
	{
		return (cy_rslt_t)spi_rslt;
	}
	
	/* Blocking wait for transfer completion */
	while (0UL != (CY_SCB_SPI_TRANSFER_ACTIVE & Cy_SCB_SPI_GetTransferStatus(ARD_SPI_HW, context)))
	{
	}
	
	return result;
}

void ARD_SPI_Isr(void)
{
    Cy_SCB_SPI_Interrupt(ARD_SPI_HW, &ARD_SPI_context);
}