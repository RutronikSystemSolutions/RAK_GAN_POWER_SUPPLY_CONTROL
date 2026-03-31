/*
 * i2c_master.h
 *
 *  Created on: 2026-03-10
 *      Author: GDR
 */

#ifndef I2C_I2C_MASTER_H_
#define I2C_I2C_MASTER_H_

#include "cy_pdl.h"
#include "cybsp.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define I2C_TIMEOUT_MS		(10U)
#define I2C_SUCCESS         (0UL)
#define I2C_FAILURE         (1UL)

extern cy_stc_scb_i2c_context_t ARD_I2C_context;

cy_rslt_t ard_i2c_master_init(void);
void ARD_I2C_Interrupt(void);
cy_rslt_t master_write(CySCB_Type *base,
                              cy_stc_scb_i2c_context_t *context,
                              uint16_t dev_addr,
                              const uint8_t *data,
                              uint16_t size,
                              uint32_t timeout,
                              bool send_stop);
cy_rslt_t master_read(CySCB_Type *base,
                             cy_stc_scb_i2c_context_t *context,
                             uint16_t dev_addr,
                             uint8_t *data,
                             uint16_t size,
                             uint32_t timeout,
                             bool send_stop);

#endif /* I2C_I2C_MASTER_H_ */
