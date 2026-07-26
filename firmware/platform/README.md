# Platform Abstraction Layer (PAL)

## Overview

The **Platform Abstraction Layer (PAL)** provides a single interface between application code and the underlying hardware.

Instead of calling vendor libraries (HAL, LL, ESP-IDF, Arduino, etc.) directly, every module inside this library communicates with the hardware through the Platform API.

This design makes the library:

* Platform independent
* Easy to unit test
* Easy to port to new microcontrollers
* Independent of any specific SDK or vendor

---

## Design Philosophy

**Every hardware interaction must go through the Platform layer.**

```
Application
      │
      ▼
Modules / Services
      │
      ▼
 Platform API
      │
      ▼
HAL / LL / Driver Library
      │
      ▼
 Hardware
```

A module should never know whether it is running on

* STM32 HAL
* STM32 LL
* ESP32
* AVR
* Raspberry Pi Pico
* Linux
* Simulation

Its only responsibility is to call Platform functions.

---

# Platform Interface

```c
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

void Platform_Write(const char *data);

float Platform_ADC_Read(unsigned char channel);

void Platform_ADC_SetValue(unsigned char channel,
                           float value);

uint32_t Platform_Time_GetMs(void);

#endif
```

---

# Current Functions

## Platform_Write()

Used for all debug output.

Instead of

```c
printf("Battery Low");
```

write

```c
Platform_Write("Battery Low");
```

The implementation may internally use

* printf()
* UART
* USB CDC
* RTT
* SWO
* File logging

The caller should never know.

---

## Platform_ADC_Read()

Reads an ADC channel.

Example

```c
float voltage = Platform_ADC_Read(3);
```

---

## Platform_ADC_SetValue()

Mainly intended for testing and simulation.

Example

```c
Platform_ADC_SetValue(0, 3.2f);
```

On actual hardware this function may do nothing, while on a simulator it allows fake sensor values to be injected.

---

## Platform_Time_GetMs()

Returns the system uptime in milliseconds.

Example

```c
uint32_t start = Platform_Time_GetMs();

/* Do work */

uint32_t elapsed = Platform_Time_GetMs() - start;
```

This should be the preferred timing API for all modules.

---

# Rules

## ✅ DO

* Use Platform functions for every peripheral access.
* Keep all hardware-specific code inside Platform.
* Extend Platform whenever new hardware access is required.
* Keep interfaces generic and reusable.
* Ensure functions are independent of any vendor SDK.

---

## ❌ DON'T

Never call vendor libraries directly inside modules.

**Incorrect**

```c
HAL_ADC_GetValue(&hadc1);
HAL_UART_Transmit(...);
HAL_GPIO_WritePin(...);
printf(...);
```

These calls belong inside the Platform implementation only.

---

# Adding New Functionality

As the library grows, additional hardware support will be required.

**Do not bypass the Platform layer.**

Instead, extend the Platform interface.

Example:

Instead of writing

```c
HAL_GPIO_WritePin(...);
```

create

```c
void Platform_GPIO_Write(
    uint8_t port,
    uint8_t pin,
    uint8_t state);
```

---

Instead of

```c
HAL_I2C_Master_Transmit(...);
```

create

```c
Platform_I2C_Write(...);
```

---

Instead of

```c
HAL_Delay(100);
```

create

```c
Platform_DelayMs(100);
```

---

Instead of

```c
HAL_SPI_Transmit(...);
```

create

```c
Platform_SPI_Write(...);
```

Every new peripheral should first receive a Platform abstraction before being used anywhere else in the library.

---

# Porting to a New Platform

Only the Platform implementation should need modification.

Example:

```
platform/
│
├── stm32/
│     platform_stm32.c
|     platform_stm32.h
│
├── esp32/
│     platform_esp32.c
|     platform_esp32.h
│
|── linux/
     platform_linux.c
     platform_linux.h


```

No changes should be required in:

* modules/
* services/
* applications/
* tests

This allows the same library to compile across multiple targets.

---

# Benefits

Using the Platform layer provides:

* Hardware independence
* Easier unit testing
* Cleaner architecture
* Improved portability
* Reduced coupling to vendor SDKs
* Simpler maintenance
* Consistent coding practices across the team

---

# Team Guidelines

Before using any new peripheral, ask the following question:

> **"Can this be done through the Platform layer?"**

If the answer is **yes**, add a new Platform API first.

If a required function does not exist:

1. Add a generic function declaration to `platform.h`.
2. Implement it in the platform-specific source file.
3. Use the new Platform function throughout the library.
4. Never call vendor APIs directly from modules or services.

Following this rule ensures that the Central Library remains portable, maintainable, and independent of any single hardware platform.
