/*
 * spi_master.h
 *
 *  Created on: 2026-03-10
 *      Author: GDR
 */

#ifndef SPI_SPI_MASTER_H_
#define SPI_SPI_MASTER_H_

#include "cy_pdl.h"
#include "cybsp.h"

#define SPI_INTR_NUM        ((IRQn_Type) scb_5_interrupt_IRQn)
#define SPI_INTR_PRIORITY   (3U)
#define ARD_SPI_WAIT_MS   	(10U)

extern cy_stc_scb_spi_context_t ARD_SPI_context;

void ARD_SPI_Isr(void);
cy_rslt_t ard_spi_master_init(void);
cy_rslt_t spi_master_write( cy_stc_scb_spi_context_t* context, void* txBuffer, uint32_t size);

#endif /* SPI_SPI_MASTER_H_ */
