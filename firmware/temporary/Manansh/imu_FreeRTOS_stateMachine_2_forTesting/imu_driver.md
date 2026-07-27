/*
 * imu_driver.c
 *
 * IMU Driver Layer
 * -----------------
 *
 * Purpose:
 * --------
 * Implements deterministic state-machine control of the IMU.
 *
 * This layer converts raw sensor protocol access into validated,
 * scaled, and application-ready data.
 *
 * Responsibilities:
 * ------------------
 * - Initialization sequence
 * - Mode configuration
 * - Periodic polling
 * - Runtime fault filtering
 * - Data scaling
 * - Data sanity validation
 * - Snapshot management
 *
 * This layer does NOT:
 * - Access MCU hardware directly
 * - Perform register-level communication
 * - Include HAL headers
 * - Contain application logic
 *
 *
 * Architectural Position:
 * ------------------------
 * Application
 *    ↓
 * Supervisor
 *    ↓
 * Driver  ← This file
 *    ↓
 * Protocol (bno055.c)
 *    ↓
 * Registers
 *    ↓
 * Hardware
 *
 *
 * Design Notes:
 * -------------
 * - All hardware communication is delegated to Protocol layer.
 * - Runtime failures are filtered using threshold counters.
 * - State transitions are explicit and deterministic.
 * - Snapshot data is updated only after successful validation.
 *
 *
 * Portability:
 * ------------
 * This file must NOT change if:
 *   - MCU changes
 *   - Board design changes
 *   - I2C implementation changes
 *
 * This file changes only if:
 *   - Sensor protocol changes
 *   - Driver behavior changes
 *
 */

/*
 * ============================================================
 * HARDWARE COMPATIBILITY REQUIREMENTS
 * ============================================================
 *
 * This driver depends only on the Protocol layer (bno055.c).
 *
 * It must not:
 *   - Include hardware headers
 *   - Call HAL_I2C functions
 *   - Use GPIO functions
 *
 * Timing must be abstracted through time_service.
 *
 * If hardware changes, only imu_hw.c must be updated.
 * No changes are required in this file.
 *
 */

/*
 * ============================================================
 * SENSOR COMPATIBILITY GUIDELINES
 * ============================================================
 *
 * This file implements the Sensor Protocol Layer.
 *
 * To ensure compatibility with the overall firmware architecture,
 * the following rules must be respected:
 *
 * 1. This file must communicate ONLY through imu_hw.
 *      - hw_read_reg()
 *      - hw_write_reg()
 *      - hw_write_multi()
 *      - hw_probe_device()
 *
 * 2. This file must NOT:
 *      - Include MCU HAL headers
 *      - Access I2C handles directly
 *      - Use GPIO functions
 *      - Call HAL_I2C_* functions
 *
 * 3. All register addresses and bit definitions must come from
 *    the Register Definition Layer (imu_regs.h).
 *
 * 4. This file may:
 *      - Parse raw register values
 *      - Implement configuration sequences
 *      - Verify chip ID
 *      - Format sensor data structures
 *
 * 5. This file must NOT:
 *      - Contain state machines
 *      - Implement filtering logic
 *      - Perform physics validation
 *      - Implement fault recovery logic
 *
 *
 * ============================================================
 * HOW TO REPLACE THIS SENSOR
 * ============================================================
 *
 * To replace BNO055 with another IMU:
 *
 * 1. Create a new protocol file (e.g., icm20948.c).
 * 2. Implement equivalent functions:
 *        - sensor_check_id()
 *        - sensor_read_data()
 *        - sensor_set_mode()
 * 3. Use the same imu_hw interface.
 * 4. Keep driver interface unchanged.
 *
 * No changes should be required in:
 *      - imu_hw.c
 *      - imu_driver.c
 *      - imu_supervisor.c
 *
 */