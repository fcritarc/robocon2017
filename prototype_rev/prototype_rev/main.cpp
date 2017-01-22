/*
 * prototype_rev.cpp
 *
 * Created: 12-01-2017 20:34:37
 * Author : Tanmay
 */ 

#define BAUD 9600
#define F_CPU 14745600L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "psx.h"
#include <util/setbaud.h>

#define BAUD_PRESCALLER (F_CPU/16/BAUD-1)
#define Adirf PA2
#define Adirb PA0
#define Bdirf PA3
#define Bdirb PA1
#define Cdirf PA5
#define Cdirb PA7
#define Ddirf PA4
#define Ddirb PA6
#define Apwm OCR1CL
#define Dpwm OCR1AL
#define Cpwm OCR1BL
#define Bpwm OCR2A

volatile int count=0;
volatile uint8_t lx;
volatile uint8_t ly;
volatile uint8_t rx;
volatile uint8_t ry;
int cross_button , square_button , circle_button , triangle_button , r1_button , l1_button , l2_button,r2_button;
int pad_up, pad_down, pad_left, pad_right;
int runflag=0,j_flag=0,y=0,dpwm=30;
float error=0,p_error=0;
float kp=1,kd=0;//0.15
	
void USART_init(void)
{
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0) | (1 << TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
}

