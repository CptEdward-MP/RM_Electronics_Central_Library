/*
 * pcb.h
 *
 *  Created on: Feb 23, 2026
 *      Author: Manansh Pandey
 */

#ifndef PCB_H
#define PCB_H

#include "stm32f4xx_hal.h"

/* ============================================================
 *                I2C BINDING
 * ============================================================ */

void pcb_bind_i2c(void);

/* ============================================================
 *                I2C PIN ACCESS (For Bus Recovery)
 * ============================================================ */

GPIO_TypeDef* pcb_i2c_scl_port(void);
uint16_t      pcb_i2c_scl_pin(void);

GPIO_TypeDef* pcb_i2c_sda_port(void);
uint16_t      pcb_i2c_sda_pin(void);

uint16_t pcb_imu_i2c_addr(void);

#endif
