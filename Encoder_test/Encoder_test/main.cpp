/*
 * Encoder_test.cpp
 *
 * Created: 18-11-2016 16:46:23
 * Author : Tanmay
 */ 

//PORTC = LCD
//PORT E5 = INTERRUPT PIN
//PORT E4 = INTERRPT CHECK

#define F_CPU 14745600L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RS 0
#define RW 1
#define EN 2
#define cbit(reg,val) reg &= ~(1<<val)
#define sbit(reg,val) reg |= (1<<val)

volatile int count=0,dir=1;

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
	_delay_ms(1);
	
	temp=cmd;
	temp=temp & 0x0F;
	temp= temp<<4;
	PORTC &= 0x0F;
	PORTC |= temp;
	cbit(PORTC,RS);
	cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	_delay_ms(1);
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
	_delay_ms(1);
	
	temp=a;
	temp=temp & 0x0F;
	temp=temp<<4;
	PORTC &= 0x0F;
	PORTC |= temp;
	sbit(PORTC,RS);
	cbit(PORTC,RW);
	sbit(PORTC,EN);
	_delay_ms(5);
	cbit(PORTC,EN);
	_delay_ms(1);
	
}
void putint(int val)
{	int flag=4;
	command(0x85);
	command(0x04);
	while(flag!=0)
	{
		
		putcha((val%10)+48);
		val=val/10;
		flag--;
	}
	
	command(0x86);
	command(0x06);
}
int main(void)
{	DDRC = 0xF7;
	lcdinit();
	lcdsetup();
	EIMSK = (1<<INT5);
	EICRB = (1<<ISC51);
	sei();
	
	
	
	while(1)
	{
		if(count>=0)
		{
			putint(count);
			dir=1;
		}
		else
		{
			putint(count*(-1));
			dir=0;
			
		}
		
		putcha(' ');
		putcha('d');
		putcha(dir+48);
		//TODO:: Please write your application code */
	}
}
ISR(INT5_vect)
{
	if((PINE&0x10)==0x10)
	count++;
	else count--;
	
}

