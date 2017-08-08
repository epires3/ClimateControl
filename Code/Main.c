/*
 * Climate_Control.c
 *
 * Created: 11/19/2016 7:25:49 PM
 * Author : Eric
 */ 
#define F_CPU 16000000UL
#define FAN_PORT PORTC
#define HEATER_PIN PORTC1
#define COOLER_PIN PORTC2
#define IN_TRIGGER !(PINC & (1<<PINC3))
#define OUT_TRIGGER !(PINC & (1<<PINC4))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))
#define TOGGLEBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define SWITCH_PRESSED !(PINB & (1<<PINB7))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcd.h"
#include "dht22.h"

volatile float currentTemp = 0;
volatile float setTemp = 0;
volatile int lsIn = 0;
volatile int roomCnt = 0;

ISR(PCINT0_vect)
{
	if(SWITCH_PRESSED)
		setTemp++;
}

ISR(PCINT1_vect)
{
	if(IN_TRIGGER)
	{										// Check for Falling Edge
		if(!lsIn)
		{
			roomCnt++;
			lsIn = 0;//TOGGLEBIT(PINC,PINC3);			// Toggles lsIn
		}
	}
	
	else
	{										// Check for Rising Edge
		if (lsIn)
			lsIn = 1;//TOGGLEBIT(PINC,PINC3);
	}
	if(roomCnt > 10)
		PORTC |= (1<<PORTC5);
	else
		PORTC &= ~(1<<PORTC5);
	_delay_us(350);
}

ISR(PCINT2_vect)
{
	if(IN_TRIGGER)
	{										// Check for Falling Edge
		if(!lsIn)
		{
			roomCnt--;
			lsIn = 0;//TOGGLEBIT(PINC,PINC3);			// Toggles lsIn
		}
	}
	
	else
	{										// Check for Rising Edge
		if (lsIn)
		lsIn = 1;//TOGGLEBIT(PINC,PINC3);
	}
	if(roomCnt > 5)
		PORTC |= (1<<PORTC5);
	else
		PORTC &= ~(1<<PORTC5);
	_delay_us(350);
}
int main(void)
{
	 // Set LCD Outputs and Initialize
     DDRD |= (1<<DDD5);    
     DDRD |= (1<<DDD6);
     DDRD |= (1<<DDD7);
     DDRB |= (1<<DDB0);
     DDRB |= (1<<DDB1);
     DDRB |= (1<<DDB2);
	 Lcd4_Clear();
	 Lcd4_Init();
	 Lcd4_Set_Cursor(0,1);
	 Lcd4_Write_String("TEMP  SET  HUM%");
	 // Initialize Fan Outputs
	 DDRC |= (1<<DDC1);
	 DDRC |= (1<<DDC2);
	 // Initialize Button UI Inputs
	 DDRB &= ~(1<<DDB7);	// Set PB7 as Input
	 PORTB |= (1<<PORTB7);	// Enable PB7 pull-up
	 PCMSK0 |= (1<<PCINT7); // Mask Button
	 PCICR |= (1<<PCIE0);	// Enable interrupt bit
	 // Initialize IR Inputs and Outputs
	 DDRC &= ~(1<<DDC3); // PIR 1 as input
	 DDRC &= ~(1<<DDC4); // PIR 2 as input
	 PORTC |= (1<<PORTC3) | (1<<PORTC4);	// Enable pull-ups
	 DDRC |= (1<<DDC5);	// MAX as output
	 PCMSK1 |= (1<<PCINT11) | (1<<PCINT22);
	 PCICR |= (1<<PCIE1) | (1<<PCIE2);
	 // Get current temp to initialize set temp
	 dht_gettemperature(&setTemp); 
	 // Relay Outputs
	 DDRC |= (1<<DDC0) | (1<<DDC1);
	 sei();
	 
    while (1) 
    {
		temp_hum_disp();
		fanControl();
    }
}

void temp_hum_disp()
{
	char buf[3];
	float humidity = 0;
	Lcd4_Set_Cursor(2,0);
	dht_gettemperaturehumidity(&currentTemp,&humidity);
	dtostrf(currentTemp,1,1,buf);
	Lcd4_Write_String(buf);
	Lcd4_Write_String("  ");
	itoa(setTemp,buf,10);
	Lcd4_Write_String(buf);
	Lcd4_Write_String("   ");
	dtostrf(humidity,1,1,buf);
	Lcd4_Write_String(buf);
}

void fanControl()
{
	// Cooler is activated, heater is deactivated (Cools room)
	if(currentTemp > setTemp)
	{
		FAN_PORT &= ~(1 << HEATER_PIN);
		FAN_PORT |= (1 << COOLER_PIN);
	}
	// Heater is activated, cooler is deactivated (Heats room)
	else if(currentTemp < setTemp)
	{
		FAN_PORT &= ~(1 << COOLER_PIN);
		FAN_PORT |= (1 << HEATER_PIN);
	}
	// Heater and cooler deactivated (Temperature matches user input)
	else
	{
		FAN_PORT &= ~(1 << COOLER_PIN);
		FAN_PORT &= ~(1 << HEATER_PIN);
	}
}


