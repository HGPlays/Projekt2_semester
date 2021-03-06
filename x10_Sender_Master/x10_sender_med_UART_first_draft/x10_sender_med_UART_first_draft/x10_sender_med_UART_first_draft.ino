#include <avr/io.h>
#include <avr/interrupt.h>
#define SIZE 12

char nextBitArray1[SIZE] = { '1','1','1','1','1','1','1','0','0','0','0','\n' }; 	// global array of bits that needs to be send to slaves 
char nextBitArray2[SIZE] = "11111111000";
char nextBitArray3[SIZE] = "01010101000";
volatile char* bit;														// global pointer to array of bits
int incomingChar;// global char for switch case
int zeroCounter = 3;
int sentCounter;

volatile bool stopReached = true;
volatile bool intReached = true;
volatile bool sentTwice = false;

ISR(TIMER1_COMPA_vect);													// ISR prototype
//ISR(TIMER3_OVF_vect);													// ISR prototype
/*
ISR(TIMER3_OVF_vect)
{
														// dummy burst signal off
	TIMSK3 = 0x00;														// disables overflow interrupt for timer3
}
*/
ISR(TIMER1_COMPA_vect)													// overflow from timer1
{
	//if (zeroCounter > 2)
	if (!( * bit == '0' || *bit == '1')||*bit=='\n')
	{
		/*Serial.print(*bit);*/
		
		return;
	}
	if (stopReached)
	{
		return;
	}

	if (sentCounter >= SIZE-1)
	{
		sentCounter = 0;
		if (sentTwice)
		{
			stopReached = true;
			sentTwice = false;
					
		}
		else
		{
			/*Serial.print("\n\nsending twice\n\n");*/
			sentTwice = true;
		}
		return;
	}
	
	/*Serial.print("pointeren er:  ");
	Serial.print(bit[sentCounter]);
	Serial.print("\n");*/
	intReached = true;
	if (bit[sentCounter] == '0')
	{
		/*Serial.print("test1\n");*/

		digitalWrite(8, LOW);													// if pointer points to 0 turn off all LEDS
		
		sentCounter++;

	}
	else if (bit[sentCounter] == '1')
	{
		/*Serial.print("test2\n");*/
		digitalWrite(8, HIGH);													// if pointer points to 1 turn on all LEDS
		sentCounter++;
		_delay_ms(2);
	}
	else
	{
		/*Serial.print("hello\n");*/

		PORTB = 0b11110000;
	}
	//TIMSK3 = 0x01;														// overflow interrupt enable
	//TCNT3 = 63536;														// x-value sent to timer 3 for overflow after 1 ms

}

void switchFunction() {
	stopReached = false;// soerger for switchFunction ikke kaldes igen foer kommandoen er udfoert
	switch (incomingChar)
	{
	case 49:	// if case is 'k'

		TIMSK1 = 0x00;													// disable interrupt timer1

		intReached = false;
		zeroCounter = 0;

		bit = nextBitArray1;											// change global pointer placement

		TIFR1 |= 0b00000010;											// make sure reset flag is reset by writing a 1 to the TIFR flag
		TIMSK1 = 0x02;					// enable interrupt
		break;

	case 50:													// if case is 'f'
		intReached = false;

		TIMSK1 = 0x00;													// disable interrupt timer1

		zeroCounter = 0;
		bit = nextBitArray2;											// change global pointer placement

		TIFR1 |= 0b00000010;											// make sure reset flag is reset by writing a 1 to the TIFR flag
		TIMSK1 = 0x02;													// enable interrupt
		break;

	case 51:													// if case is 'h'
		intReached = false;
		TIMSK1 = 0x00;													// disable interrupt timer1
		zeroCounter = 0;

		bit = nextBitArray3;											// change global pointer placement

		TIFR1 |= 0b00000010;											// make sure reset flag is reset by writing a 1 to the TIFR flag
		TIMSK1 = 0x02;													// enable interrupt
		break;

	default:															// default case

		break;
	}
}


void setup()
{
	Serial.begin(9600);
	pinMode(11, OUTPUT);
	TCCR1B = 0b00001100;												// setup timer1
	TCCR1A = 0x00;														//
	TCCR3B = 0x02;														// setup timer3
	OCR1A = 624;														// make timer start at 624
	TIMSK1 = 0x02;														// enable interrupts
	DDRB = 0xFF;														// set portB to output
	sei();																// enable global interrupts
}

void loop()
{
	if ((intReached && stopReached)&&(Serial.available()>0))
	{

		Serial.print("command recieved");
		cli();
		incomingChar = Serial.read();//loader char'en ind i incomingChar til switch casen.
		Serial.print("I received: ");
		Serial.println(incomingChar);

		switchFunction();
		sei();
	}

}