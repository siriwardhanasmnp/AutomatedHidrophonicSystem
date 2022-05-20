/*
 * Project_Complete.c
 *
 * Created: 4/19/2021 11:10:16 PM
 * Author : Navodya
 */ 

#define F_CPU 8000000UL
//Include Files
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "avr/sfr_defs.h"
#include "avr/interrupt.h"
#include <avr/pgmspace.h>

//Ports and Pin defining
#define DHT11_PIN 2
#define LCD_DPRT PORTC
#define LCD_DDDR DDRC
#define LCD_RS 0
#define LCD_EN 1


void mode_select();
void pH_Motors();

//DHT_11 functions
void DHT_11();
void DHT_11_Display();
void Request();
void Response();
uint8_t Receive_data();

//LCD_functions
void lcdcommand(unsigned char cmnd);
void lcddata(unsigned char data);
void lcdinit();
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_print(char *str);
void lcd_clear();
void convert_Mseconds(void);

//LDR_functions
void LDR();
void InitADC();
uint16_t ReadADC(uint8_t ch);

//Variable Declaration
int microSec=0,m_sec=0,seconds=0,unit=0,microsecs=0,m_seconds=0;
float Standard_pH_LOW = 0.0, Standard_pH_HIGH = 0.0;
int pH_logic = 0, Mode=0, mode_logic=0;
int hum =0, temp=0;

char data[5];
char data_pH[5];
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
uint16_t adc_result;




int main(void)
{

	sei();
	
	DDRA=0x00;
	DDRB=0Xff;
	DDRC=0Xff;
	DDRD=0Xff;
	

	DDRD = DDRD | (1<<0);

	lcdinit();					/* initialize LCD */
	lcd_clear();				/* clear LCD */
	lcd_gotoxy(3,0);			/* enter column and row position */
	lcd_print("..WELCOME..");
	_delay_ms(2000);
	m_seconds+=1000;
	convert_Mseconds();
	lcd_clear();
	lcd_gotoxy(3,0);
	for(int a=0; a<6 ; a++){
		lcd_print("SELECT MODE");
		_delay_ms(200);
		lcd_clear();
		lcd_gotoxy(3,0);
	}
	lcd_print("SELECT MODE");
	_delay_ms(1000);
	m_seconds+=1000;
	convert_Mseconds();
	

	
	mode_select();
	

   lcd_clear();
   lcd_gotoxy(3,0);
   for(int a=0; a<11 ; a++){
	   lcd_print("PREPAIRING...");
	   _delay_ms(300);
	   lcd_clear();
	   lcd_gotoxy(3,0);
   }

	InitADC();
	
	lcd_clear();
	lcd_gotoxy(0,0);
	lcd_print("Hum= ");
	lcd_gotoxy(10,0);
	lcd_print("pH= ");
	lcd_gotoxy(0,1);
	lcd_print("Tem= ");


	while(mode_logic!=0)
	{
		pH_Motors();
		LDR();
		
		DHT_11();
		
		Request();
		Response();
		I_RH=Receive_data();
		D_RH=Receive_data();
		I_Temp=Receive_data();
		D_Temp=Receive_data();
		CheckSum=Receive_data();
		_delay_ms(1000);
		m_seconds+=1000;
		convert_Mseconds();
		
		DHT_11_Display();

		
	}
}





//Mode Selection
void mode_select(){
	while(mode_logic!=1){
		
		if(PINA & 0x01){
			Mode=1;
			mode_logic =1;
			temp = 20;
			hum = 90;
			Standard_pH_HIGH = 7.5;
			Standard_pH_LOW = 6.5;
		}

		else{
			if(PINA & 0x02){
				Mode=2;
				mode_logic =1;
				temp = 22;
				hum = 60;
				Standard_pH_HIGH = 8.5;
				Standard_pH_LOW = 7.0;
				
			}
			
			else{
				if(PINA & 0x04){
					Mode=3;
					mode_logic =1;
					temp = 18;
					hum = 70;
					Standard_pH_HIGH = 9.0;
					Standard_pH_LOW = 7.5;
				}
			}
		}
	}
	if(mode_logic==1)
	PORTD |= 0x10;
}




//Timer for Main_motor

void convert_Mseconds(void)
{
	//_delay_ms(10);
	//m_seconds+=10;
	//convert_Mseconds();
	m_sec = m_seconds;
	m_seconds=0;
	if (m_sec>=100)
	{
		seconds = floor(m_sec/100);
		m_sec=m_sec%100;

	}
	if (seconds>=10)
	{
		unit++;
		seconds=0;
		_delay_ms(1);
		m_seconds+=1;
		//convert_Mseconds();
	}
	if(unit<=2)
	{
		PORTD|=0x08;
	}
	else{
		if(unit<=3)
		PORTD &= ~(0x08);
		PORTD |= 0x80;
	}
	if(unit>3)
	unit=0;
}

//pH control motors activation
void pH_Motors(){
	if(PINA & 0x08){
		pH_logic = 1;
		PORTB |= 0x20;
		_delay_ms(100);
		m_seconds+=100;
		convert_Mseconds();
		PORTB &= ~(0x20);
	}

	
	else{
		if(PINA & 0x10){
			pH_logic = 2;
		}
		
		else{
			if(PINA & 40){
				pH_logic=0;
			}
		}
	}

	
	if( pH_logic == 1){
		itoa(Standard_pH_HIGH,data_pH,10);
		lcd_gotoxy(13,0);
		lcd_print(data_pH);
		lcd_gotoxy(14,0);
		pH_logic = 0;
		PORTB |= 0x10;
		_delay_ms(3000);
		m_seconds+=3000;
		convert_Mseconds();
		PORTB &= ~(0x10);
		_delay_ms(3000);
		m_seconds+=3000;
		convert_Mseconds();
	}
	else{
		if(pH_logic == 2){
			itoa(Standard_pH_LOW,data_pH,10);
			lcd_gotoxy(13,0);
			lcd_print(data_pH);
			lcd_gotoxy(14,0);
			lcd_print(".0");
			pH_logic=0;
			PORTB|= 0xC0;
			_delay_ms(3000);
			m_seconds+=3000;
			convert_Mseconds();
			PORTB &= ~(0xC0);

		}
		else{
			PORTB &= ~(0xf0);
		}

	}
	
	_delay_ms(100);
	m_seconds+=100;
	convert_Mseconds();
}




