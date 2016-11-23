/*
 * Climate_Control.c
 *
 * Created: 11/19/2016 7:25:49 PM
 * Author : Eric
 */ 
#define F_CPU 16000000UL
#define SWITCH_PRESSED !(PINB & (1<<PINB7))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "dht22.h"

volatile int setTemp = 0;

ISR(PCINT0_vect)
{
	setTemp++;
}
int main(void)
{
	 unsigned char buf[3];
	 // Set LCD Outputs and Initialize
     DDRD |= (1<<DDD5);    
     DDRD |= (1<<DDD6);
     DDRD |= (1<<DDD7);
     DDRB |= (1<<DDB0);
     DDRB |= (1<<DDB1);
     DDRB |= (1<<DDB2);
	 Lcd4_Clear();
	 Lcd4_Init();
	 Lcd4_Set_Cursor(1,1);
	 Lcd4_Write_String("TEMP  SET  HUM%");
	 // Initialize Button UI
	 DDRB &= ~(1<<DDB7);		// Set PB7 as Input
	 PORTB |= (1<<PORTB7);	// Enable PB7 pull-up
	 PCMSK0 |= (1<<PCINT7); // Mask Button
	 PCICR |= (1<<PCIE0);	// Enable interrupt bit
	 sei();
	 
    while (1) 
    {
		/*itoa(setTemp,buf,10);
		Lcd4_Set_Cursor(2,2);
		Lcd4_Write_String(buf);	*/
    }
}

