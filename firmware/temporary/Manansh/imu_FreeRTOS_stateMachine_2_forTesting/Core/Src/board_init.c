/*
 * board_init.c
 *
 *  Created on: Mar 27, 2026
 *      Author: Manansh Pandey
 */


#include "imu_hw.h"
#include "bno055.h"
#include "imu_logic.h"
#include "main.h"

#include "board_init.h"

extern I2C_HandleTypeDef hi2c2;

//* PRIVATE instances (not visible outside) */
static imu_t imu;
static hw_i2c_t i2c_bus;
static bno055_t bno;

/* Getter functions */
imu_t* imu_get(void)
{
    return &imu;
}

hw_i2c_t* i2c_bus_get(void)
{
    return &i2c_bus;
}

/* Init */
void imu_system_init(void)
{
    hw_i2c_init(&i2c_bus, &hi2c2);
    bno_init(&bno, &i2c_bus, 0x28);

    imu.bno = &bno;
}
