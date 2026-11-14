#include <reg51.h>

#define LCD_PORT P3

sbit RS = P3^2;
sbit E  = P3^3;


void delay_ms(unsigned int ms)
{
    unsigned int i, j;

    for(i = 0; i < ms; i++)
    {
        for(j = 0; j < 1275; j++);
    }
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

    /* Higher nibble */
    lcd_send_nibble(datas);

    /* Lower nibble */
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



void main()
{
    lcd_init();
while(1){
  lcd_data('H');
	lcd_data('E');
	lcd_data('L');
	lcd_data('L');
	lcd_data('O');
	lcd_data(' ');
	lcd_data('F');
	lcd_data('R');
	lcd_data('O');
	lcd_data('M');
	lcd_data(' ');
	lcd_data('8');
	lcd_data('0');
	lcd_data('5');
	lcd_data('1');
	lcd_data(' ');
	lcd_data(' ');
	lcd_data(' ');
	lcd_data(' ');
	lcd_data(' ');
	lcd_cmd(0x01);
}
}