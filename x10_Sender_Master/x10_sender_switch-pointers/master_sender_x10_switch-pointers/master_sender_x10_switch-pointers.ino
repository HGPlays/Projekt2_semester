#include <avr/io.h>
#include <avr/interrupt.h>

char nextBitArray1[11] = { '1','1','1','1','1','1','1','1','1','1', '\0'};	// global array of bits that needs to be send to slaves 
char nextBitArray2[11] = { '1','1','1','1','1','1','1','1','0','0', '\0' };
char nextBitArray3[11] = { '0','1','0','1','0','1','0','1','0','1', '\0' };
volatile char* bit;														// global pointer to array of bits
char a = 'b';// global char for switch case
int zeroCounter=3;
volatile bool stopReached = true;
volatile bool intReached = true;
volatile bool sentTwice = false;

ISR(TIMER1_COMPA_vect);													// ISR prototype
ISR(TIMER3_OVF_vect);													// ISR prototype
	
ISR(TIMER3_OVF_vect)
{
	digitalWrite(11, LOW);														// dummy burst signal off
	TIMSK3 = 0x00;														// disables overflow interrupt for timer3
}

ISR(TIMER1_COMPA_vect)													// overflow from timer1
{
	Serial.print("pointeren er:  ");
	Serial.print(*bit);
	Serial.print("\n");



	if(zeroCounter>2)
	{
		return;
	}
	intReached = true;
	if (*bit == '0')
	{
		Serial.print("test1\n");

		digitalWrite(11, LOW);													// if pointer points to 0 turn off all LEDS
		bit++;
		zeroCounter++;
	
		
	}
	else if (*bit == '1')
	{
		Serial.print("test2\n");
		digitalWrite(11, HIGH);													// if pointer points to 1 turn on all LEDS
		bit++;
		zeroCounter = 0;
		
	}
	else if (*bit == '\0' )
	{
		Serial.print("\nSentinal reached\n");
		if (sentTwice)
		{
			sentTwice = false;
			stopReached = true;
			
			bit -= 10;
		}
		else
		{
			sentTwice = true;
			bit -= 10;
		}
	}
	else
	{
		Serial.print("hello\n");

		PORTB = 0b11110000;
	}
	TIMSK3 = 0x01;														// overflow interrupt enable
	TCNT3 = 63536;														// x-value sent to timer 3 for overflow after 1 ms

}

void switchFunction() {
	switch (a)
	{
	case 'a':	// if case is 'k'
		
		TIMSK1 = 0x00;													// disable interrupt timer1

		intReached = false;
		zeroCounter = 0;

		bit = nextBitArray1;											// change global pointer placement

		TIFR1 |= 0b00000010;											// make sure reset flag is reset by writing a 1 to the TIFR flag
		TIMSK1 = 0x02;					// enable interrupt
		break;

	case 'b':													// if case is 'f'
		intReached = false;

		TIMSK1 = 0x00;													// disable interrupt timer1

		zeroCounter = 0;
		bit = nextBitArray2;											// change global pointer placement

		TIFR1 |= 0b00000010;											// make sure reset flag is reset by writing a 1 to the TIFR flag
		TIMSK1 = 0x02;													// enable interrupt
		break;
			
	case 'c':													// if case is 'h'
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
	 if (intReached && stopReached)
	 {

		 switchFunction();
	 }
 }


