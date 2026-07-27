#ifndef IMU_LOGIC_H
#define IMU_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include "bno055.h"

/* ============================================================
 * CONFIG
 * ============================================================ */

#define IMU_ENABLE_CALIBRATION  0

/* ============================================================
 * STATES
 * ============================================================ */

typedef enum {
    IMU_STATE_UNINIT = 0,
    IMU_STATE_INIT,
    IMU_STATE_READY,
    IMU_STATE_ERROR,
    IMU_STATE_DISCONNECTED
} imu_state_t;

/* ============================================================
 * STATUS
 * ============================================================ */

typedef enum {
    IMU_STATUS_OK = 0,
    IMU_STATUS_NOT_READY,
    IMU_STATUS_TIMEOUT,
    IMU_STATUS_COMM_ERROR,
    IMU_STATUS_BAD_ID,
    IMU_STATUS_HW_ERROR,
    IMU_STATUS_INVALID_ARG
} imu_status_t;

/* ============================================================
 * ERRORS
 * ============================================================ */

typedef enum {
    IMU_ERROR_NONE = 0,
    IMU_ERROR_COMM_FAIL,
    IMU_ERROR_BAD_ID,
    IMU_ERROR_INIT_FAIL,
    IMU_ERROR_MODE_FAIL,
    IMU_ERROR_READ_FAIL,
    IMU_ERROR_BAD_CALIB,
    IMU_ERROR_DATA_SANITY_FAIL,
    IMU_ERROR_CALIB_FAIL,
} imu_error_t;

/* ============================================================
 * SCALED DATA
 * ============================================================ */

typedef struct {

    float yaw, roll, pitch;
    float ax, ay, az;
    float gx, gy, gz;
    float gravx, gravy, gravz;
    float qw, qx, qy, qz;

    uint32_t exec_time_ms;
    uint32_t wcet_ms;
    uint32_t deadline_miss;
    uint32_t imu_us;
    uint32_t period_jitter_ms;

    uint8_t state;
    uint8_t calibrated;

} imu_9val_scaled_t;

/* ============================================================
 * INSTANCE STRUCT
 * ============================================================ */

typedef struct
{
    bno055_t *bno;

    imu_state_t  state;
    imu_error_t  error;
    imu_status_t status;

    bool calibrated;

    uint8_t sanity_fail_count;
    uint8_t comm_fail_count;

    imu_9val_scaled_t snapshot;

} imu_t;

/* ============================================================
 * API
 * ============================================================ */

imu_status_t imu_init(imu_t *imu);
imu_status_t imu_poll(imu_t *imu);
imu_status_t imu_read(imu_t *imu, imu_9val_scaled_t *out);

imu_state_t  imu_state_get(imu_t *imu);
imu_status_t imu_status_get(imu_t *imu);
imu_error_t  imu_error_get(imu_t *imu);

bool imu_is_calibrated(imu_t *imu);

#endif
