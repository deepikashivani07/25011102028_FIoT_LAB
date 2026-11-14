# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 5 – TASK 2A

**DATE:** 10/08/2026

**NAME OF EXP:** Hardware Timer and Interrupt-Based Software PWM for LED Fading

**HARDWARE SETUP / PLATFORM:** 8051 Microcontroller (AT89S52) + Keil µVision (Keil C51 Compiler)

---

## TASK 2A: LED Fading Using Timer-Driven Software PWM

### 1. Circuit Diagram

AT89S52 development board configured with a crystal oscillator ($11.0592\text{ MHz}$) and an output LED connected to a GPIO pin via a current-limiting resistor. Circuit diagram attached.

### 2. Hardware & PWM Duty Cycle Specifications

| Parameter | Student Entry |
| --- | --- |
| MCU | AT89S52 |
| CPU Core | 8-bit 8051 ($12$-clock architecture) |
| Crystal Frequency | $11.0592\text{ MHz}$ ($T_{\text{machine}} \approx 1.085\ \mu\text{s}$) |
| Operating Voltage | $5\text{V}$ |
| Timer Used | Timer 0 (16-bit Mode 1) |
| Suggested PWM Frequency | $500\text{ Hz} – 1\text{ kHz}$ (non-flickering persistence) |
| Duty Cycle Progression | $0\% \rightarrow 10\% \rightarrow \dots \rightarrow 100\% \rightarrow \dots \rightarrow 10\% \rightarrow 0\%$ |
| Brightness Mapping | $0\%$ (OFF), $25\%$ (Dim), $50\%$ (Medium), $75\%$ (Bright), $100\%$ (Fully ON) |

### 3. Core Logic / Pseudocode

1. Include the required 8051 register header file (`reg51.h` / `reg52.h`).
2. Configure Timer 0 in Mode 1 (16-bit timer) and load initial timer values (`TH0`, `TL0`) for periodic interrupt generation.
3. Enable Timer 0 interrupt (`ET0 = 1`) and global interrupts (`EA = 1`), then start the timer (`TR0 = 1`).
4. Inside the Timer 0 ISR, reload the timer registers and increment a software PWM tick counter to maintain a non-blocking time base.
5. Compare the tick counter against the current duty-cycle threshold to toggle the LED pin HIGH or LOW.
6. Periodically increment the duty cycle from $0\%$ to $100\%$ and decrement it back to $0\%$ to create a smooth fading effect without using blocking delay loops.

### 4. Program Parameters

| Parameter | Value |
| --- | --- |
| Microcontroller | AT89S52 |
| Source File | `timer_interrupt.c` |
| Project File | `timer_interrupt.uvproj` |
| Timer Preload ($1\text{ ms}$ base) | `TH0 = 0xFC; TL0 = 0x66;` ($N \approx 922$ counts) |
| Timing Mechanism | Hardware Timer 0 Interrupt (Non-Blocking) |
| Programming Language | Bare-Metal Embedded C (Direct Register Access) |
| Restrictions | Strictly No Arduino APIs, HAL libraries, or RTOS services |
| Verification Tool | Oscilloscope / Multimeter |

### 5. Inference

Successfully implemented a non-blocking, timer-interrupt-driven software PWM controller on the AT89S52 microcontroller. By dynamically varying the duty cycle between $0\%$ and $100\%$ at a high switching frequency ($500\text{ Hz} – 1\text{ kHz}$), smooth LED brightness fading was achieved without using blocking software delay loops.