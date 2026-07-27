#include "imu_logic.h"
#include <math.h>

/* ============================================================
 * CONFIG
 * ============================================================ */

#define SANITY_FAIL_THRESHOLD 10
#define COMM_FAIL_THRESHOLD   3

#define DEG_SCALE  (1.0f / 16.0f)
#define GYRO_SCALE (1.0f / 16.0f)
#define ACC_SCALE  (9.80665f / 1000.0f)

/* ============================================================
 * HELPER
 * ============================================================ */

static imu_status_t imu_fail(imu_t *imu,
                            imu_error_t err,
                            imu_status_t status)
{
    imu->error  = err;
    imu->status = status;
    return status;
}

/* ============================================================
 * INIT
 * ============================================================ */

imu_status_t imu_init(imu_t *imu)
{
    if (!imu || !imu->bno)
        return IMU_STATUS_INVALID_ARG;

    imu->sanity_fail_count = 0;
    imu->comm_fail_count   = 0;

    imu->state  = IMU_STATE_INIT;
    imu->error  = IMU_ERROR_NONE;
    imu->status = IMU_STATUS_NOT_READY;

    /* Check communication */
    bno_status_t id = bno_check_chip_id(imu->bno);

    if (id == BNO_FAIL)
    {
        imu->state = IMU_STATE_DISCONNECTED;
        return imu_fail(imu, IMU_ERROR_COMM_FAIL, IMU_STATUS_TIMEOUT);
    }

    if (id == BNO_BAD_ID)
    {
        imu->state = IMU_STATE_ERROR;
        return imu_fail(imu, IMU_ERROR_BAD_ID, IMU_STATUS_BAD_ID);
    }

    /* Set CONFIG mode */
    if (bno_set_and_verify_mode(imu->bno, BNO_MODE_CONFIG) != BNO_OK)
        return imu_fail(imu, IMU_ERROR_MODE_FAIL, IMU_STATUS_HW_ERROR);

    imu->bno->io.delay_ms(50);

    /* Set IMU mode */
    if (bno_set_and_verify_mode(imu->bno, BNO_MODE_IMUPLUS) != BNO_OK)
        return imu_fail(imu, IMU_ERROR_MODE_FAIL, IMU_STATUS_HW_ERROR);

    imu->bno->io.delay_ms(100);

    imu->state = IMU_STATE_READY;
    return (imu->status = IMU_STATUS_OK);
}

/* ============================================================
 * POLL
 * ============================================================ */

imu_status_t imu_poll(imu_t *imu)
{
    if (!imu || imu->state != IMU_STATE_READY)
        return IMU_STATUS_NOT_READY;

    /* Communication check */
    if (bno_check_chip_id(imu->bno) != BNO_OK)
    {
        imu->comm_fail_count++;

        if (imu->comm_fail_count >= COMM_FAIL_THRESHOLD)
        {
            imu->state = IMU_STATE_DISCONNECTED;
            imu->comm_fail_count = 0;
            return imu_fail(imu, IMU_ERROR_COMM_FAIL, IMU_STATUS_TIMEOUT);
        }

        return imu->status = IMU_STATUS_OK;
    }

    imu->comm_fail_count = 0;

    bno_9val_t raw;

    if (bno_read_9val(imu->bno, &raw) != BNO_OK)
    {
        imu->comm_fail_count++;

        if (imu->comm_fail_count >= COMM_FAIL_THRESHOLD)
        {
            imu->state = IMU_STATE_DISCONNECTED;
            imu->comm_fail_count = 0;
            return imu_fail(imu, IMU_ERROR_COMM_FAIL, IMU_STATUS_TIMEOUT);
        }

        return imu->status = IMU_STATUS_OK;
    }

    /* ============================================================
     * SCALING
     * ============================================================ */

    imu->snapshot.yaw   = raw.yaw   * DEG_SCALE;
    imu->snapshot.roll  = raw.roll  * DEG_SCALE;
    imu->snapshot.pitch = raw.pitch * DEG_SCALE;

    imu->snapshot.gx = raw.gx * GYRO_SCALE;
    imu->snapshot.gy = raw.gy * GYRO_SCALE;
    imu->snapshot.gz = raw.gz * GYRO_SCALE;

    imu->snapshot.ax = raw.ax * ACC_SCALE;
    imu->snapshot.ay = raw.ay * ACC_SCALE;
    imu->snapshot.az = raw.az * ACC_SCALE;

    imu->snapshot.gravx = raw.gravx * ACC_SCALE;
    imu->snapshot.gravy = raw.gravy * ACC_SCALE;
    imu->snapshot.gravz = raw.gravz * ACC_SCALE;

    imu->snapshot.qw = raw.qw / 16384.0f;
    imu->snapshot.qx = raw.qx / 16384.0f;
    imu->snapshot.qy = raw.qy / 16384.0f;
    imu->snapshot.qz = raw.qz / 16384.0f;

    /* ============================================================
     * SANITY CHECK
     * ============================================================ */

    float g = sqrtf(imu->snapshot.gravx * imu->snapshot.gravx +
                    imu->snapshot.gravy * imu->snapshot.gravy +
                    imu->snapshot.gravz * imu->snapshot.gravz);

    float q = sqrtf(imu->snapshot.qw * imu->snapshot.qw +
                    imu->snapshot.qx * imu->snapshot.qx +
                    imu->snapshot.qy * imu->snapshot.qy +
                    imu->snapshot.qz * imu->snapshot.qz);

    if (g < 6.0f || g > 13.0f || q < 0.7f || q > 1.3f)
    {
        imu->sanity_fail_count++;

        if (imu->sanity_fail_count >= SANITY_FAIL_THRESHOLD)
        {
            imu->state = IMU_STATE_ERROR;
            imu->sanity_fail_count = 0;
            return imu_fail(imu, IMU_ERROR_DATA_SANITY_FAIL,
                            IMU_STATUS_HW_ERROR);
        }

        return imu->status = IMU_STATUS_OK;
    }

    imu->sanity_fail_count = 0;
    return imu->status = IMU_STATUS_OK;
}

/* ============================================================
 * READ
 * ============================================================ */

imu_status_t imu_read(imu_t *imu, imu_9val_scaled_t *out)
{
    if (!imu || !out)
        return IMU_STATUS_INVALID_ARG;

    if (imu->state != IMU_STATE_READY)
        return IMU_STATUS_NOT_READY;

    *out = imu->snapshot;
    return IMU_STATUS_OK;
}

/* ============================================================
 * ACCESSORS
 * ============================================================ */

imu_state_t imu_state_get(imu_t *imu)
{
    return imu ? imu->state : IMU_STATE_ERROR;
}

imu_status_t imu_status_get(imu_t *imu)
{
    return imu ? imu->status : IMU_STATUS_HW_ERROR;
}

imu_error_t imu_error_get(imu_t *imu)
{
    return imu ? imu->error : IMU_ERROR_NONE;
}

bool imu_is_calibrated(imu_t *imu)
{
    return imu ? imu->calibrated : false;
}