//LDR

void LDR()
{
	adc_result=ReadADC(5);
	
	if (adc_result<200)
	{
		PORTD = PORTD | (1<<0);
	}
	else{
		PORTD = PORTD & (~(1<<0));
	}

}


void InitADC()
{
	ADMUX=(1<<REFS0);									// For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// Prescalar div factor =128
}

uint16_t ReadADC(uint8_t ch)
{
	ch=ch&0b00000111;
	
	ADMUX&=0b11100000;
	ADMUX|=ch;
	//Start Single conversion
	ADCSRA|=(1<<ADSC);
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA|=(1<<ADIF);

	return(ADC);
}



//DHT_11 - Display values

void DHT_11_Display()
{
	if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
	{
		lcd_gotoxy(0,0);
		lcd_print("Error");
	}
	
	else
	{
		itoa(I_RH,data,10);
		lcd_gotoxy(4,0);
		lcd_print(data);
		lcd_print(".");
		
		itoa(D_RH,data,10);
		lcd_print(data);
		lcd_print("%");

		itoa(I_Temp,data,10);
		lcd_gotoxy(6,1);
		lcd_print(data);
		lcd_print(".");
		
		itoa(D_Temp,data,10);
		lcd_print(data);
		lcddata(0xDF);
		lcd_print("C ");
	}
	
	_delay_ms(10);
	m_seconds+=10;
	convert_Mseconds();
}

//DHT_11 - Condition checking

void DHT_11(){

	if(I_Temp > temp){
		if(I_RH < hum)
		PORTB |= 0x03;
		
		else{
			PORTB &= ~(0x02);
			PORTB |= 0x01;
		}
	}
	else{
		if(I_RH < hum){
			PORTB &= ~(0x01);
			PORTB |= 0x02;
		}
		else{
			PORTB &= ~(0x03);
		}
	}
	_delay_ms(10);
	m_seconds+=10;
	convert_Mseconds();
}




void Request()
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	// set to low pin
	_delay_ms(20);
	m_seconds+=20;
	convert_Mseconds();			// wait for 20ms
	PORTD |= (1<<DHT11_PIN);	//set to high pin
}

void Response()				// receive response from DHT11
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()			//receive data
{
	for (int q=0; q<8; q++) // 0100 0000 | 0100 0000 > 0
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  // check received bit 0 or 1
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN)) // if high pulse is greater than 30ms  12 => 8 bits  43 => 8 bits
		c = (c<<1)|(0x01);	// then its logic HIGH
		else			// otherwise its logic LOW
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}


// LCD_Display

void lcdcommand(unsigned char cmnd)
{
	LCD_DPRT = (LCD_DPRT & 0x0f)|(cmnd & 0xf0);		
	LCD_DPRT &= ~ (1<<LCD_RS);						
	LCD_DPRT |= (1<<LCD_EN);						
	_delay_us(1);									
	LCD_DPRT &= ~(1<<LCD_EN);						
	_delay_us(100);									
	
	LCD_DPRT = (LCD_DPRT & 0x0f)|(cmnd << 4);		
	LCD_DPRT |= (1<<LCD_EN);						
	_delay_us(1);									
	LCD_DPRT &= ~(1<<LCD_EN);						
	_delay_ms(2);
	m_seconds+=2;
	convert_Mseconds();									
}

void lcddata(unsigned char data)
{
	LCD_DPRT = (LCD_DPRT & 0x0f)|(data & 0xf0);		
	LCD_DPRT |= (1<<LCD_RS);						
	LCD_DPRT |= (1<<LCD_EN);						
	_delay_us(1);									
	LCD_DPRT &= ~(1<<LCD_EN);						
	_delay_us(100);									
	
	LCD_DPRT = (LCD_DPRT & 0x0f)|(data << 4);		
	LCD_DPRT |= (1<<LCD_EN);						
	_delay_us(1);									
	LCD_DPRT &= ~(1<<LCD_EN);						
	_delay_ms(2);
	m_seconds+=2;
	convert_Mseconds();								
}

void lcdinit()
{
	LCD_DDDR = 0xFF;
	_delay_ms(200);
	m_seconds+=200;
	convert_Mseconds();									
	lcdcommand(0x33);
	lcdcommand(0x32);								
	lcdcommand(0x28);								
	lcdcommand(0x0C);								
	lcdcommand(0x01);								
	_delay_ms(2);
	m_seconds+=2;
	convert_Mseconds();
	lcdcommand(0x82);								
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
	unsigned char firstcharadd[]={0x80, 0xC0};
	lcdcommand(firstcharadd[y] + x);
}

void lcd_print(char *str)
{
	unsigned char i=0;
	while (str[i] |= 0)
	{
		lcddata(str[i]);
		i++;
	}
}

void lcd_clear()
{
	lcdcommand(0x01);
	_delay_ms(2);
	m_seconds+=2;
	convert_Mseconds();
}
