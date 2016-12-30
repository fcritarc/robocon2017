/*
 * Blinking.cpp
 *
 * Created: 18-11-2016 13:10:01
 * Author : Tanmay
 */ 
//LED PORT = PORTC
#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{	DDRC = 0xFF;
    /* Replace with your application code */
    while (1) 
    {
		PORTC ^=0xFF;
		_delay_ms(1000);
    }
}

