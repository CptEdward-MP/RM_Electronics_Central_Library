#ifndef IMU_HW_H
#define IMU_HW_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * RESULT TYPE
 * ============================================================ */

typedef enum
{
    HW_OK = 0,
    HW_FAIL,
    HW_TIMEOUT,
    HW_NO_ACK,
    HW_BUS_ERROR
} hw_result_t;

/* ============================================================
 * I2C BUS STRUCT (BUS ONLY — NO DEVICE ADDRESS)
 * ============================================================ */

typedef struct
{
    I2C_HandleTypeDef *i2c;

    uint32_t timeout_short;
    uint32_t timeout_medium;
    uint32_t timeout_long;

    hw_result_t last_error;

} hw_i2c_t;

/* ============================================================
 * INIT
 * ============================================================ */

void hw_i2c_init(hw_i2c_t *hw, I2C_HandleTypeDef *i2c);

void hw_i2c_set_timeouts(hw_i2c_t *hw,
                         uint32_t t_short,
                         uint32_t t_med,
                         uint32_t t_long);

/* ============================================================
 * BASIC ACCESS (ADDR PASSED PER CALL)
 * ============================================================ */

hw_result_t hw_i2c_read_reg(hw_i2c_t *hw,
                           uint8_t addr,
                           uint8_t reg,
                           uint8_t *value);

hw_result_t hw_i2c_write_reg(hw_i2c_t *hw,
                            uint8_t addr,
                            uint8_t reg,
                            uint8_t value);

hw_result_t hw_i2c_read_multi(hw_i2c_t *hw,
                             uint8_t addr,
                             uint8_t reg,
                             uint8_t *buf,
                             uint16_t len);

hw_result_t hw_i2c_write_multi(hw_i2c_t *hw,
                              uint8_t addr,
                              uint8_t reg,
                              uint8_t *data,
                              uint16_t len);

/* ============================================================
 * BUS MANAGEMENT
 * ============================================================ */

hw_result_t hw_i2c_probe(hw_i2c_t *hw, uint8_t addr);
hw_result_t hw_i2c_reinit(hw_i2c_t *hw);
hw_result_t hw_i2c_recover(hw_i2c_t *hw);

/* ============================================================
 * ERROR HELPERS
 * ============================================================ */

bool hw_i2c_is_timeout(hw_i2c_t *hw);
bool hw_i2c_no_ack(hw_i2c_t *hw);
bool hw_i2c_bus_error(hw_i2c_t *hw);
hw_result_t hw_i2c_last_error(hw_i2c_t *hw);

#endif
