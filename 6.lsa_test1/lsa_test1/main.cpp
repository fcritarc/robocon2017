/*
 * lsa_test1.cpp
 *
 * Created: 16-11-2016 15:58:41
 * Author : Tanmay
 */ 
#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>

#define RS 0
#define RW 1
#define EN 2
#define cbit(reg,val) reg &= ~(1<<val)
#define sbit(reg,val) reg |= (1<<val)


void lcdinit()
{
	int i;
	PORTC &= 0x80 ;
	cbit(PORTC,RS);
	cbit(PORTC,RW);
	PORTC = 0x30;
	for(i=0;i<=2;i++)
	{
		
		
		sbit(PORTC,EN);
		_delay_ms(5);
		cbit(PORTC,EN);
		_delay_ms(1);
	}
	
	PORTC = 0x20;
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	_delay_ms(1);
}


void command(unsigned char cmd)
{
	unsigned char temp;
	temp=cmd;
	temp=temp & 0xF0;
	PORTC &= 0x0F;
	PORTC |= temp;
	cbit(PORTC,RS);
	cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	
	temp=cmd;
	temp=temp & 0x0F;
	temp= temp<<4;
	PORTC &= 0x0F;
	PORTC |= temp;
	//cbit(PORTC,RS);
	//cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
}

void lcdsetup()
{
	//command(0x28);	//4 bit 2 row
	command(0x01);	//clr scr
	command(0x06);	//cursor right shift
	command(0x0E);	//cursor blink display on
	command(0x80);	//force cursor to 1 row 0 col
	
}

void putcha(char a)
{
	unsigned char temp;
	temp=a;
	temp=temp & 0xF0;
	PORTC &= 0x0F;
	PORTC |= temp;
	sbit(PORTC,RS);
	cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	
	temp=a;
	temp=temp & 0x0F;
	temp=temp<<4;
	PORTC &= 0x0F;
	PORTC |= temp;
	//sbit(PORTC,RS);
	//cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	
}
void putint(int x)
{	int count=0;
	command(0x85);
	command(0x04);
	while(count!=4)
	{
		putcha((x%10)+48);
		x=x/10;
		count++;
		
	}
	command(0x80);
	command(0x06);
	
}
int main(void)
{	DDRC = 0xFF;
	DDRF = 0x00;
	lcdinit();
	lcdsetup();
	ADMUX = (1<<REFS0) | (1<<MUX0) | (1<<MUX1) | (1<<ADLAR);
	ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADATE) | (1<<ADSC);
	int y;
    /* Replace with your application code */
    while (1) 
    {
		y=ADCL;
		y=(ADCH<<2) | y;
		y = ((float(y)/921)*70);
		putint(y);
		_delay_ms(1000);
		if((PINF&0x10)==0x10)
		{
			command(0x86);
			putcha('j');
			
		}
		else
		{
			command(0x86);
			putcha(' ');
		}
		
    }
}

