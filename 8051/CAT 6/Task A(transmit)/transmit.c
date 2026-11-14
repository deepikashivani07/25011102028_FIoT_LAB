#include <reg52.h>

void uart_init(void)
{
    SCON = 0x50;            /* Mode 1 (8-bit UART), REN = 1      */
    PCON &= 0x7F;           /* SMOD = 0 (no baud doubling)       */
    TMOD &= 0x0F;           /* clear Timer1 bits                 */
    TMOD |= 0x20;           /* Timer1, Mode 2 (8-bit auto-reload)*/
    TH1 = 0xFD;             /* 9600 baud @ 11.0592 MHz           */
    TL1 = 0xFD;
    TR1 = 1;                /* start Timer1                      */
    TI  = 0;
    RI  = 0;
}

void uart_tx(char c)
{
    SBUF = c;               /* load character -> transmission starts */
    while (TI == 0);        /* wait until the stop bit is sent       */
    TI = 0;                 /* clear flag in software                */
}

void uart_puts(char *s)
{
    while (*s)
    {
        uart_tx(*s);
        s++;
    }
}

void main(void)
{
    uart_init();

    while (1)
    {
        uart_puts("Hello from 8051\r\n");
    }
}