#ifndef BNO055_H
#define BNO055_H

#include <stdint.h>

/* ============================================================
 * IO ABSTRACTION
 * ============================================================ */

typedef struct
{
    void *ctx;

    int (*i2c_read)(void *ctx,
                    uint8_t addr,
                    uint8_t reg,
                    uint8_t *data,
                    uint16_t len);

    int (*i2c_write)(void *ctx,
                     uint8_t addr,
                     uint8_t reg,
                     uint8_t *data,
                     uint16_t len);

    void (*delay_ms)(uint32_t ms);

} bno055_io_t;

/* ============================================================
 * DEVICE STRUCT
 * ============================================================ */

typedef struct
{
    bno055_io_t io;
    uint8_t addr;   // 7-bit address (0x28 / 0x29)

} bno055_t;

/* ============================================================
 * STATUS
 * ============================================================ */

typedef enum
{
    BNO_OK = 0,
    BNO_FAIL,
    BNO_TIMEOUT,
    BNO_BAD_ID

} bno_status_t;

/* ============================================================
 * RAW DATA STRUCT
 * ============================================================ */

typedef struct
{
    int16_t yaw, roll, pitch;

    int16_t gx, gy, gz;
    int16_t ax, ay, az;

    int16_t qw, qx, qy, qz;

    int16_t gravx, gravy, gravz;

} bno_9val_t;

/* ============================================================
 * MODES
 * ============================================================ */

#define BNO_MODE_CONFIG   0x00
#define BNO_MODE_IMUPLUS  0x08

/* ============================================================
 * API
 * ============================================================ */

/* Init */
void bno_init(bno055_t *dev, bno055_io_t *io, uint8_t addr);

/* Chip */
bno_status_t bno_check_chip_id(bno055_t *dev);

/* Mode */
bno_status_t bno_set_mode(bno055_t *dev, uint8_t mode);
bno_status_t bno_set_and_verify_mode(bno055_t *dev, uint8_t mode);

/* Full read (backward compatible) */
bno_status_t bno_read_9val(bno055_t *dev, bno_9val_t *out);

/* Modular read APIs */
bno_status_t bno_read_euler(bno055_t *dev,
                            int16_t *yaw,
                            int16_t *roll,
                            int16_t *pitch);

bno_status_t bno_read_gyro(bno055_t *dev,
                           int16_t *gx,
                           int16_t *gy,
                           int16_t *gz);

bno_status_t bno_read_accel(bno055_t *dev,
                            int16_t *ax,
                            int16_t *ay,
                            int16_t *az);

bno_status_t bno_read_quat(bno055_t *dev,
                           int16_t *qw,
                           int16_t *qx,
                           int16_t *qy,
                           int16_t *qz);

bno_status_t bno_read_gravity(bno055_t *dev,
                              int16_t *gx,
                              int16_t *gy,
                              int16_t *gz);

#endif
