/*
 * ECE388Climate.c
 *
 * Created: 11/17/2016 10:50:30 PM
 * Author : Johnny
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif


#define D4 PD7
#define D5 PB0
#define D6 PB1
#define D7 PB2
#define RS PD5
#define E PD6

#define DHT PD2

#define UP PD3
#define DOWN PD4

#define PIR1 PC3
#define PIR2 PC4

#define LED PC5

#define FAN PC2

#define RELAY1 PC0
#define RELAY2 PC1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int roomCount; 

unsigned int current_temp = 0;
unsigned int humidity = 0;
unsigned int set_temp;

int main(void)
{
    
	DDRD = 0xff;
	DDRC = 0xff;

	//PIR ISR
	DDRC &= ~(1<<DDC3); //PIR 1
	DDRC &= ~(1<<DDC4); //PIR 2
	PCMSK1 |= 1<<PCINT11 | 1<<PCINT22;
	PCICR |= 1<<PCIE1;

	sei();	

    while (1) 
    {
		
		//Max Capacity - Enable LED
		if (roomCount > 5)
		PORTC |= (1<<LED);
		//OK Capacity - Ensure LED is off
		if (roomCount < 5)
		PORTC |= (0<<LED);
		//If roomCount goes negative, it is reset to 0
		if (roomCount < 0)
		roomCount = 0; 
		//Enable Relay 1
		if (roomCount < 1)
		PORTC |= (1<<RELAY1);
		//Disable Relay 1 
		if (roomCount < 1)
		PORTC |= (0<<RELAY1);

    }
	
}

ISR(PCINT1_vect)
{
	if(PIR1 == 0)
	{
		if(PIR2 == 0){
			roomCount++;
			PORTC |= (0<<PIR2);
		}
	}
	
	else
	{
		if(PIR2 == 0)
			PORTC |= (1<<PIR2);
	}
}



