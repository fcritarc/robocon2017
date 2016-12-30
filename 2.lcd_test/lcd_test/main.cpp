/*
 * lcd_test.cpp
 *
 * Created: 18-11-2016 13:18:03
 * Author : Tanmay
 */ 

//LCD PORT = PORT C 

#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"


int main(void)
{																		

	DDRC = 0xFF;																			
	PORTC &= 0x80;														
	lcd_set_4bit();														
	lcd_init();															
	
	
	while(1)
	{
		lcd_string("LCD Test") ; //LCD test
		_delay_ms(1000);
		lcd_cursor(1,1);
		
	}
}


