#include <reg52.h>

#define RX_SIZE 16                      /* power of 2 */
#define RX_MASK (RX_SIZE - 1)

static unsigned char rx_buf[RX_SIZE];
static volatile unsigned char rx_head = 0;   /* written by ISR  */
static volatile unsigned char rx_tail = 0;   /* read by main    */
static volatile bit tx_done = 1;             /* set by ISR      */

/*---------------- UART initialisation ----------------*/
void uart_init(void)
{
    SCON = 0x50;            /* Mode 1, 8-N-1, REN = 1            */
    PCON &= 0x7F;           /* SMOD = 0                          */
    TMOD &= 0x0F;
    TMOD |= 0x20;           /* Timer1 Mode 2 (auto-reload)       */
    TH1 = 0xFD;             /* 9600 baud @ 11.0592 MHz           */
    TL1 = 0xFD;
    TR1 = 1;
    TI = 0;
    RI = 0;
    ES = 1;                 /* enable serial interrupt           */
    EA = 1;                 /* global interrupt enable           */
}

/*---------------- Serial interrupt (vector 4) ---------*/
void uart_isr(void) interrupt 4
{
    unsigned char next;

    if (RI)                                  /* byte received   */
    {
        RI = 0;
        next = (rx_head + 1) & RX_MASK;
        if (next != rx_tail)                 /* buffer not full */
        {
            rx_buf[rx_head] = SBUF;
            rx_head = next;
        }
        else
        {
            next = SBUF;                     /* full: drop byte */
        }
    }

    if (TI)                                  /* byte sent       */
    {
        TI = 0;
        tx_done = 1;
    }
}

/*---------------- Driver functions --------------------*/
void uart_tx(char c)
{
    while (!tx_done);       /* wait for previous byte to finish  */
    tx_done = 0;
    SBUF = c;
}

void uart_puts(char *s)
{
    while (*s) uart_tx(*s++);
}

bit uart_available(void)
{
    return (rx_head != rx_tail);
}

char uart_getc(void)
{
    char c;
    while (rx_head == rx_tail);              /* wait for data   */
    c = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) & RX_MASK;
    return c;
}

/*---------------- Main --------------------------------*/
void main(void)
{
    char c;

    uart_init();
    uart_puts("UART Echo Ready\r\n");

    while (1)
    {
        if (uart_available())
        {
            c = uart_getc();

            if (c == '\r')                   /* Enter key       */
            {
                uart_tx('\r');
                uart_tx('\n');
            }
            else
            {
                uart_tx(c);                  /* echo character  */
            }
        }
    }
}