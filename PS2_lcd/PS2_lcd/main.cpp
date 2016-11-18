/*
 * PS2_lcd.cpp
 *
 * Created: 18-11-2016 13:31:52
 * Author : Tanmay
 */ 

#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "psx.h"

volatile uint8_t lx;
volatile uint8_t ly;
volatile uint8_t rx;
volatile uint8_t ry;
int cross_button , square_button , circle_button , triangle_button , r1_button , l1_button , l2_button;
int pad_up, pad_down, pad_left, pad_right;
void values_input()
{
	psx_read_gamepad();
	lx = psx_stick(PSS_LX);
	ly = psx_stick(PSS_LY);
	ry = psx_stick(PSS_RY);
	rx = psx_stick(PSS_RX);
	cross_button = psx_button(PSB_CROSS);
	square_button = psx_button(PSB_SQUARE);
	circle_button = psx_button(PSB_CIRCLE);
	triangle_button = psx_button(PSB_TRIANGLE);
	pad_up = psx_button(PSB_PAD_UP);
	pad_down = psx_button(PSB_PAD_DOWN);
	pad_left = psx_button(PSB_PAD_LEFT);
	pad_right = psx_button(PSB_PAD_RIGHT);
	r1_button = psx_button(PSB_R1);
	l1_button = psx_button(PSB_L1);
	l2_button = psx_button(PSB_L2);
}

int main(void)
{																		//DESCRIPTION:					PORT:
	psx_init(&PORTK , 1 , &PORTK , 4 , &PORTK , 0 , &PORTK , 2);		//PS2 Initialization			K
	DDRC = 0xFF;														//LCD							C
	PORTC &= 0x80;														//LCD 
	lcd_set_4bit();														//LCD 
	lcd_init();															//LCD   
	while(1)
    {
	values_input();
	lcd_cursor(1,2);
	lcd_string("RX");
	lcd_cursor(1,6);
	lcd_string("RY");
	lcd_cursor(1,10);
	lcd_string("LX");
	lcd_cursor(1,14);
	lcd_string("LY");
	lcd_print(2,1,rx,3);
	lcd_print(2,5,ry,3);
	lcd_print(2,9,lx,3);
	lcd_print(2,13,ly,3);
	_delay_ms(100);
    }
}


