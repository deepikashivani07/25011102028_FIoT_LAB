#include <reg52.h>

sbit SIG   = P1^0;
sbit KEY_F = P3^4;
sbit KEY_S = P3^5;

unsigned char code half_ticks[5] = {100, 20, 10, 5, 2};

unsigned char code pat_square[] = {1,0,1,0,1,0,1,0};
unsigned char code pat_vduty[]  = {1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0};
unsigned char code pat_pulse[]  = {1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0};
unsigned char code pat_burst[]  = {1,0,1,0,1,0,1,0,1,0, 0,0,0,0,0,0,0,0,0,0, 1,0,1,0,1,0,1,0,1,0};

unsigned char code *pat;
unsigned char pat_len = 8;
unsigned char pat_idx = 0;
unsigned char bit_ticks = 20;
unsigned char cnt = 0;
unsigned int  scan_cnt = 0;
bit scan_flag = 0;

void Timer0_ISR(void) interrupt 1       /* every ~50 us */
{
    cnt++;
    if (cnt >= bit_ticks)
    {
        cnt = 0;
        SIG = pat[pat_idx];
        pat_idx++;
        if (pat_idx >= pat_len) pat_idx = 0;
    }

    scan_cnt++;
    if (scan_cnt >= 400)                /* 400 x 50 us = 20 ms */
    {
        scan_cnt = 0;
        scan_flag = 1;                  /* time to scan buttons */
    }
}

void load_pattern(unsigned char t)
{
    EA = 0;
    switch (t)
    {
        case 0:  pat = pat_square; pat_len = sizeof(pat_square); break;
        case 1:  pat = pat_vduty;  pat_len = sizeof(pat_vduty);  break;
        case 2:  pat = pat_pulse;  pat_len = sizeof(pat_pulse);  break;
        default: pat = pat_burst;  pat_len = sizeof(pat_burst);  break;
    }
    pat_idx = 0;
    cnt = 0;
    EA = 1;
}

void main(void)
{
    unsigned char f_mode = 0, s_mode = 0;
    unsigned char f_prev = 1, s_prev = 1;
    unsigned char f_now, s_now;

    SIG = 0;
    KEY_F = 1;  KEY_S = 1;              /* input mode */

    bit_ticks = half_ticks[f_mode];
    load_pattern(s_mode);

    TMOD = (TMOD & 0xF0) | 0x02;        /* Timer0 Mode 2 */
    TH0 = 0xD2;
    TL0 = 0xD2;
    ET0 = 1;
    EA  = 1;
    TR0 = 1;

    while (1)
    {
        if (scan_flag)
        {
            scan_flag = 0;
            f_now = KEY_F;
            s_now = KEY_S;

            if (f_prev == 1 && f_now == 0)       /* falling edge */
            {
                f_mode++;
                if (f_mode >= 5) f_mode = 0;
                EA = 0;
                bit_ticks = half_ticks[f_mode];
                cnt = 0;
                EA = 1;
            }
            if (s_prev == 1 && s_now == 0)
            {
                s_mode++;
                if (s_mode >= 4) s_mode = 0;
                load_pattern(s_mode);
            }
            f_prev = f_now;
            s_prev = s_now;
        }
    }
}