void USART_send(char data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void USART_integer(int a)
{
	int rev=0,count=0;
	while(a!=0)
	{
		rev=(rev*10)+(a%10);
		a=a/10;
	}
	while(count!=2)
	{
		USART_send((rev%10)+48);
		rev=rev/10;
		count++;
	}
}
void USART_display()
{
	/*USART_integer(kp*100);
	USART_send('\t');
	USART_integer(kd*100);
	USART_send('\t');
	USART_integer(ki*100);
	USART_send('\t');*/
	USART_integer(y);
	USART_send('\n');
	//USART_integer(error);
	//USART_send('\n');
}
void backward()
{
	PORTA=(1<<Ddirb) | (1<<Cdirb) | (1<<Bdirb) | (1<<Adirb);
	Apwm = 40;	 //modified
	Bpwm = 40;
	Cpwm = 40;
	Dpwm = 40;
	_delay_ms(50);
}

void forward()
{
	PORTA=(1<<Ddirf) | (1<<Cdirf) | (1<<Bdirf) | (1<<Adirf);
	Apwm = 40;	 //modified
	Bpwm = 40;
	Cpwm = 40;
	Dpwm = 40;
	_delay_ms(50);
}

void stop()
{	
	PORTA=0x00;
	_delay_ms(50);
}
/*void left_slide_O()
{
	PORTA=(1<<Adirf) | (1<<Cdirb) | (1<<Bdirb) | (1<<Ddirf);
}
void right_slide_O()
{
	PORTA=(1<<Adirb) | (1<<Cdirf) | (1<<Bdirf) | (1<<Ddirb);
}*/
void left_slide_X()
{
		PORTA=(1<<Adirf) | (1<<Cdirf) | (1<<Bdirb) | (1<<Ddirb);
		Apwm = 50;	 //modified
		Bpwm = 50;
		Cpwm = 50;
		Dpwm = 50;
}
void right_slide_X()
{
	PORTA=(1<<Adirb) | (1<<Cdirb) | (1<<Bdirf) | (1<<Ddirf);
	Apwm = 50;	 //modified
	Bpwm = 50;
	Cpwm = 50;
	Dpwm = 50;
}
/*void rotate_clockwise_O()
{
	PORTA=(1<<Adirf) | (1<<Cdirf) | (1<<Bdirb) | (1<<Ddirb);
	Apwm = 30;	 //comment B and C for slow turn
	Bpwm = 30;	 
	Cpwm = 30;	 
	Dpwm = 30;      
	_delay_ms(1);

}

void rotate_anticlockwise_O()
{
	PORTA=(1<<Bdirf) | (1<<Ddirf) | (1<<Cdirb) | (1<<Adirb);
	Apwm = 30;	 //comment A and D for slow turn
	Bpwm = 30;	
	Cpwm = 30;	
	Dpwm = 30;    
	_delay_ms(1);
}*/
void rotate_anticlockwise_X()
{
	PORTA=(1<<Adirb) | (1<<Cdirf) | (1<<Bdirb) | (1<<Ddirf);
	Apwm = 40;	 //comment B and C for slow turn
	Bpwm = 40;
	Cpwm = 40;
	Dpwm = 40;
	_delay_ms(1);

}
void rotate_clockwise_X()
{
	PORTA=(1<<Bdirf) | (1<<Ddirb) | (1<<Cdirb) | (1<<Adirf);
	Apwm = 40;	 //comment A and D for slow turn
	Bpwm = 40;
	Cpwm = 40;
	Dpwm = 40;
	_delay_ms(1);
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
	r2_button = psx_button(PSB_R2);
	l1_button = psx_button(PSB_L1);
	l2_button = psx_button(PSB_L2);
}

void adjust()
{
	if(y>40)
	{
		rotate_clockwise_X();
	}
	else if(y<30)
	{
		rotate_anticlockwise_X();
	}
	else
	{
		stop();
		_delay_ms(10000);
		j_flag=0;	
	}
}

void run_controller()
{
	values_input();
	
	if(l1_button)
	{
		forward();	 
	//rotate_clockwise();
	}
	
	if(triangle_button)     //run PID
	{
		runflag=1;
		forward();
	}
	
	if(l2_button)
	{
		backward(); 
		//rotate_anticlockwise();
	}
	if(cross_button)
	{
		stop();
		runflag=0;
	}
	
	if(r2_button)
	{
		left_slide_X();
	}
	if(r1_button)
	{
		right_slide_X();
	}
	if(square_button)
	{
		rotate_anticlockwise_X();
	}
	if(circle_button)
	{
		rotate_clockwise_X();
	}
	
}
void error_calc()
{
	y=ADCL;
	y=(ADCH<<8) | y;          //Direct LSA value
	y=((float(y)/1020)*70);   //normalized value

	if(y>=68)
	{
		error=p_error;
		lcd_cursor(1,13);
		lcd_string("i");
	}
	else
	{
		error=(y-35);
		error=error*kp +(error-p_error)*kd;  
		p_error=error;
		lcd_cursor(1,13);
		lcd_string("o");
	}
	
	if(error<0)
	{	lcd_cursor(2,1);
		lcd_string("-");
		lcd_print(2,2,(error*(-1)),3);
	}
	
	else
	{
		lcd_cursor(2,1);
		lcd_string("+");
		lcd_print(2,2,error,3);
	}
	
	lcd_print(2,9,y,4);
}

void p_line()
{
	error_calc();
	if((error>=(40)) || (error<=(-40)))
	{	
		lcd_cursor(1,13);
		lcd_string("h");
		stop();
	}
	else
	{
		if((error<0)||(error>0))
			{
			Cpwm=40-error;  
			Bpwm=40+error;	 
			Apwm= 40+error;	 
			 Dpwm= 40-error;	 
			}
		
		else
		{
			OCR1CL = 40;	 //wheel A
			OCR1AL = 40;	 //wheel D
			OCR1BL = 40;	 //wheel B
			OCR2A =	40;      //wheel C
			
			
		}
	}
}

void junc_detect()   //Detect Junction
{
	if(((PINF&0x10)==0x10))
		{	
			stop();
			count=0;
			PORTC|=(1<<PC3);
			_delay_ms(2000);
			PORTC&=~(1<<PC3);
			lcd_cursor(1,13);
			lcd_string("junc");
			j_flag=1;
			runflag=0;
			_delay_ms(2000);
			lcd_cursor(1,13);
			lcd_string("    ");
			
		}
		
}

int main(void)
{
	psx_init(&PORTK , 1 , &PORTK , 4 , &PORTK , 0 , &PORTK , 2);		
	
	USART_init();
	EIMSK = (1<<INT5);   //Encoder initialization
	EICRB = (1<<ISC51);
	sei();
	
	DDRF = 0x00;
	
	ADMUX = (1<<REFS0) | (1<<MUX0) | (1<<MUX1) ;  //ADC initialization
	ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADATE) | (1<<ADSC);
	
	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) |(1 << COM1C1);  //wheel initialization
	TCCR2A |= (1 << WGM20) | (1 << COM2A1) | (1 << WGM21);
	
	
	TCCR1B |= (1 << WGM12) |(1 << CS10) | (1<<CS11); // precaling = t/64
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
	error_calc();
	USART_display();
	/*if(j_flag==1)
	{
		adjust();
		lcd_cursor(1,13);
		lcd_string("adj");
	}*/
	
	while((runflag==1))
	{	
		run_controller();
		//forward();
		//rotate_clockwise_O();
		p_line();
		USART_display();
	}
	//error_calc();
	//adjust();
}
}
ISR(INT5_vect)
{
	if((PINE&0x10)==0x10)
	{
		count++;
	}
	else 
	{
		count--;
	}
}

