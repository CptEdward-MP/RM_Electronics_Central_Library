#include "imu_hw.h"

/* ============================================================
 * PRIVATE: ERROR MAPPING
 * ============================================================ */

static hw_result_t map_hal_error(hw_i2c_t *hw, HAL_StatusTypeDef ret)
{
    if (!hw || !hw->i2c)
        return HW_FAIL;

    if (ret == HAL_OK)
        return hw->last_error = HW_OK;

    if (ret == HAL_TIMEOUT)
        return hw->last_error = HW_TIMEOUT;

    uint32_t err = HAL_I2C_GetError(hw->i2c);

    if (err & HAL_I2C_ERROR_AF)
        return hw->last_error = HW_NO_ACK;

    if (err & (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_ARLO))
        return hw->last_error = HW_BUS_ERROR;

    return hw->last_error = HW_FAIL;
}

/* ============================================================
 * PRIVATE: LOW LEVEL
 * ============================================================ */

static hw_result_t i2c_read(hw_i2c_t *hw,
                           uint8_t addr,
                           uint8_t reg,
                           uint8_t *buf,
                           uint16_t len,
                           uint32_t timeout)
{
    if (!hw || !hw->i2c || !buf)
        return HW_FAIL;

    HAL_StatusTypeDef ret =
        HAL_I2C_Mem_Read(hw->i2c,
                         addr << 1,
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         buf,
                         len,
                         timeout);

    return map_hal_error(hw, ret);
}

static hw_result_t i2c_write(hw_i2c_t *hw,
                            uint8_t addr,
                            uint8_t reg,
                            uint8_t *buf,
                            uint16_t len,
                            uint32_t timeout)
{
    if (!hw || !hw->i2c || !buf)
        return HW_FAIL;

    HAL_StatusTypeDef ret =
        HAL_I2C_Mem_Write(hw->i2c,
                          addr << 1,
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          buf,
                          len,
                          timeout);

    return map_hal_error(hw, ret);
}

/* ============================================================
 * PUBLIC: INIT
 * ============================================================ */

void hw_i2c_init(hw_i2c_t *hw, I2C_HandleTypeDef *i2c)
{
    if (!hw || !i2c)
        return;

    hw->i2c = i2c;

    hw->timeout_short  = 2;
    hw->timeout_medium = 10;
    hw->timeout_long   = 100;

    hw->last_error = HW_OK;
}

void hw_i2c_set_timeouts(hw_i2c_t *hw,
                         uint32_t t_short,
                         uint32_t t_med,
                         uint32_t t_long)
{
    if (!hw)
        return;

    hw->timeout_short  = t_short;
    hw->timeout_medium = t_med;
    hw->timeout_long   = t_long;
}

/* ============================================================
 * PUBLIC: READ / WRITE
 * ============================================================ */

hw_result_t hw_i2c_read_reg(hw_i2c_t *hw,
                           uint8_t addr,
                           uint8_t reg,
                           uint8_t *value)
{
    if (!hw || !value)
        return HW_FAIL;

    return i2c_read(hw, addr, reg, value, 1, hw->timeout_short);
}

hw_result_t hw_i2c_write_reg(hw_i2c_t *hw,
                            uint8_t addr,
                            uint8_t reg,
                            uint8_t value)
{
    if (!hw)
        return HW_FAIL;

    return i2c_write(hw, addr, reg, &value, 1, hw->timeout_short);
}

hw_result_t hw_i2c_read_multi(hw_i2c_t *hw,
                             uint8_t addr,
                             uint8_t reg,
                             uint8_t *buf,
                             uint16_t len)
{
    if (!hw || !buf || len == 0)
        return HW_FAIL;

    return i2c_read(hw, addr, reg, buf, len, hw->timeout_long);
}

hw_result_t hw_i2c_write_multi(hw_i2c_t *hw,
                              uint8_t addr,
                              uint8_t reg,
                              uint8_t *data,
                              uint16_t len)
{
    if (!hw || !data || len == 0)
        return HW_FAIL;

    return i2c_write(hw, addr, reg, data, len, hw->timeout_medium);
}

/* ============================================================
 * PUBLIC: BUS MANAGEMENT
 * ============================================================ */

hw_result_t hw_i2c_probe(hw_i2c_t *hw, uint8_t addr)
{
    if (!hw || !hw->i2c)
        return HW_FAIL;

    HAL_StatusTypeDef ret =
        HAL_I2C_IsDeviceReady(hw->i2c,
                              addr << 1,
                              5,
                              hw->timeout_short);

    return map_hal_error(hw, ret);
}

hw_result_t hw_i2c_reinit(hw_i2c_t *hw)
{
    if (!hw || !hw->i2c)
        return HW_FAIL;

    HAL_I2C_DeInit(hw->i2c);
    HAL_Delay(2);
    HAL_I2C_Init(hw->i2c);

    return HW_OK;
}

hw_result_t hw_i2c_recover(hw_i2c_t *hw)
{
    if (!hw)
        return HW_FAIL;

    hw_i2c_reinit(hw);
    return HW_OK;
}

/* ============================================================
 * ERROR HELPERS
 * ============================================================ */

bool hw_i2c_is_timeout(hw_i2c_t *hw)
{
    return hw && hw->last_error == HW_TIMEOUT;
}

bool hw_i2c_no_ack(hw_i2c_t *hw)
{
    return hw && hw->last_error == HW_NO_ACK;
}

bool hw_i2c_bus_error(hw_i2c_t *hw)
{
    return hw && hw->last_error == HW_BUS_ERROR;
}

hw_result_t hw_i2c_last_error(hw_i2c_t *hw)
{
    return hw ? hw->last_error : HW_FAIL;
}
