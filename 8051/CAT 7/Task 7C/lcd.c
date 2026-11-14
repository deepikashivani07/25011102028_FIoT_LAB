/* LCD   : RS=P3.2, E=P3.3, D4-D7=P3.4-P3.7 (RW tied to GND)
   Keypad: Rows R1-R4 = P1.0-P1.3 (outputs), Columns C1-C4 = P1.4-P1.7 (inputs)
*/
#include <reg51.h>

#define LCD_PORT P3
#define KEYPAD   P1

sbit RS = P3^2;
sbit E  = P3^3;

/* Key map: [row][column] */
unsigned char code keymap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 1275; j++);
}

void lcd_enable()
{
    E = 1;
    delay_ms(1);
    E = 0;
}

void lcd_send_nibble(unsigned char nibble)
{
    LCD_PORT = (LCD_PORT & 0x0F) | (nibble & 0xF0);
    lcd_enable();
}

void lcd_cmd(unsigned char cmd)
{
    RS = 0;
    lcd_send_nibble(cmd);
    lcd_send_nibble(cmd << 4);
    delay_ms(2);
}

void lcd_data(unsigned char datas)
{
    RS = 1;
    lcd_send_nibble(datas);
    lcd_send_nibble(datas << 4);
    delay_ms(1);
}

void lcd_init()
{
    RS = 0;
    E = 0;
    delay_ms(20);
    lcd_send_nibble(0x20);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    delay_ms(2);
}

void lcd_goto(unsigned char row, unsigned char column)
{
    if(row == 0)
        lcd_cmd(0x80 + column);
    else
        lcd_cmd(0xC0 + column);
}

/* ---------- Keypad ---------- */

/* Returns the key character, or 0 if no key is pressed */
unsigned char keypad_scan()
{
    unsigned char r, c, cols;

    for(r = 0; r < 4; r++)
    {
        KEYPAD = 0xFF & ~(1 << r);        /* drive one row low, columns high (pull-up) */
        cols = (KEYPAD >> 4) & 0x0F;

        if(cols != 0x0F)                  /* some column pulled low */
        {
            delay_ms(20);                 /* debounce */
            cols = (KEYPAD >> 4) & 0x0F;
            for(c = 0; c < 4; c++)
            {
                if(!(cols & (1 << c)))
                    return keymap[r][c];
            }
        }
    }
    return 0;
}

void wait_key_release()
{
    while(keypad_scan() != 0);
    delay_ms(20);
}

void main()
{
    unsigned char row = 0, col = 0, key;

    KEYPAD = 0xFF;
    lcd_init();

    lcd_goto(row, col);
    lcd_data('O');                        /* the moving character */

    while(1)
    {
        key = keypad_scan();

        if(key != 0)
        {
            lcd_goto(row, col);
            lcd_data(' ');                /* erase old position */

            if(key == '2' && row > 0)  row--;      /* Up    */
            if(key == '8' && row < 1)  row++;      /* Down  */
            if(key == '4' && col > 0)  col--;      /* Left  */
            if(key == '6' && col < 15) col++;      /* Right */

            lcd_goto(row, col);
            lcd_data('O');                /* draw at new position */

            wait_key_release();           /* one step per key press */
        }
    }
}