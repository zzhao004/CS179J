#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
//#include "L293D.h"
//#include "Adafruit_HTU21DF.h"

enum State1 {start1, wait1, scrollup1, scrolldown1, hold11, hold21, select1, reset1} state1;
enum State2 {start, wait, scrollup, scrolldown, hold1, hold2, select, reset} state2;
enum State3 {start3, wait3, incr3, decr3, hold31, hold32, set3, reset3} state3;
unsigned char A = 0x00;

//keeps track of ecosystem and reptile types
unsigned char eco_num = 0x00;
unsigned char reptile_num = 0x00;

//average temperature expected for temperature/humidity at certain times for ecosystems
float temp_goal_morningevening;
float humidity_goal_morningevening;
float temp_goal_noon;
float humidity_noon;
float temp_goal_night;
float humidity_goal_night;

//24 hour clock time stamps for feeding and light feature
unsigned short feed_time;
unsigned short morning = 21600;
unsigned short noon = 43200;
unsigned short evening = 64800;
unsigned short night = 75600;
unsigned short clock = 0x00;

//time variables
unsigned char hour;
unsigned char minutes;
unsigned char seconds;
unsigned char time_set = 0x00;

//Defined to ensure settings are confirmed before automation starts
unsigned char eco_selected = 0x00;
unsigned char reptile_selected = 0x00;

//Menu part 1, selecting ecosystem
void eco_select () {
	switch(state2) {
		case start:
			LCD_DisplayString(1, "Select Ecosystem");
			if(A == 0x01)
			{
				state2 = wait;
			}
			else if(A == 0x02)
			{
				state2 = wait;
			}
			else if(A == 0x03)
			{
				state2 = reset;
			}
			else {
				state2 = start;
			}
			break;
			
		case wait:
			if(A == 0x01) 
			{
				state2 = scrollup;
			}
			else if(A == 0x02)
			{
				state2 = scrolldown;
			}
			else if(A == 0x03)
			{
				state2 = reset;
			}
			else if(A == 0x04)
			{
				state2 = select;
			}
			else {
				state2 = wait;
			}
			break;
		case scrollup:
			if(eco_num == 0x00) {
				LCD_DisplayString(1, "Desert");
				eco_num == 0x01;
				state2 = wait;
			}
			else if (eco_num == 0x01) {
				LCD_DisplayString(1, "Tropical");
				eco_num == 0x02;
				state2 = wait;
			}
			else if (eco_num == 0x02) {
				LCD_DisplayString(1, "Mediterranean");
				eco_num == 0x00;
				state2 = wait;
			}
			break;
		case scrolldown:
			if(eco_num == 0x00) {
				LCD_DisplayString(1, "Tropical");
				eco_num == 0x02;
				state2 = wait;
			}
			else if (eco_num == 0x01) {
				LCD_DisplayString(1, "Mediterranean");
				eco_num == 0x00;
				state2 = wait;
			}
			else if (eco_num == 0x02) {
				LCD_DisplayString(1, "Desert");
				eco_num == 0x01;
				state2 = wait;
			}
			else {
				state2 = wait;
			}
			break;			
		case hold1:
			if(A == 0x00 || A == 0x01) {
				state2 = wait;
			}
			else if(A == 0x03) {
				state2 = reset;
			}
			else {
				state2 = wait;
			}
			break;
			
		case hold2:
			if(A == 0x00 || A == 0x02) {
				state2 = wait;
			}
			else if(A == 0x03) {
				state2 = reset;
			}
			else {
				state2 = wait;
			}
			break;
				
		case select:
			eco_selected = 0x01;
			state2 = wait;
			break;
			
		case reset:
			if(A == 0x03) {
				eco_num = 0x00;
				eco_selected = 0x00;
				reptile_selected = 0x00;
				state2 = start;
			}
			break;
		default:
			state2 = start;
			break;
	}
	
}

