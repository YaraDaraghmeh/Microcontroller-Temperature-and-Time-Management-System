# Microcontroller Temperature and Time Management System

## Project Overview
This embedded system project uses a PIC microcontroller to create a multipurpose temperature and time management device with interactive LCD display and serial communication capabilities.

## Features
- Real-time clock with hour, minute, and second tracking
- Temperature monitoring using ADC
- Mode switching between Normal and Setup modes
- Interactive buttons for:
  - Time adjustment
  - Mode selection
  - Cooler/Heater control
- Serial communication for remote monitoring and control
- LCD display showing current time, temperature, and system status

## Hardware Requirements
- PIC Microcontroller (18F series)
- 4 MHz Crystal Oscillator
- LCD Display
- Push buttons
- Temperature sensor
- Serial communication interface

## Peripheral Configurations
- ADC configured for temperature reading
- Timer0 for timekeeping
- Interrupts for button interactions
- Serial communication setup
- LCD interface

## Communication Commands
- `t`: Request current time
- `T`: Request current temperature
- `s`: Request system status (Heater/Cooler)
- Time update via serial input in `HH:MM:SS` format

## Mode Functionality
### Normal Mode
- Automatic time progression
- Heater/Cooler toggle capability

### Setup Mode
- Manually adjust time (hours, minutes, seconds)
- Button-based incrementation/decrementation

## Button Interactions
- RB1: Toggle between Normal/Setup modes
- RB2: Cycle through time adjustment targets
- RB3: Increment values
- RB4: Decrement values
- RB0: Cooler toggle

## Authors
Yara and Deema

## License
Open-source project
