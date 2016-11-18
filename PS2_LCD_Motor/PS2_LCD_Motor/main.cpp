/*
 * PS2_LCD_Motor.cpp
 *
 * Created: 18-11-2016 14:21:21
 * Author : Tanmay
 */ 
//Pad up      = PWM increase
//Pad down    = PWM decrease
//Circle      = Rotate anticlockwise
//Square      = Rotate clockwise


#define F_CPU 14745600L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "psx.h"
#define PWM_MIN 0


volatile uint8_t lx;
volatile uint8_t ly;
volatile uint8_t rx;
volatile uint8_t ry;
int cross_button , square_button , circle_button , triangle_button , r1_button , l1_button , l2_button,pwm_val=123;
int pad_up, pad_down, pad_left, pad_right;

void forward();
void backward();
void right_slide();
void left_slide();
void rotate_clockwise();
void rotate_anticlockwise();
void stop();



void set_pwm(uint8_t pwm) {
	OCR1CL = pwm;	// Set pwm for oc0a //wheel 3
	OCR1AL = pwm;	// Set pwm for oc1a //wheel 2
	OCR1BL = pwm;	// Set pwm for 0c1b //wheel 4
	OCR2A = pwm; //wheel 1
}
void pwm_setup()
{
	//TCCR0A |= (1 << WGM00) | (1 << COM0A1) | (1 << WGM01); // Set output compare register for oc0a and oc0b with COM0a and COM0b
	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) |(1 << COM1C1); // Set output compare register for oc1a and oc1b with COM1a and COM1b
	TCCR2A |= (1 << WGM20) | (1 << COM2A1) | (1 << WGM21);
	
	//TCCR0B |= (1 << CS00); // Set no pre-scaling
	TCCR1B |= (1 << WGM12) |(1 << CS10); // set no pre-scaling
	TCCR2B |= (1 << CS20);
	DDRB = 0xff;
}
void values_input()
{
	psx_read_gamepad();
	lx = psx_stick(PSS_LX);
	ly = psx_stick(PSS_LY);
	ry = psx_stick(PSS_RY);
	rx = psx_stick(PSS_RX);
	//lcd_print(2 , 13 , ry , 3);
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
void ramping(uint8_t pwm)
{
	
	for(i=0;i<=pwm;i++)
	{
		set_pwm(i);
		_delay_ms(5);
	}
}
void run_controller()
{
	values_input();
	if((cross_button)||(120<lx && lx<125 && 120<ly && ly<125))
	{
		stop();
	}
	else
	{
		if(ly<120 && 120<lx && lx<125)
		{
			lcd_cursor(1,1);
			lcd_string("Forward    ");
			forward();					//FWD
			PORTJ=0x01;
		}
		if(ly>125 && 120<lx && lx<125)
		{
			lcd_cursor(1,1);
			lcd_string("Backward    ");
			backward();					//BCK
			PORTJ=0x02;
		}
		if(lx>125 && 119<ly && ly<127)
		{
			lcd_cursor(1,1);
			lcd_string("Right Slide  ");
			right_slide();						//R
			PORTJ=0x04;
		}
		if(lx<120 && 119<ly && ly<127)
		{
			lcd_cursor(1,1);
			lcd_string("Left Slide  ");
			left_slide();						//L
			PORTJ=0x08;
		}
	}
	if (pad_up)									
	{
		pwm_val+=10;
		if (pwm_val>255)
		{
			pwm_val=255;
		}
	}
	if (pad_down)
	{
		pwm_val-=10;
		if (pwm_val<0)
		{
			pwm_val=0;
		}
	}
	
	if(square_button)
	{
		_delay_ms(10);
		rotate_clockwise();
	}
	
	if(circle_button)
	{
		_delay_ms(10);
		rotate_anticlockwise();
	}
	

}
int main(void)
{																		//DESCRIPTION:					PORT:
	psx_init(&PORTK , 1 , &PORTK , 4 , &PORTK , 0 , &PORTK , 2);		//PS2 Initialization			K
	DDRB=0xFF;															//PWM Initialization			B
	pwm_setup();
	DDRA=0xFF;															//Directional Initialization	A
	PORTA=0x00;															//Directional Initialization
	DDRC = 0xF7;														//LCD							C
	PORTC &= 0x80;														//LCD
	lcd_set_4bit();														//LCD
	lcd_init();															//LCD
	
	
	
	while(1)
	{
		run_controller();
		lcd_print(2,1,pwm_val,4);
		_delay_ms(10);
	}
}

void forward()
{
	PORTA=0x00;
	PORTA=0x66;
	
	_delay_ms(10);
	set_pwm(pwm_val);
	
}

void backward()
{
	PORTA=0x00;
	PORTA=0x99;
	
	_delay_ms(10);
	set_pwm(pwm_val);
	
}

void right_slide()
{
	PORTA=0x00;
	PORTA=0xC3;
	
	_delay_ms(10);
	set_pwm(pwm_val);
	
}

void left_slide()
{
	PORTA=0x00;
	PORTA=0x3C;
	
	_delay_ms(10);
	set_pwm(pwm_val);
}

void stop()
{
	PORTA=0xFF;
	set_pwm(0);
}

void rotate_clockwise()
{
	PORTA=0x00;
	PORTA=0xCC;
	OCR1CL = 30;	 //wheel 3
	OCR1AL = 30;	 //wheel 2
	OCR1BL = 30;	 //wheel 4
	OCR2A = 30;      //wheel 1
	_delay_ms(10);

}

void rotate_anticlockwise()
{
	PORTA=0x00;
	PORTA=0x33;
	_delay_ms(10);
	OCR1CL = 30;	//wheel 3
	OCR1AL = 30;	//wheel 2
	OCR1BL = 30;	//wheel 4
	OCR2A = 30;    //wheel 1
	_delay_ms(10);
}

