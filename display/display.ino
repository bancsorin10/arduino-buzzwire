/*
 * 7 segment display layout
 * pb5 = a
 * pb4 = b
 * pb3 = c
 * pb2 = d
 * pd0 = e
 * pb0 = f
 * pd7 = g
 * pd6 = first digit
 * pd5 = second digit
 * 
 * digits are in normal logic
 * the segments are in inverse logic
 * 
 */

#include "pitches.h"

int melody[] = {
	500, 1000, 700, 3000, 
	NOTE_B4, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0, 

	NOTE_C7, 0, 0, NOTE_G6, 
	0, 0, NOTE_E6, 0, 
	0, NOTE_A6, 0, NOTE_B6, 
	0, NOTE_AS6, NOTE_A6, 0, 

	NOTE_G6, NOTE_E7, NOTE_G7, 
	NOTE_A7, 0, NOTE_F7, NOTE_G7, 
	0, NOTE_E7, 0,NOTE_C7, 
	NOTE_D7, NOTE_B6, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6, 
	0, 0, NOTE_E6, 0, 
	0, NOTE_A6, 0, NOTE_B6, 
	0, NOTE_AS6, NOTE_A6, 0, 

	NOTE_G6, NOTE_E7, NOTE_G7, 
	NOTE_A7, 0, NOTE_F7, NOTE_G7, 
	0, NOTE_E7, 0,NOTE_C7, 
	NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
	12, 12, 12, 12, 
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12, 

	12, 12, 12, 12,
	12, 12, 12, 12, 
	12, 12, 12, 12, 
	12, 12, 12, 12, 

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
};


int ms = 0, s = 0, i = 0, icr; //i < 78
char digit = 0;

void display(char c, char d)
{
	//reset
	PORTB |= 0b00111101;
	PORTD |= 0b10000001;
	PORTD &= 0b10011111;

	switch (d)
	{
		case 0:
			{
				PORTB &= 0b11000010;
				PORTD &= 0b11111110;
				//pd7 already 1 from reset
				break ;
			}
		case 1:
			{
				PORTB &= 0b11100111;
				break ;
			}
		case 2:
			{
				PORTB &= 0b11001011;
				PORTD &= 0b01111110;
				break ;
			}
		case 3:
			{
				PORTB &= 0b11000011;
				PORTD &= 0b01111111;
				break ;
			}
		case 4:
			{
				PORTB &= 0b11100110;
				PORTD &= 0b01111111;
				break ;
			}
		case 5:
			{
				PORTB &= 0b11010010;
				PORTD &= 0b01111111;
				break ;
			}
		case 6:
			{
				PORTB &= 0b11010010;
				PORTD &= 0b01111110;
				break ;
			}
		case 7:
			{
				PORTB &= 0b11000111;
				break ;
			}
		case 8:
			{
				PORTB &= 0b11000010;
				PORTD &= 0b01111110;
				break ;
			}
		case 9:
			{
				PORTB &= 0b11000010;
				PORTD &= 0b01111111;
				break ;
			}
		case '-':
			{
				PORTD &= 0b01111111;
				break ;
			}
		default: break ;
	}

	switch (c)
	{
		case 1:
			{
				PORTD |= (1 << 6);
				break ;
			}
		case 2:
			{
				PORTD |= (1 << 5);
				break ;
			}
		default: break ;
	}
}

ISR(TIMER0_COMPA_vect)
{

	digit++;
	switch(digit)
	{
		case 1: display(2,s%10); break;
		case 2: display(1,(s/10)%10); digit = 0; break;
	}
	//not actual milliseconds

	if (ms == 1499)
	{
		// set first note
		icr = (12000000 / melody[s] / 8) - 1;
		ICR1L = icr;
		ICR1H = icr >> 8;

		OCR1AL = icr/2;
		OCR1AH = (icr/2) >> 8;
		s++;
		ms = 0;
	}
	else
		ms++;
}

ISR(INT1_vect)
{
	SREG &= ~(1<<7);
	while (1)
	{
		// Serial.println("kek");
		display(2, s % 10);
		display(1, s / 10 % 10);
	}
}

ISR(INT0_vect)
{
	// game over - buzzer 'screams' 
	SREG &= ~(1<<7);
	DDRB |= 0b00000010;
	PORTB |= 0b00000010; // turn on buzzer

	while (1)
	{
		//scream until reset
		display(1, '-');
		display(2, '-');
	}
}

void set_int1()
{
	EICRA |= 0b00001100; // enable int1 interrupt on rising edge
	EIMSK |= 0b00000010; // enable interrupt on int1
	DDRD &= ~(1<<3);    // pd3 or D3 input
}

void set_int0()
{
	EICRA |= 0b00000011; // enable int0 interrupt on rising edge
	EIMSK |= 0b00000001; // enable interrupt on int0
	DDRD &= ~(1<<2);  //pd2 or D2 input
}

void set_timer1()
{
	//set tccr to 0 because arduino defaults (theu mess up our pwm setup)
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A |= 0b10000000; // non_inverting oc1a on compare match
	//TIMSK1 |= 0b00000010; // enable interrup for compare on ocr1a
	TCCR1B |= 0b00000010; // prescaler of 8
	TCCR1A |= 0b00000010; // for fast pwm
	TCCR1B |= 0b00011000; // for fast pwm with icr1 as top

	ICR1H=1000>>8;
	ICR1L=1000;
	OCR1AH=500>>8;
	OCR1AL=500;
	TCNT1H = 0;
	TCNT1L = 0;



}

void set_timer0()
{
	OCR0A = 124; // 12 MHz / 64 prescaler / 125 ocr = 1500 interrupts / second 
	TCCR0A |= 0b00000010; // CTC mode
	TCCR0B |= 0b00000011; //prescaler of 64
	TIMSK0 |= 0b00000010; // enable interrupt for ocr0a
	TCNT0 = 0;
}

void setup() {

	// put your setup code here, to run once:
	SREG = 1<<7; // global interrupts enable

	set_timer0();
	set_int1();
	set_int0();
	DDRB=0b00111111; // the 1 bit is for fast pwm 
	DDRD=0b11100001;
	set_timer1();
}

void loop() {
	// put your main code here, to run repeatedly:
}
