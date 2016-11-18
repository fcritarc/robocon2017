#define F_CPU 14745600L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcd.h"

void forward();
void backward();
void right_slide();
void left_slide();
void rotate_clockwise();
void rotate_anticlockwise();
void right();
void left();
void stop();
int main(void)
{
	
	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) |(1 << COM1C1);
	TCCR2A |= (1 << WGM20) | (1 << COM2A1) | (1 << WGM21);
	
	
	TCCR1B |= (1 << WGM12) |(1 << CS10); // set no pre-scaling
	TCCR2B |= (1 << CS20);
	DDRA=0xFF;                                                           // direction    port a
	DDRB = 0xFF;                                                         //pwm           port b
	DDRC = 0xFF;														//LCD            port c
	PORTC &= 0x80;														//LCD
	lcd_set_4bit();														//LCD
	lcd_init();															//LCD
	stop();
	_delay_ms(4000);
	while(1)
	{
		lcd_string("FWD"); //Forward
		lcd_cursor(1,1);
		forward();
		_delay_ms(5000);
		stop();
		PORTC |= (1<<PC3);
		_delay_ms(2000);
		PORTC &= ~(1<<PC3);
		
		lcd_string("BWD");  //Backward
		lcd_cursor(1,1);
		backward();
		_delay_ms(5000);
		stop();
		PORTC |= (1<<PC3);
		_delay_ms(2000);
		PORTC &= ~(1<<PC3);
		
		
		lcd_string("RSL");  //Right slide
		lcd_cursor(1,1);
		right_slide();
		_delay_ms(5000);
		stop();
		PORTC |= (1<<PC3);
		_delay_ms(2000);
		PORTC &= ~(1<<PC3);
		
		
		lcd_string("LSL");  //Left slide
		lcd_cursor(1,1);
		left_slide();
		_delay_ms(5000);
		stop();
		PORTC |= (1<<PC3);
		_delay_ms(2000);
		PORTC &= ~(1<<PC3);
		
	}
}

void forward()
{
	PORTA=0x00;
	PORTA=0x66;
	_delay_ms(10);
	for(int i=30;i<=50;i+=5)
	{OCR1CL = i;	 //wheel 3
		OCR1AL =i;	 //wheel 2
		OCR1BL = i;	 //wheel 4
		OCR2A = i;      //wheel 1
		_delay_ms(10);
	}
}

void backward()
{
	PORTA=0x00;
	PORTA=0x99;
	_delay_ms(10);
	for(int i=30;i<=50;i+=5)
	{
		OCR1CL = i;	 //wheel 3
		OCR1AL = i;	 //wheel 2
		OCR1BL = i;	 //wheel 4
		OCR2A = i;      //wheel 1
		_delay_ms(10);
	}
	
}


void right_slide()
{
	PORTA=0x00;
	PORTA=0xC3;
	_delay_ms(10);
	for(int i=30;i<=50;i+=5)
	{OCR1CL = i;	 //wheel 3
		OCR1AL = i;	 //wheel 2
		OCR1BL = i;	 //wheel 4
		OCR2A = i;      //wheel 1
		_delay_ms(10);
	}
}

void left_slide()
{
	PORTA=0x00;
	PORTA=0x3C;
	_delay_ms(10);
	for(int i=30;i<=50;i+=5)
	{OCR1CL = i;	 //wheel 3
		OCR1AL = i;	 //wheel 2
		OCR1BL = i;	 //wheel 4
		OCR2A = i;      //wheel 1
		_delay_ms(10);
	}
}


void stop()
{PORTA=0x00;
	OCR1CL = 0;	 //wheel 3
	OCR1AL = 0;	 //wheel 2
	OCR1BL = 0;	 //wheel 4
	OCR2A = 0;      //wheel 1
}






