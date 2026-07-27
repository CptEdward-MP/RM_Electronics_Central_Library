/*review and rewrite/*
 * pcb.c
 *
 *  Created on: Feb 23, 2026
 *      Author: Manansh Pandey
 */


#include "pcb.h"
#include "imu_hw.h"
#include "main.h"   // ONLY this file includes main.h

/* ============================================================
 *                BOARD PIN DEFINITIONS
 * ============================================================ */

#define IMU_SCL_PORT GPIOB
#define IMU_SCL_PIN  GPIO_PIN_10

#define IMU_SDA_PORT GPIOB
#define IMU_SDA_PIN  GPIO_PIN_11

extern I2C_HandleTypeDef hi2c2;




#define IMU_I2C_ADDR (0x28 << 1)






uint16_t pcb_imu_i2c_addr(void)
{
    return IMU_I2C_ADDR;
}


//
///* ============================================================
// *                BIND I2C TO HW LAYER
// * ============================================================ */
//
//void pcb_bind_i2c(void)
//{
//    hw_bind_i2c(&hi2c2);
//}

/* ============================================================
 *                PIN ACCESSORS
 * ============================================================ */

GPIO_TypeDef* pcb_i2c_scl_port(void)
{
    return IMU_SCL_PORT;
}

uint16_t pcb_i2c_scl_pin(void)
{
    return IMU_SCL_PIN;
}

GPIO_TypeDef* pcb_i2c_sda_port(void)
{
    return IMU_SDA_PORT;
}

uint16_t pcb_i2c_sda_pin(void)
{
    return IMU_SDA_PIN;
}
