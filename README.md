# System on Chip Labs
This repository contains the C code implementations and functions definitions for multiple projects using the PYNQ-Z2 FPGA for the System on Chip course.

The following projects are contained in each of the folders:
- `lab_1_1_basic_gpio`: contains the C implementation for Lab 1.1, which makes use of the basic GPIO I/O API functions, using push buttons to write to the board's LEDs and switches to implement a blinking function.
- `lab_1_2_gpio_interrupts`: Lab 1.2 implementation. Builds on Lab 1.1 hardware and adds an additional GPIO channel with interrupts enabled to change the behaviour of the program when a hardware interrupt is triggered.
