#include "bno055.h"
#include "imu_regs.h"

/* ============================================================
 * PRIVATE
 * ============================================================ */

static bno_status_t bno_rd16(bno055_t *dev,
                             uint8_t reg,
                             int16_t *out)
{
    uint8_t buf[2];

    if (!dev || !out)
        return BNO_FAIL;

    if (dev->io.i2c_read(dev->io.ctx,
                         dev->addr,
                         reg,
                         buf,
                         2) != 0)
        return BNO_FAIL;

    *out = (int16_t)((buf[1] << 8) | buf[0]);
    return BNO_OK;
}

/* ============================================================
 * INIT
 * ============================================================ */

void bno_init(bno055_t *dev, bno055_io_t *io, uint8_t addr)
{
    if (!dev || !io)
        return;

    dev->io   = *io;
    dev->addr = addr;
}

/* ============================================================
 * CHIP CHECK
 * ============================================================ */

bno_status_t bno_check_chip_id(bno055_t *dev)
{
    uint8_t id;

    if (!dev)
        return BNO_FAIL;

    if (dev->io.i2c_read(dev->io.ctx,
                         dev->addr,
                         BNO_REG_CHIP_ID,
                         &id,
                         1) != 0)
        return BNO_FAIL;

    return (id == BNO_ID) ? BNO_OK : BNO_BAD_ID;
}

/* ============================================================
 * MODE CONTROL
 * ============================================================ */

bno_status_t bno_set_mode(bno055_t *dev, uint8_t mode)
{
    if (!dev)
        return BNO_FAIL;

    if (dev->io.i2c_write(dev->io.ctx,
                          dev->addr,
                          BNO_REG_OPR_MODE,
                          &mode,
                          1) != 0)
        return BNO_FAIL;

    return BNO_OK;
}

bno_status_t bno_set_and_verify_mode(bno055_t *dev, uint8_t mode)
{
    if (bno_set_mode(dev, mode) != BNO_OK)
        return BNO_FAIL;

    dev->io.delay_ms(30);

    uint8_t readback;

    if (dev->io.i2c_read(dev->io.ctx,
                         dev->addr,
                         BNO_REG_OPR_MODE,
                         &readback,
                         1) != 0)
        return BNO_FAIL;

    return ((readback & 0x0F) == mode) ? BNO_OK : BNO_FAIL;
}

/* ============================================================
 * DATA READ (FULL - BACKWARD COMPATIBLE)
 * ============================================================ */

bno_status_t bno_read_9val(bno055_t *dev, bno_9val_t *out)
{
    if (!dev || !out)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_EULER_DATA, &out->yaw)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_EULER_DATA+2, &out->roll)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_EULER_DATA+4, &out->pitch)) return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_GYRO_DATA, &out->gx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GYRO_DATA+2, &out->gy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GYRO_DATA+4, &out->gz)) return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_ACCEL_DATA, &out->ax)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_ACCEL_DATA+2, &out->ay)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_ACCEL_DATA+4, &out->az)) return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_QUAT_DATA, &out->qw)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+2, &out->qx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+4, &out->qy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+6, &out->qz)) return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA, &out->gravx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA+2, &out->gravy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA+4, &out->gravz)) return BNO_FAIL;

    return BNO_OK;
}

/* ============================================================
 * MODULAR READ APIS (NEW)
 * ============================================================ */

bno_status_t bno_read_euler(bno055_t *dev,
                            int16_t *yaw,
                            int16_t *roll,
                            int16_t *pitch)
{
    if (!dev || !yaw || !roll || !pitch)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_EULER_DATA, yaw)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_EULER_DATA+2, roll)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_EULER_DATA+4, pitch)) return BNO_FAIL;

    return BNO_OK;
}

bno_status_t bno_read_gyro(bno055_t *dev,
                           int16_t *gx,
                           int16_t *gy,
                           int16_t *gz)
{
    if (!dev || !gx || !gy || !gz)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_GYRO_DATA, gx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GYRO_DATA+2, gy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GYRO_DATA+4, gz)) return BNO_FAIL;

    return BNO_OK;
}

bno_status_t bno_read_accel(bno055_t *dev,
                            int16_t *ax,
                            int16_t *ay,
                            int16_t *az)
{
    if (!dev || !ax || !ay || !az)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_ACCEL_DATA, ax)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_ACCEL_DATA+2, ay)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_ACCEL_DATA+4, az)) return BNO_FAIL;

    return BNO_OK;
}

bno_status_t bno_read_quat(bno055_t *dev,
                           int16_t *qw,
                           int16_t *qx,
                           int16_t *qy,
                           int16_t *qz)
{
    if (!dev || !qw || !qx || !qy || !qz)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_QUAT_DATA, qw)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+2, qx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+4, qy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_QUAT_DATA+6, qz)) return BNO_FAIL;

    return BNO_OK;
}

bno_status_t bno_read_gravity(bno055_t *dev,
                              int16_t *gx,
                              int16_t *gy,
                              int16_t *gz)
{
    if (!dev || !gx || !gy || !gz)
        return BNO_FAIL;

    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA, gx)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA+2, gy)) return BNO_FAIL;
    if (bno_rd16(dev, BNO_REG_GRAVITY_DATA+4, gz)) return BNO_FAIL;

    return BNO_OK;
}
