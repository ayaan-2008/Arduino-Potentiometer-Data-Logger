# Arduino Potentiometer Data Logger

## Overview

This project demonstrates Dynamic Memory Allocation on Arduino by creating a data logger for potentiometer readings.

The user specifies the number of readings to store, and the program dynamically allocates only the required memory using C++ pointers and heap allocation.

## Features

- Potentiometer interfacing
- Analog-to-Digital Conversion (ADC)
- Dynamic Memory Allocation
- Data Logging
- Statistical Analysis
- Memory Deallocation
- Serial Monitor Interface

## Hardware

- Arduino Uno
- Potentiometer
- USB Serial Monitor

## Concepts Demonstrated

- Pointers
- Heap Memory
- Dynamic Arrays
- ADC (Analog-to-Digital Conversion)
- Sensor Data Logging
- Memory Management

## How It Works

1. User specifies the number of readings.
2. Memory is dynamically allocated.
3. Potentiometer values are recorded.
4. Data is stored in RAM.
5. Statistics are calculated:
   - Minimum
   - Maximum
   - Average
6. Memory is released when logging is complete.

## Hardware Connections

Potentiometer:

Left Pin   → GND
Middle Pin → A0
Right Pin  → 5V

## Author

Abdul Ayaan
