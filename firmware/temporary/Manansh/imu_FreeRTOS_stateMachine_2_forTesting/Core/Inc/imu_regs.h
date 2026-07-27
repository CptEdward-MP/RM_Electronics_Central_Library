/*
 * imu_regs.h
 *
 *  Created on: Feb 10, 2026
 *      Author: Manansh Pandey
 */

#ifndef INC_IMU_REGS_H_
#define INC_IMU_REGS_H_

/* ===================== BNO055 Registers ===================== */
#define BNO_ID				 0xA0

#define BNO_REG_CHIP_ID      0x00
#define BNO_REG_PAGE_ID      0x07

#define BNO_REG_ACCEL_DATA   0x08
#define BNO_REG_GYRO_DATA    0x14
#define BNO_REG_EULER_DATA   0x1A

#define BNO_REG_CALIB_STAT   0x35
#define BNO_REG_OPR_MODE     0x3D
#define BNO_REG_CALIB_DATA   0x55
#define BNO_REG_UNIT_SEL     0x3B

/* ===================== BNO055 Operation Modes ===================== */

#define BNO_MODE_CONFIG        0x00

#define BNO_MODE_ACCONLY       0x01
#define BNO_MODE_MAGONLY       0x02
#define BNO_MODE_GYRONLY       0x03
#define BNO_MODE_ACCMAG        0x04
#define BNO_MODE_ACCGYRO       0x05
#define BNO_MODE_MAGGYRO       0x06
#define BNO_MODE_AMG           0x07
#define BNO_MODE_IMUPLUS       0x08
#define BNO_MODE_COMPASS       0x09
#define BNO_MODE_M4G           0x0A
#define BNO_MODE_NDOF_FMC_OFF  0x0B
#define BNO_MODE_NDOF          0x0C


#define BNO_REG_QUAT_DATA     0x20
#define BNO_REG_LIA_DATA      0x28
#define BNO_REG_GRAVITY_DATA  0x2E
#endif /* INC_IMU_REGS_H_ */

