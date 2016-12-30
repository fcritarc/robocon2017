/*
 * prototype_base_1.cpp
 *
 * Created: 29-12-2016 11:27:40
 * Author : Tanmay
 */ 

#define F_CPU 14745600L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "psx.h"
#define PWM_MIN 0

volatile int count=0,dir=1,dir_l=1;
volatile uint8_t lx;
volatile uint8_t ly;
volatile uint8_t rx;
volatile uint8_t ry;
int cross_button , square_button , circle_button , triangle_button , r1_button , l1_button , l2_button;
int pad_up, pad_down, pad_left, pad_right;
int flag=0;

	int y;
	
int ramp(int count)
{	int v;
	if(count<=2275) return 50;
	
	else 
	{
		v=(count-25)/90;
		v=v-25;
		return (50-v);
	}
	
}
void backward()
{
	//PORTA=0x00;
	PORTA=(1<<PA6) | (1<<PA5) | (1<<PA3) | (1<<PA0);
	
	/*OCR1CL = 30;	 //wheel 3
	OCR1AL =30;	 //wheel 2
	OCR1BL = 30;	 //wheel 4
	OCR2A = 30;  */    //wheel 1
}

void forward()
{
	//PORTA=0x00;
	PORTA=(1<<PA4) | (1<<PA2) | (1<<PA1) | (1<<PA7);
	/*OCR1CL = 30;	 //wheel A
	OCR1AL = 30;	 //wheel D
	OCR1BL = 30;	 //wheel B
	OCR2A =	30;      //wheel C*/
	
}

void stop()
{	PORTA=0xFF;
	/*OCR1CL = 0;	 //wheel 3
	OCR1AL = 0;	 //wheel 2
	OCR1BL = 0;	 //wheel 4
	OCR2A = 0;*/
	//PORTA=0x00;     //wheel 1
}

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

void run_controller()
{
	values_input();
	
	if(l1_button)
	{
		flag=1;
		forward();
	
		
	}
	if(l2_button)
	{
		flag=1;
		backward();
		
	}
	if(cross_button)
	{
		stop();
		flag=0;
	}

}


void line()
{
	//PORTA=(1<<PA4) | (1<<PA2) | (1<<PA1) | (1<<PA7);
	if(y>60)
	{
		OCR1CL = 30;	 //wheel A
		OCR1AL = 20;	 //wheel D
		OCR1BL = 20;	 //wheel B
		OCR2A =	30;      //wheel C
		
		
	}
	else if(y<40)
	{
		OCR1CL = 20;	 //wheel A
		OCR1AL = 30;	 //wheel D
		OCR1BL = 30;	 //wheel B
		OCR2A =	20;      //wheel C
		
		
	}
	else
	{
		OCR1CL = 30;	 //wheel A
		OCR1AL = 30;	 //wheel D
		OCR1BL = 30;	 //wheel B
		OCR2A =	30;      //wheel C
		
		
	}
	
}
void p_line()
{
	int error,kp=2;
	y=ADCL;
	y=(ADCH<<2) | y;
	y = ((float(y)/921)*70);
	lcd_print(2,1,y,3);
	error=(y-50)*kp;
	if(dir==0)
	{	
		error=error*(-1)*kp;
	}
	else;
	if((error>=(50)) || (error<=(-50)))
	{
		stop();
	}
	else
	{
		
		if(error>0)
		{
			OCR1BL=30-error;  //wheel B
			OCR2A=30;	 //wheel C
			OCR1CL = 30;	 //wheel A
			OCR1AL = 30-error;	 //wheel D
		}
		else if(error<0)
		{
			OCR1BL=30; //wheel B
			OCR2A=30+error;	 //wheel C
			OCR1CL = 30+error;	 //wheel A
			OCR1AL = 30;	 //wheel D
			
		}
		else
		{
			OCR1CL = 30;	 //wheel A
			OCR1AL = 30;	 //wheel D
			OCR1BL = 30;	 //wheel B
			OCR2A =	30;      //wheel C
			
			
		}
	}
}
int main(void)
{
	psx_init(&PORTK , 1 , &PORTK , 4 , &PORTK , 0 , &PORTK , 2);		
	
	EIMSK = (1<<INT5);
	EICRB = (1<<ISC51);
	sei();
	
	int dir_l;
	
	DDRF = 0x00;
	
	ADMUX = (1<<REFS0) | (1<<MUX0) | (1<<MUX1) | (1<<ADLAR);
	ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADATE) | (1<<ADSC);
	
	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) |(1 << COM1C1);
	TCCR2A |= (1 << WGM20) | (1 << COM2A1) | (1 << WGM21);
	
	
	TCCR1B |= (1 << WGM12) |(1 << CS10) | (1<<CS11); // set no pre-scaling
	TCCR2B |= (1<<CS22);
	DDRA=0xFF;                                                          
	DDRB = 0xFF;                                                         
	DDRC = 0xFF;														
	PORTC &= 0x80;														
	lcd_set_4bit();														
	lcd_init();						
	stop();
		
while(1)
{	
	run_controller();
	y=ADCL;
	y=(ADCH<<2) | y;
	y = ((float(y)/921)*70);
	lcd_print(2,1,y,3);	
	if(flag==1)
	{	
		run_controller();
		if((count>-4500) && (count<4500))
		{
			line();
		}
		else 
		{
			stop();
			flag=0;
			lcd_cursor(1,14);
			lcd_string("e");
			while(flag==0)
			{
				run_controller();
			}
			_delay_ms(200);
			lcd_cursor(1,14);
			lcd_string(" ");
			count=0;
		}
		if((PINF&0x10)==0x10)
		{	
			lcd_cursor(2,13);
			lcd_string("j");
			stop();
			flag=0;
			while(flag==0)
			{
				run_controller();
			}
			_delay_ms(200);
			lcd_cursor(2,13);
			lcd_string(" ");
			
		}
		
		if(count>=0)
		{
			lcd_print(1,1,count,5);
		}
		else
		{
			lcd_print(1,1,(count*(-1)),5);//modified
		}
		if((dir_l)!=dir)
		{
			count=0;
			dir_l=dir;
		}
		
	}
}
}
ISR(INT5_vect)
{
	if((PINE&0x10)==0x10)
	{
		count++;
		dir=1;
	}
	else 
	{
		count--;
		dir=0;
	}
}