#include "imu_logic.h"
#include "imu_hw.h"
#include "board_init.h"

#include "FreeRTOS.h"
#include "task.h"

void imu_supervisor_task(void *arg)
{
    /* ✅ GET INSTANCES */
    imu_t *imu = imu_get();
    hw_i2c_t *bus = i2c_bus_get();

    for (;;)
    {
        imu_state_t state = imu_state_get(imu);

        switch (state)
        {
            case IMU_STATE_UNINIT:
            case IMU_STATE_ERROR:
            case IMU_STATE_DISCONNECTED:

                hw_i2c_recover(bus);
                vTaskDelay(pdMS_TO_TICKS(100));

                imu_init(imu);
                vTaskDelay(pdMS_TO_TICKS(300));
                break;

            case IMU_STATE_INIT:
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case IMU_STATE_READY:
            default:
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
        }
    }
}
