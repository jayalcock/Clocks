# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A kinetic clock display system: 120 analog clocks arranged in an 8×15 grid, each with two stepper-motor-driven arms, coordinated to show time and animations. The system uses a master-slave architecture over CAN bus.

## Architecture

**Master (LPC1768)** — `1768 Clock Master/`
- `clockEngine.c` — time management, display logic, coordinates all slaves
- `can.c` — CAN bus protocol for sending commands to slave boards
- `uart_rb.c` — serial debug interface with ring buffer

**Slave (LPC11C24)** — two implementations:
- `11c24 Clock Slave/` — basic bare-metal firmware, single clock driver
- `11C24 Clock Slave RTOS/` — RTOS-based, drives multiple clocks per board
  - `clockDriverRT.c` — real-time motor control with hall-sensor homing
  - `ccan_rom.c` — CAN ROM API driver

**Common/** — shared clock data definitions (`clockData.c/h`): digit representations for the 8×15 grid

**Simulator/** — `clockSim.py`: Pygame visualization of the full display (Python 3.10+, requires pygame and numpy)

**ESP32/** — ESP32-C3 PlatformIO project (Arduino framework) for WiFi connectivity

**Elec Design/** — KiCad schematics and PCB layouts (Main Board v1.4, Slave Board v1.3)

## Build & Tools

- **Firmware**: Segger Embedded Studio — open `Clocks Project.hzp`. Build configurations include ARM/THUMB × Flash/RAM × Debug/Release
- **ESP32**: PlatformIO — `cd "ESP32/Clocks ESP32" && pio run`
- **Simulator**: `python3 Simulator/clockSim.py`
- **PCB**: KiCad 10 — project files in `Elec Design/`. Tag `pre-kicad10` is the last KiCad 9 state

## KiCad Conventions

- Current boards are `Elec Design/Main Board v1.4/` and `Elec Design/Slave Board v1.3/`; `Elec Design/Archive/` is history, don't edit it
- Shared project libraries live in `Elec Design/Libraries/`; reference them from project lib tables as `${KIPRJMOD}/../Libraries/...`, never absolute paths
- Global user libraries are referenced via `${MY_LIBS}` (set in KiCad Preferences → Configure Paths)
- 3D models should live in each board's `3D Models/` folder and be referenced via `${KIPRJMOD}`
- `.kicad_prl`, `fp-info-cache`, `*-bak` and `*-backups/` are per-user/generated and gitignored

## Key Technical Details

- CAN bus addressing scheme documented in `Documents/CAN Data structure.numbers`
- Clock numbering/layout in `Documents/Clock numbering.numbers`
- Each slave board controls multiple clocks; each clock has hour and minute arms driven by steppers with hall-effect home sensors
- Device drivers use CMSIS and LPC ROM APIs (not vendor HALs)
- Memory maps: `LPC11C24FBD48_301_MemoryMap.xml`, `LPC1768_MemoryMap.xml`

## Known Bugs

Documented in `improvements.md` — includes a GPIO pin/port mixup in `clockDriverRT.c` for Clock 3's hour hall sensor, and a missing struct field in a motor data initializer.
