/*LCD: RS=P3.2, E=P3.3, D4-D7=P3.4-P3.7 (RW tied to GND) */
#include <reg51.h>

#define LCD_PORT P3

sbit RS = P3^2;
sbit E  = P3^3;

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
    lcd_send_nibble(0x20);   /* switch to 4-bit mode */
    lcd_cmd(0x28);           /* 4-bit, 2-line, 5x8 font */
    lcd_cmd(0x0C);           /* display ON, cursor OFF */
    lcd_cmd(0x06);           /* entry mode: cursor moves right */
    lcd_cmd(0x01);           /* clear display */
    delay_ms(2);
}

/* ---------- Task 7B functions ---------- */

/* row: 0 or 1, column: 0 to 15
   DDRAM: row 0 starts at 0x00, row 1 starts at 0x40; command = 0x80 | address */
void lcd_goto(unsigned char row, unsigned char column)
{
    if(row == 0)
        lcd_cmd(0x80 + column);
    else
        lcd_cmd(0xC0 + column);
}

void lcd_puts(char *str)
{
    while(*str)
        lcd_data(*str++);
}

void lcd_print_number(unsigned int value)
{
    char buf[5];             /* max 65535 = 5 digits */
    unsigned char i = 0;

    if(value == 0)
    {
        lcd_data('0');
        return;
    }
    while(value > 0)
    {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }
    while(i > 0)
        lcd_data(buf[--i]);
}

void main()
{
    unsigned int count = 0;

    lcd_init();

    lcd_goto(0, 0);
    lcd_puts("Hello 8051");
    lcd_goto(1, 0);
    lcd_puts("Count: ");

    while(1)
    {
        lcd_goto(1, 7);              /* position after "Count: " */
        lcd_print_number(count);
        lcd_puts("     ");            /* blank leftover digits when number shrinks */

        count++;
        delay_ms(500);
    }
}