//Menu part 2, selecting reptile
void reptile_select () {
	switch(state1) {
		case start1:
			LCD_DisplayString(1, "Select Reptile");
			if(A == 0x01)
			{
				state1 = wait1;
			}
			else if(A == 0x02)
			{
				state1 = wait1;
			}
			else if(A == 0x03)
			{
				state1 = reset1;
			}
			else {
				state1 = start1;
			}
			break;
			
		case wait1:
			if(A == 0x01) 
			{
				state1 = scrollup1;
			}
			else if(A == 0x02)
			{
				state1 = scrolldown1;
			}
			else if(A == 0x03)
			{
				state1 = reset1;
			}
			else if(A == 0x04)
			{
				state1 = select1;
			}
			else {
				state1 = wait1;
			}
			break;
		case scrollup1:
			if(reptile_num == 0x00) {
				LCD_DisplayString(1, "Snake");
				reptile_num == 0x01;
				state1 = wait1;
			}
			else if (reptile_num == 0x01) {
				LCD_DisplayString(1, "Tortoise");
				reptile_num == 0x02;
				state1 = wait1;
			}
			else if (reptile_num == 0x02) {
				LCD_DisplayString(1, "Lizard");
				reptile_num == 0x00;
				state1 = wait1;
			}
			break;
		case scrolldown:
			if(reptile_num == 0x00) {
				LCD_DisplayString(1, "Tortoise");
				reptile_num == 0x02;
				state1 = wait1;
			}
			else if (reptile_num == 0x01) {
				LCD_DisplayString(1, "Lizard");
				reptile_num == 0x00;
				state1 = wait1;
			}
			else if (reptile_num == 0x02) {
				LCD_DisplayString(1, "Snake");
				reptile_num == 0x01;
				state1 = wait1;
			}
			else {
				state1 = wait1;
			}
			break;			
			break;
		case hold11:
			if(A == 0x00 || A == 0x01) {
				state1 = wait1;
			}
			else if(A == 0x03) {
				state1 = reset1;
			}
			else {
				state1 = wait1;
			}
			break;
			
		case hold21:
			if(A == 0x00 || A == 0x02) {
				state1 = wait1;
			}
			else if(A == 0x03) {
				state1 = reset1;
			}
			else {
				state1 = wait1;
			}
			break;
				
		case select1:
			reptile_selected = 0x01;
			state1 = wait1;
			break;
			
		case reset1:
			if(A == 0x03) {
				eco_num = 0x00;
				reptile_num = 0x00;
				eco_selected = 0x00;
				reptile_selected = 0x00;
				state1 = start1;
			}
			break;
		default:
			state1 = start1;
			break;
	}
	
}

//Setting temperature and humidity
void set_hum_temp_feed () {
	if (eco_num == 0x00 && reptile_num == 0x00) {
		LCD_DisplayString(1, "Med. Lizard");
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 43200;
	}
	else if (eco_num == 0x00 && reptile_num == 0x01) {
		LCD_DisplayString(1, "Med. Snake");
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 43200;
	}
	else if (eco_num == 0x00 && reptile_num == 0x02) {
		LCD_DisplayString(1, "Med. Tort");
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 43200;
	}
	else if (eco_num == 0x01 && reptile_num == 0x00) {
		LCD_DisplayString(1, "Desert Lizard");
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time = 64800;
	}
	else if (eco_num == 0x01 && reptile_num == 0x01) {
		LCD_DisplayString(1, "Desert Snake");
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time = 77760;
	}
	else if (eco_num == 0x01 && reptile_num == 0x02) {
		LCD_DisplayString(1, "Desert Tort");
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time =	21600;
	}
	else if (eco_num == 0x02 && reptile_num == 0x00) {
		LCD_DisplayString(1, "Trop. Lizard");
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 43200;
	}
	else if (eco_num == 0x02 && reptile_num == 0x01) {
		LCD_DisplayString(1, "Trop. Snake");
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 43200;
	}
	else if (eco_num == 0x02 && reptile_num == 0x02) {
		LCD_DisplayString(1, "Trop. Tort");
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 29000;
	}
}

//sets 24 hour clock 
void set_time() {
	switch(state3) {
		case start3:
			LCD_DisplayString(1, "Set Clock");
			LCD_Cursor(2);
			if(A == 0x01)
			{
				state3 = wait3;
			}
			else if(A == 0x02)
			{
				state3 = wait3;
			}
			else if(A == 0x03)
			{
				state3 = reset3;
			}
			break;
		case wait3:
			if(A == 0x01)
			{
				state3 = incr3;
			}
			else if(A == 0x02)
			{
				state3 = decr3;
			}
			else if(A == 0x03)
			{
				state2 = reset3;
			}
			else if(A == 0x04)
			{
				state3 = set3
			}
			break;		
		case incr3:
			break;
		case decr3:
			break;
		case hold31:
			break;
		case hold32:
			break;
		case set3:
			time_set = 0x01;
			break;
		case reset3:
			hour = 0;
			minutes = 0;
			seconds = 0;
			time_set = 0x00;
			state3 = start3;
			break;
		default:
			state3 = start3;
	}
}

volatile unsigned char TimerFlag = 0;

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
	TimerISR(); // Call the ISR that the user uses
	_avr_timer_cntcurr = _avr_timer_M;
}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

int main(void)
{ 	
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	state2 = wait;
	
	LCD_init(); 
	LCD_Cursor(1);
	TimerSet(1);
	TimerOn();
	
    while(1)
    {
		A = PINA & 0x07;
		if (eco_selected == 0x00) {
			eco_select();
		}
		if (eco_selected == 0x01 && reptile_selected == 0x00)  {	
			reptile_select();
		}
		if (eco_selected == 0x01 && reptile_selected == 0x01){
			set_hum_temp_feed();
		}
		if(eco_selected == 0x01 && reptile_selected == 0x01 && time_set == 0x00){
			set_time();
		}
		if(time_set == 0x01) {
			
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
    }
}