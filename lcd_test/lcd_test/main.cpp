/*
 * lcd_test.cpp
 *
 * Created: 18-11-2016 13:18:03
 * Author : Tanmay
 */ 

#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"


int main(void)
{																		//DESCRIPTION:					PORT:

	DDRC = 0xFF;														//LCD							C
	PORTC &= 0x80;														//LCD
	lcd_set_4bit();														//LCD
	lcd_init();															//LCD
	
	
	while(1)
	{
		lcd_string("LCD Test") ; //LCD test
		_delay_ms(1000);
		lcd_cursor(1,1);
		
	}
}


