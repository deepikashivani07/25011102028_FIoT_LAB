#include <reg52.h>

sbit LED = P1^7;                 /* Active-HIGH LED. If your LED is
                                    active-LOW, see ISR comment.     */

#define PWM_STEPS     20         /* 20 ticks per PWM period          */
#define FADE_PERIODS  50         /* PWM periods between duty changes */

volatile unsigned char duty = 0; /* 0..PWM_STEPS                     */
unsigned char tick = 0;
unsigned char period_cnt = 0;
bit fade_flag = 0;
bit dir_up = 1;

void Timer0_ISR(void) interrupt 1     /* every ~50 us (auto-reload) */
{
    tick++;
    if (tick >= PWM_STEPS)
    {
        tick = 0;
        period_cnt++;
        if (period_cnt >= FADE_PERIODS)
        {
            period_cnt = 0;
            fade_flag = 1;            /* tell main to change duty   */
        }
    }

    LED = (tick < duty);              /* active-HIGH.                */
    /* active-LOW board: LED = !(tick < duty);                       */
}

void main(void)
{
    LED = 0;

    TMOD = (TMOD & 0xF0) | 0x02;      /* Timer0, Mode 2            */
    TH0  = 0xD2;                      /* 256 - 46 = 210 = 0xD2     */
    TL0  = 0xD2;

    ET0 = 1;
    EA  = 1;
    TR0 = 1;

    while (1)
    {
        if (fade_flag)
        {
            fade_flag = 0;

            if (dir_up)
            {
                if (duty < PWM_STEPS) duty++;
                if (duty >= PWM_STEPS) dir_up = 0;
            }
            else
            {
                if (duty > 0) duty--;
                if (duty == 0) dir_up = 1;
            }
        }
    }
}