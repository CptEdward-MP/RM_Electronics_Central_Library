# 📘 Firmware Platform Rules

## 1. Purpose

This repository is a **long-term firmware platform**, not a collection of isolated projects.

The goal is to:

* Build reusable, high-quality firmware modules
* Improve code quality across generations
* Avoid rewriting the same drivers repeatedly

### Additional Requirements

* All firmware (except **platform** and **drivers**) MUST compile in **standard C (C99 or later)**

* Only standard C libraries (`stdint.h`, `stdbool.h`, etc.) and internal project headers are allowed

* No platform-specific headers (HAL, Arduino, etc.) are allowed outside the **platform layer**

* Every firmware/module MUST include a **tested document** with:

  * Testing method (how it was verified)
  * Hardware used
  * Known limitations
  * Failure conditions (if any)


All contributions must follow the rules defined in this document.

---

## 2. Architecture (MANDATORY)

All code must follow this layered structure:

```id="mqhrav"
Application
↓
Modules (PID, filters, logic)
↓
Devices (sensors, actuators)
↓
Drivers (I2C, SPI, GPIO)
↓
Platform (HAL / Arduino / etc.)
```

### ❌ Not Allowed

* Application calling HAL directly
* Drivers using HAL directly
* Mixing layers

### ✅ Required

* Strict separation of responsibilities
* Platform-independent design

---

## 3. Repository Structure

```id="tsdie4"
drivers/     → Hardware drivers (platform-independent)
devices/     → Sensors and actuators (BNO055, motors, etc.)
modules/     → Reusable algorithms (PID, filters)
platform/    → Platform-specific implementations (STM32, ESP32)
examples/    → Usage examples (MANDATORY)
docs/        → Documentation
```

---

## 4. Driver Rules

All drivers MUST:

* Be **platform independent**
* Use **IO abstraction (function pointers)**
* NOT use HAL or hardware-specific APIs
* Return status for every operation
* Avoid unnecessary blocking delays
* Contain only low-level hardware communication

### Example

```c id="argyqd"
typedef struct {
    void *ctx;
    int (*read)(...);
    int (*write)(...);
} io_t;
```

---

## 5. Device Rules (Sensors / Actuators)

### Sensors MUST:

* Provide `init()`
* Provide clean `read()` APIs
* Hide register-level details
* Return meaningful, usable data

### ❌ Not Allowed

* Exposing raw register access to application
* Forcing user to apply scaling manually

---

### Actuators MUST:

* Define safe initial state
* Provide control APIs (start, stop, set value)
* Not directly access hardware (use drivers)

---

## 6. Module Rules (Algorithms)

Modules (PID, filters, etc.) MUST:

* Be 100% hardware independent
* Contain no driver or HAL code
* Be reusable across projects

---

## 7. API Design Standard

All components must follow this structure:

```c id="9cts5x"
typedef struct {
    // configuration parameters
} module_config_t;

typedef struct {
    // runtime state
} module_handle_t;

status_t module_init(module_handle_t *h, module_config_t *cfg);
status_t module_execute(module_handle_t *h, ...);
```

---

## 8. Error Handling

All modules must use standard status codes:

```c id="3ugefb"
typedef enum {
    FW_OK = 0,
    FW_ERROR,
    FW_TIMEOUT,
    FW_INVALID
} fw_status_t;
```

### Rules:

* No silent failures
* All errors must be propagated
* Return values must NOT be ignored

---

## 9. Examples (MANDATORY)

Every module MUST include at least one example:

```id="i33s2o"
examples/<module_name>/
```

### PR will be rejected if:

* No example is provided

---

## 10. Documentation (MANDATORY)

Each module folder MUST include:

```id="7w094e"
README.md
```

### Must contain:

* What the module does
* How to use it
* Dependencies
* Example usage
* Testing details
* Known limitations

---

## 11. Git Workflow

### Branches

* `main` → stable, production-ready
* `dev` → integration branch
* `feature/<name>` → new features
* `fix/<name>` → bug fixes

---

### Rules

* ❌ No direct push to `main`
* ❌ No direct push to `dev`
* ✅ All changes via Pull Request (PR)

---

### Pull Request Requirements

Each PR MUST include:

* Description of changes
* Problem being solved
* Hardware tested? (Yes/No)
* Example added (MANDATORY)

---

## 12. Code Review Rules

PR will be rejected if:

* Uses HAL directly in driver
* No IO abstraction
* No documentation
* No example
* No testing details or limitations mentioned
* Hardcoded values
* Mixed abstraction layers

---

## 13. Naming Conventions

### Functions

```id="ymcst9"
sensor_init()
motor_set_speed()
i2c_read()
```

### Files

```id="bv0rzj"
module_name.c
module_name.h
```

---

## 14. Design Philosophy

* Code must be reusable
* Code must be testable
* Code must be hardware-independent
* Code must be understandable by future contributors

---

## 15. Long-Term Vision

This platform is designed to:

* Reduce development time for future teams
* Improve firmware quality over time
* Build a reusable embedded systems foundation

Every contribution should move the platform forward—not just solve a one-time problem.
