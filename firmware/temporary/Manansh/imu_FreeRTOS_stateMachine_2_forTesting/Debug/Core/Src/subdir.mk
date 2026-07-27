################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_tasks.c \
../Core/Src/bno055.c \
../Core/Src/board_init.c \
../Core/Src/cJSON.c \
../Core/Src/freertos.c \
../Core/Src/imu_hw.c \
../Core/Src/imu_logic.c \
../Core/Src/imu_supervisor.c \
../Core/Src/imu_task.c \
../Core/Src/main.c \
../Core/Src/pcb.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/time_service.c \
../Core/Src/usb_task.c 

OBJS += \
./Core/Src/app_tasks.o \
./Core/Src/bno055.o \
./Core/Src/board_init.o \
./Core/Src/cJSON.o \
./Core/Src/freertos.o \
./Core/Src/imu_hw.o \
./Core/Src/imu_logic.o \
./Core/Src/imu_supervisor.o \
./Core/Src/imu_task.o \
./Core/Src/main.o \
./Core/Src/pcb.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/time_service.o \
./Core/Src/usb_task.o 

C_DEPS += \
./Core/Src/app_tasks.d \
./Core/Src/bno055.d \
./Core/Src/board_init.d \
./Core/Src/cJSON.d \
./Core/Src/freertos.d \
./Core/Src/imu_hw.d \
./Core/Src/imu_logic.d \
./Core/Src/imu_supervisor.d \
./Core/Src/imu_task.d \
./Core/Src/main.d \
./Core/Src/pcb.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/time_service.d \
./Core/Src/usb_task.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_tasks.cyclo ./Core/Src/app_tasks.d ./Core/Src/app_tasks.o ./Core/Src/app_tasks.su ./Core/Src/bno055.cyclo ./Core/Src/bno055.d ./Core/Src/bno055.o ./Core/Src/bno055.su ./Core/Src/board_init.cyclo ./Core/Src/board_init.d ./Core/Src/board_init.o ./Core/Src/board_init.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/imu_hw.cyclo ./Core/Src/imu_hw.d ./Core/Src/imu_hw.o ./Core/Src/imu_hw.su ./Core/Src/imu_logic.cyclo ./Core/Src/imu_logic.d ./Core/Src/imu_logic.o ./Core/Src/imu_logic.su ./Core/Src/imu_supervisor.cyclo ./Core/Src/imu_supervisor.d ./Core/Src/imu_supervisor.o ./Core/Src/imu_supervisor.su ./Core/Src/imu_task.cyclo ./Core/Src/imu_task.d ./Core/Src/imu_task.o ./Core/Src/imu_task.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pcb.cyclo ./Core/Src/pcb.d ./Core/Src/pcb.o ./Core/Src/pcb.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/time_service.cyclo ./Core/Src/time_service.d ./Core/Src/time_service.o ./Core/Src/time_service.su ./Core/Src/usb_task.cyclo ./Core/Src/usb_task.d ./Core/Src/usb_task.o ./Core/Src/usb_task.su

.PHONY: clean-Core-2f-Src

