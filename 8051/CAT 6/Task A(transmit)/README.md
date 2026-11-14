# EXPERIMENT DETAILS

**EXP:** CONTINUOUS ASSESSMENT 6

**DATE:** 17/08/2026

**NAME OF EXP:** UART Driver and Serial Debugging – Task A: UART Initialization and Character Transmission

**HARDWARE SETUP / PLATFORM:** 8051 Microcontroller (AT89S52) + Keil µVision (Keil C51 Compiler) + USB-to-TTL Converter + PuTTY

---

## TASK A: UART Initialization and Character Transmission

### 1. Circuit Diagram

AT89S52 development board connected to a PC via a USB-to-TTL serial converter. The microcontroller's TXD pin (`P3.1`) is connected to the RX pin of the USB-to-TTL adapter, with a common GND connection established between the board and converter. Circuit diagram attached.

### 2. Core Logic / Pseudocode

1. Include the required 8051 register header file (`reg51.h` / `reg52.h`).
2. Configure Timer 1 in Mode 2 (8-bit auto-reload mode) by setting `TMOD = 0x20`.
3. Load `TH1 = 0xFD` (and `TL1 = 0xFD`) to generate a $9600\text{ baud}$ rate using an $11.0592\text{ MHz}$ crystal oscillator.
4. Configure the serial control register (`SCON = 0x50`) for UART Mode 1 (8-bit variable baud rate) with reception enabled.
5. Start Timer 1 (`TR1 = 1`) to begin baud-rate clock generation.
6. Create a character transmit function that writes a byte into `SBUF`, polls the Transmit Interrupt flag (`TI`) until transmission completes (`while (TI == 0)`), and clears `TI = 0` in software.
7. Create a string transmit function to send `"Hello from 8051\r\n"` character by character.
8. Continuously transmit the string to the PC terminal inside an infinite `while(1)` loop.

### 3. Program Parameters

| Parameter                    | Value                                       |
| ---------------------------- | ------------------------------------------- |
| Microcontroller              | AT89S52                                     |
| Crystal Frequency            | $11.0592\text{ MHz}$                        |
| Header File                  | `reg51.h` / `reg52.h`                       |
| UART Pins                    | TXD (`P3.1`), RXD (`P3.0`)                  |
| Serial Mode (`SCON`)         | `0x50` (Mode 1: 8-bit UART, REN = 1)        |
| Timer Configuration (`TMOD`) | `0x20` (Timer 1, Mode 2: 8-bit Auto-Reload) |
| Baud Rate Reload (`TH1`)     | `0xFD` ($9600\text{ bps}$)                  |
| Framing Format               | $9600\text{-}8\text{-N-}1$ (8 data, No parity, 1 stop bit) |
| Serial Buffer Register       | `SBUF`                                      |
| Transmit Completion Flag     | `TI` (cleared manually in software)         |
| Transmitted Message          | `"Hello from 8051"`                         |
| Execution                    | Infinite loop                               |
| Programming Language         | Embedded C (Bare-Metal)                     |

### 4. Inference

Successfully implemented a bare-metal UART serial transmitter on the AT89S52 microcontroller using Timer 1 in 8-bit auto-reload mode to achieve an accurate $9600\text{ baud}$ rate. The string `"Hello from 8051"` is transmitted continuously over the USB-to-TTL interface and verified on the PuTTY serial terminal.