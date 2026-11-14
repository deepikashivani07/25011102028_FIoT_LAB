# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESMENT 2

**DATE:** 20/07/2026

**NAME OF EXP:** 8051 Tool Chain Setup – Development Board – AT89S52

**HARDWARE SETUP / PLATFORM:** 8051 Microcontroller + Keil µVision (Keil C51 Compiler)

---

## TASK 1: Hardware Board Bring-Up

### 1. Circuit Diagram

AT89S52 development board with an LED connected to Port 1, Pin 0 (P1.0). Circuit diagram attached.

### 2. Hardware Bring-Up Details

| Parameter         | Student Entry                          |
| ----------------- | -------------------------------------- |
| MCU               | AT89S52                                |
| CPU Core          | 8-bit 8051                             |
| Operating Voltage | 5V                                     |
| Debug Interface   | Programmer interface (board-dependent) |
| LED GPIO Port     | Port 1                                 |
| LED GPIO Pin      | P1.0                                   |
| LED Active State  | HIGH/LOW (depends on LED wiring)       |
| LED Frequency     | Approximately 1 Hz (target)            |

---

## TASK 2: Bare-Metal LED Blink

### 1. Circuit Diagram

LED connected to P1.0 of the AT89S52 microcontroller through a current-limiting resistor.

### 2. Core Logic / Pseudocode

1. Include the required 8051 register header file.
2. Define P1.0 as the LED output pin.
3. Create a delay function using nested for loops.
4. Set the LED pin HIGH.
5. Call the delay function.
6. Set the LED pin LOW.
7. Call the delay function again.
8. Repeat continuously using an infinite while loop.

### 4. Program Parameters

| Parameter                   | Value                  |
| --------------------------- | ---------------------- |
| Microcontroller             | AT89S52                |
| Header File                 | reg51.h                |
| LED Pin                     | P1.0                   |
| Delay Function              | delay()                |
| Outer Loop                  | 500 iterations         |
| Inner Loop                  | 500 iterations         |
| Total Inner Loop Iterations | 250,000 per delay call |
| LED ON Command              | ledpin = 1             |
| LED OFF Command             | ledpin = 0             |
| Execution                   | Infinite loop          |
| Programming Language        | Embedded C             |

### 5. Inference

Successfully implemented a bare-metal LED blinking program on the AT89S52 microcontroller using direct port access. The LED connected to P1.0 is toggled continuously using a software delay generated through nested loops.



