#include<stdio.h>
#include<reg51.h>

sbit ledpin=P1^0;
void delay()
{
	int i,j;
	for(i=0;i<500;i++)
	{
		for(j=0;j<500;j++)
		{
		}
	}
}

void main()
{
while(1)
{
ledpin=1;
delay();
ledpin=0;
delay();
}
}