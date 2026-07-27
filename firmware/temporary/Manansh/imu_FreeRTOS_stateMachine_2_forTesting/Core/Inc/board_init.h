/*
 * board_init.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Manansh Pandey
 */

#ifndef BOARD_INIT_H
#define BOARD_INIT_H

#include "imu_logic.h"
#include "imu_hw.h"

/* Getters */
imu_t* imu_get(void);
hw_i2c_t* i2c_bus_get(void);

/* Init */
void imu_system_init(void);

#endif
