#include <reg51.h>

#define LED P1          // 4 LEDs connected to P1.0 - P1.3
#define SEG P0          // 7-Segment Display on Port 0

// Keypad Column Pin Definitions (Inputs)
sbit col1 = P2^3;
sbit col2 = P2^2;
sbit col3 = P2^1;

// Seven-segment cathode code array (0-9)
unsigned char code seg_cod[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Software delay
void delay(void)
{
    unsigned int i, j;
    for(i = 0; i < 200; i++)
        for(j = 0; j < 200; j++);
}

void display_digit(unsigned char value)
{
    if (value <= 9)
        SEG = seg_cod[value];
}

unsigned char keypad_scan(void)
{
    // Row 1 Check (P2.7 = 0)
    P2 = 0x7F; 
    if(col1 == 0) { delay(); display_digit(1); while(col1 == 0); return 1; }
    if(col2 == 0) { delay(); display_digit(2); while(col2 == 0); return 2; }
    if(col3 == 0) { delay(); display_digit(3); while(col3 == 0); return 3; }

    // Row 2 Check (P2.6 = 0)
    P2 = 0xBF; 
    if(col1 == 0) { delay(); display_digit(4); while(col1 == 0); return 4; }
    if(col2 == 0) { delay(); display_digit(5); while(col2 == 0); return 5; }
    if(col3 == 0) { delay(); display_digit(6); while(col3 == 0); return 6; }

    // Row 3 Check (P2.5 = 0)
    P2 = 0xDF; 
    if(col1 == 0) { delay(); display_digit(7); while(col1 == 0); return 7; }
    if(col2 == 0) { delay(); display_digit(8); while(col2 == 0); return 8; }
    if(col3 == 0) { delay(); display_digit(9); while(col3 == 0); return 9; }

    // Row 4 Check (P2.4 = 0)
    P2 = 0xEF; 
    if(col2 == 0) { delay(); display_digit(0); while(col2 == 0); return 0; }

    return 0xFF; // No key pressed
}

void running_led(void)
{
    unsigned char k;
    for(k = 0; k < 4; k++)
    {
        LED = (1 << k); // Fixed Active-HIGH logic
        delay();
    }
}

void binary_counter(void)
{
    unsigned char k;
    for(k = 0; k < 16; k++)
    {
        LED = k;
        delay();
    }
}

void alternate_led(void)
{
    LED = 0x0A;
    delay();
    LED = 0x05;
    delay();
}

void blink_all(void)
{
    LED = 0x0F;
    delay();
    LED = 0x00;
    delay();
}

void main(void)
{
    unsigned char key;

    // Optional: Turn off LEDs and Display initially
    LED = 0x00;
    SEG = 0x00;

    while(1)
    {
        key = keypad_scan();

        switch(key)
        {
            case 1: LED = 0x01; break;
            case 2: LED = 0x02; break;
            case 3: LED = 0x04; break;
            case 4: LED = 0x08; break;
            case 5: running_led(); break;
            case 6: alternate_led(); break;
            case 7: binary_counter(); break;
            case 8: blink_all(); break;
            case 9: LED = 0x00; break;
            case 0: 
                LED = 0x00; 
                display_digit(0); 
                break;
            default: break;
        }
    }
}