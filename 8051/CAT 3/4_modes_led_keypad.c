#include <reg51.h>

// LED Pin Definitions (P1.0 to P1.3)
sbit ledpin1 = P1^0;
sbit ledpin2 = P1^1;
sbit ledpin3 = P1^2;
sbit ledpin4 = P1^3;

// Switch Pin Definitions
sbit swrow  = P2^7;
sbit swicol = P2^3;

unsigned char mode = 0;

// Smart delay function: constantly checks if the button is pressed
unsigned char delay_and_check() {
    unsigned int i, j;
    for (i = 0; i < 300; i++) {
        for (j = 0; j < 300; j++) {
            // If button is pressed during delay, return 1 to signal early exit
            if (swicol == 0) {
                return 1; 
            }
        }
    }
    return 0; // Delay completed naturally without button press
}

// Mode 0: Running LED pattern (Chaser)
void runningled() {
    ledpin1 = 1; ledpin2 = 0; ledpin3 = 0; ledpin4 = 0; if (delay_and_check()) return;
    ledpin1 = 0; ledpin2 = 1; ledpin3 = 0; ledpin4 = 0; if (delay_and_check()) return;
    ledpin1 = 0; ledpin2 = 0; ledpin3 = 1; ledpin4 = 0; if (delay_and_check()) return;
    ledpin1 = 0; ledpin2 = 0; ledpin3 = 0; ledpin4 = 1; if (delay_and_check()) return;
}

// Mode 1: 4-bit Binary Counter (0000 to 1111)
void binarycounter() {
    unsigned char count;
    for (count = 0; count < 16; count++) {
        P1 = (P1 & 0xF0) | (count & 0x0F);
        if (delay_and_check()) return; // Instant break on button press
    }
}

// Mode 2: Alternating Pattern
void alternatingpattern() {
    ledpin1 = 1; ledpin2 = 0; ledpin3 = 1; ledpin4 = 0; if (delay_and_check()) return;
    ledpin1 = 0; ledpin2 = 1; ledpin3 = 0; ledpin4 = 1; if (delay_and_check()) return;
}

// Mode 3: Flashing All LEDs
void flash() {
    ledpin1 = 1; ledpin2 = 1; ledpin3 = 1; ledpin4 = 1; if (delay_and_check()) return;
    ledpin1 = 0; ledpin2 = 0; ledpin3 = 0; ledpin4 = 0; if (delay_and_check()) return;
}

void main() {
    swrow = 0;     // Drive switch row low
    swicol = 1;    // Set switch column pin as input
    
    P1 = P1 & 0xF0; // Clear lower 4 LEDs initially

    while (1) {
        // Execute pattern based on current mode
        switch (mode) {
            case 0: runningled(); break;
            case 1: binarycounter(); break;
            case 2: alternatingpattern(); break;
            case 3: flash(); break;
        }

        // Button handler: check if button was pressed
        if (swicol == 0) {
            // Debounce delay
            delay_and_check(); 
            if (swicol == 0) {
                mode++;
                if (mode > 3) {
                    mode = 0;
                }
                while (swicol == 0); // Wait until button is released
            }
        }
    }
}