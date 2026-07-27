# Deterministic IMU Firmware Framework – RoboCup 2026

A layered, state-machine driven IMU firmware architecture designed for
competition-grade reliability and hardware portability.

---

## Purpose

This repository contains the official IMU firmware framework used for RoboCup 2026.

The system is designed to:

- Be deterministic and predictable
- Use strict layered separation
- Isolate hardware dependencies
- Allow easy MCU replacement
- Allow easy IMU sensor replacement
- Allow control logic modification without breaking lower layers
- Support both RTOS and baremetal execution



---

## Design Philosophy

The firmware follows a strict layered model:

Application  
↓  
Supervisor  
↓  
Driver  
↓  
Sensor Protocol  
↓  
Hardware Abstraction  

Each layer has clearly defined responsibilities and forbidden actions.

No cross-layer violations are allowed.

---

## Portability Goals

This framework allows:

✔ Swapping MCU (e.g., STM32F4 → STM32H7)  
✔ Swapping IMU sensor (e.g., BNO055 → ICM20948)  
✔ Replacing control algorithms without touching hardware  
✔ Running with or without RTOS  
✔ Isolating hardware drivers from application logic  

Only the Hardware Abstraction Layer must change when porting MCU.

Only the Protocol Layer must change when porting IMU.

Higher layers remain untouched.

---

## Repository Structure
