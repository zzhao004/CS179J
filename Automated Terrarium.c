#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "timer.h"
#include <stdlib.h>
#include <util/delay.h>
//#include "L293D.h"
//#include "Adafruit_HTU21DF.h"


enum State1 {start1, wait1, scrollup1, scrolldown1, hold11, hold21, select1, reset1} state1;
enum State2 {start, wait, scrollup, scrolldown, hold1, hold2, select, reset} state2;
enum State3 {start3, wait3, hold31, hold32, set3, reset3, hardreset} stateHours;
enum State4 {startM, waitM, holdM1, holdM2, setM, resetM, hardresetM} stateMinutes;
enum State5 {startManual, pressed} stateManual;
enum State6 {startManual2, pressed2} stateManual2;
	
	
unsigned char A = 0x07;
unsigned char B = 0x03;
unsigned char light1 = 0x00;
unsigned char light2 = 0x00;

//checks whether or not the user overrides the automated lights
unsigned char manual_activated = 0x00;
unsigned char manual_activated2 = 0x00;

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
unsigned char feed_time;
unsigned char morning = 6;
unsigned char noon = 12;
unsigned char evening = 18;
unsigned char night = 21;
unsigned long clock1 = 0x00;


//time variables
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;

unsigned char displaytime [32] = "Time: 00:00:00  Feed: 00:00:00  ";

unsigned char time_set = 0x00;
unsigned char hours_set = 0x00;
ISR(TIMER1_COMPA_vect);

//Defined to ensure settings are confirmed before automation starts
unsigned char eco_selected = 0x00;
unsigned char reptile_selected = 0x00;
unsigned char confirmed = 0x00;

//Menu part 1, selecting ecosystem
void eco_select () {
	switch(state2) {
		case start:
			LCD_Cursor(1);
			LCD_DisplayString(1, "Select Ecosystem");
			if(A == 0x06)
			{
				state2 = wait;
			}
			else if(A == 0x05)
			{
				state2 = wait;
			}
			else if(A == 0x04)
			{
				state2 = reset;
			}
			else {
				state2 = start;
			}
			break;
			
		case wait:
			if(A == 0x06)
			{
				state2 = scrollup;
			}
			else if(A == 0x05)
			{
				state2 = scrolldown;
			}
			else if(A == 0x04)
			{
				state2 = reset;
			}
			else if(A == 0x03)
			{
				state2 = select;
			}
			else {
				state2 = wait;
			}
			break;
		//allows user to scroll through environment menu options
		case scrollup:
			if(eco_num == 0x00) {
				LCD_DisplayString(1, "Desert");
				eco_num = 0x01;
				state2 = wait;
			}
			else if (eco_num == 0x01) {
				LCD_DisplayString(1, "Tropical");
				eco_num = 0x02;
				state2 = wait;
			}
			else if (eco_num == 0x02) {
				LCD_DisplayString(1, "Mediterranean");
				eco_num = 0x00;
				state2 = wait;
			}
			break;
		case scrolldown:
			if(eco_num == 0x00) {
				LCD_DisplayString(1, "Tropical");
				eco_num = 0x02;
				state2 = wait;
			}
			else if (eco_num == 0x01) {
				LCD_DisplayString(1, "Mediterranean");
				eco_num = 0x00;
				state2 = wait;
			}
			else if (eco_num == 0x02) {
				LCD_DisplayString(1, "Desert");
				eco_num = 0x01;
				state2 = wait;
			}
			else {
				state2 = wait;
			}
			break;
			case hold1:
			if(A == 0x07 || A == 0x06) {
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
			if(A == 0x07 || A == 0x05) {
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
			if(A == 0x04) {
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
		if(A == 0x06)
		{
			state1 = wait1;
		}
		else if(A == 0x05)
		{
			state1 = wait1;
		}
		else if(A == 0x04)
		{
			state1 = reset1;
		}
		else {
			state1 = start1;
		}
		break;
		
		case wait1:
		if(A == 0x06)
		{
			state1 = scrollup1;
		}
		else if(A == 0x05)
		{
			state1 = scrolldown1;
		}
		else if(A == 0x04)
		{
			state1 = reset1;
		}
		else if(A == 0x03)
		{
			state1 = select1;
		}
		else {
			state1 = wait1;
		}
		break;
		//allows user to scroll through reptile menu options
		case scrollup1:
		if(reptile_num == 0x00) {
			LCD_DisplayString(1, "Snake");
			reptile_num = 0x01;
			state1 = wait1;
		}
		else if (reptile_num == 0x01) {
			LCD_DisplayString(1, "Tortoise");
			reptile_num = 0x02;
			state1 = wait1;
		}
		else if (reptile_num == 0x02) {
			LCD_DisplayString(1, "Lizard");
			reptile_num = 0x00;
			state1 = wait1;
		}
		break;
		case scrolldown:
		if(reptile_num == 0x00) {
			LCD_DisplayString(1, "Tortoise");
			reptile_num = 0x02;
			state1 = wait1;
		}
		else if (reptile_num == 0x01) {
			LCD_DisplayString(1, "Lizard");
			reptile_num = 0x00;
			state1 = wait1;
		}
		else if (reptile_num == 0x02) {
			LCD_DisplayString(1, "Snake");
			reptile_num = 0x01;
			state1 = wait1;
		}
		else {
			state1 = wait1;
		}
		break;
		case hold11:
		if(A == 0x07 || A == 0x06) {
			state1 = wait1;
		}
		else if(A == 0x04) {
			state1 = reset1;
		}
		else {
			state1 = wait1;
		}
		break;
		
		case hold21:
		if(A == 0x07 || A == 0x05) {
			state1 = wait1;
		}
		else if(A == 0x04) {
			state1 = reset1;
		}
		else {
			state1 = wait1;
		}
		break;
		
		case select1:
		reptile_selected = 0x01;
		set_hum_temp_feed();
		state1 = wait1;
		break;
		
		case reset1:
		if(A == 0x04) {
			eco_num = 0x00;
			reptile_num = 0x00;
			eco_selected = 0x00;
			reptile_selected = 0x00;
			state1 = start1;
			state2 = start;
		}
		break;
		default:
		state1 = start1;
		break;
	}
	
}

//Sets temperature and humidity once user completed initial selection
void set_hum_temp_feed () {
	if (eco_num == 0x00 && reptile_num == 0x00) {
		displaytime[22] = '1';
		displaytime[23] = '2';
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 11;
	}
	else if (eco_num == 0x00 && reptile_num == 0x01) {
		displaytime[22] = '1';
		displaytime[23] = '2';
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 11;
	}
	else if (eco_num == 0x00 && reptile_num == 0x02) {
		displaytime[23] = '9';
		temp_goal_morningevening = 70;
		humidity_goal_morningevening = 80;
		temp_goal_noon = 85;
		humidity_noon = 40;
		temp_goal_night = 65;
		humidity_goal_night = 60;
		feed_time = 9;
	}
	else if (eco_num == 0x01 && reptile_num == 0x00) {
		displaytime[22] = '1';
		displaytime[23] = '8';
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time = 18;
	}
	else if (eco_num == 0x01 && reptile_num == 0x01) {
		displaytime[22] = '2';
		displaytime[23] = '1';
		displaytime[25] = '3';
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time = 19;
	}
	else if (eco_num == 0x01 && reptile_num == 0x02) {
		displaytime[23] = '6';
		temp_goal_morningevening = 80;
		humidity_goal_morningevening = 70;
		temp_goal_noon = 100;
		humidity_noon = 20;
		temp_goal_night = 70;
		humidity_goal_night = 40;
		feed_time =	6;
	}
	else if (eco_num == 0x02 && reptile_num == 0x00) {
		displaytime[22] = '1';
		displaytime[23] = '2';
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 12;
	}
	else if (eco_num == 0x02 && reptile_num == 0x01) {
		displaytime[22] = '1';
		displaytime[23] = '2';
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 12;
	}
	else if (eco_num == 0x02 && reptile_num == 0x02) {
		displaytime[23] = '8';
		temp_goal_morningevening = 75;
		humidity_goal_morningevening = 90;
		temp_goal_noon = 85;
		humidity_noon = 90;
		temp_goal_night = 75;
		humidity_goal_night = 90;
		feed_time = 8;
	}
}

//asks user to confirm initial setup choices 
// void confirm_selected() {
// 	switch(state_conf) {
// 		case start_conf:
// 		if(A == 0x03) {
// 			confirmed = 0x01;
// 		}
// 		else if(A == 0x06) {
// 			state_conf = reset_conf;
// 		}
// 		break;
// 		case reset_conf:
// 		if(A == 0x06) {
// 			eco_num = 0x00;
// 			reptile_num = 0x00;
// 			eco_selected = 0x00;
// 			reptile_selected = 0x00;
// 		}
// 		break;
// 	}
// }

//requests user to set up 24 hour clock, starts with hour
void set_timeHours() {
	switch(stateHours) {
		case start3:
			LCD_DisplayString(1, "Set Clock");
			if(A == 0x06)
			{
				stateHours = wait3;
			}
			else if(A == 0x05)
			{
				stateHours = wait3;
			}
			else if(A == 0x04)
			{
				stateHours = reset3;
			}
			else if(A == 0x00)
			{
				stateHours = hardreset;
			}
			break;
		case wait3:
			if(A == 0x06)
			{
				hours++;
				if(hours == 24) {
					hours = 0;
				}
				display_time();
				stateHours = wait3;
			}
			else if(A == 0x05)
			{
				if(hours == 0) {
					hours = 24;
				}
				hours--;
				display_time();
				stateHours = wait3;
			}
			else if(A == 0x04)
			{
				stateHours = reset3;
			}
			else if(A == 0x03)
			{
				stateHours = set3;
			}
			else if(A == 0x00)
			{
				stateHours = hardreset;
			}
			break;
		case hold31:
			if(A == 0x07 || A == 0x05) {
				stateHours = wait3;
			}
			else if(A == 0x04)
			{
				stateHours = reset3;
			}
			else {
				stateHours = wait3;
			}
			break;
		case hold32:
			if(A == 0x07 || A == 0x06) {
				stateHours = wait3;
			}
			else if(A == 0x04)
			{
				stateHours = reset3;
			}
			else {
				stateHours = wait3;
			}
			break;
		case set3:
			hours_set = 0x01;
			stateHours = wait3;
			break;
		case reset3:
			hours = 0;
			minutes = 0;
			seconds = 0;
			hours_set = 0x00;
			stateHours = start3;
			break;
		case hardreset:
			if(A == 0x00) {
				hours = 0;
				minutes = 0;
				seconds = 0;
				hours_set = 0x00;
				eco_num = 0x00;
				reptile_num = 0x00;
				eco_selected = 0x00;
				reptile_selected = 0x00;
				stateHours = start3;
				state1 = start1;
				state2 = start;
			}
		default:
			stateHours = start3;
	}
}

// once user sets hour time, requests user to set minute time
void set_timeMinutes () {
	switch(stateMinutes) {
		case startM:
			if(A == 0x06)
			{
				stateMinutes = waitM;
			}
			else if(A == 0x05)
			{
				stateMinutes = waitM;
			}
			else if(A == 0x04)
			{
				stateMinutes = resetM;
			}
			else if(A == 0x00)
			{
				stateMinutes = hardresetM;
			}
			break;
		case waitM:
			if(A == 0x06)
			{
				minutes++;
				if(minutes == 60) {
					minutes = 0;
				}
				display_time();
				stateMinutes = waitM;
			}
			else if(A == 0x05)
			{
				if(minutes == 0) {
					minutes = 60;
				}
				minutes--;
				display_time();
				stateMinutes = waitM;
			}
			else if(A == 0x04)
			{
				stateMinutes = resetM;
			}
			else if(A == 0x03)
			{
				stateMinutes = setM;
			}
			else if(A == 0x00)
			{
				stateMinutes = hardresetM;
			}
			break;
		case holdM1:
			if(A == 0x07 || A == 0x05) {
				stateMinutes = waitM;
			}
			else if(A == 0x04)
			{
				stateMinutes = resetM;
			}
			else {
				stateMinutes = waitM;
			}
			break;
		case holdM2:
			if(A == 0x07 || A == 0x06) {
				stateMinutes = waitM;
			}
			else if(A == 0x04)
			{
				stateMinutes = resetM;
			}
			else {
				stateMinutes = waitM;
			}
			break;
		case setM:
			time_set = 0x01;
			break;
		case resetM:
			hours = 0;
			minutes = 0;
			seconds = 0;
			hours_set = 0x00;
			stateMinutes = startM;
			stateHours = start3;
			break;
		case hardresetM:
			if(A == 0x00) {
				hours = 0;
				minutes = 0;
				seconds = 0;
				hours_set = 0x00;
				eco_num = 0x00;
				reptile_num = 0x00;
				eco_selected = 0x00;
				reptile_selected = 0x00;
				stateMinutes = startM;
				stateHours = start3;
				state1 = start1;
				state2 = start;
			}
		default:
			stateMinutes = startM;
	}
}

//once initial time is set, starts clock
void clock_SM_tick() {
	if (seconds < 60) {
		seconds++;
	}
	if (seconds == 60) {
		if (minutes < 60) {
			minutes++;
		}
		seconds = 0;
	}
	if (minutes == 60) {
		if (hours < 24) {
			hours++;
		}
		minutes = 0;
	}
	if (hours == 24) {
		hours = 0;
	}
	//variables trickle down from seconds to minutes to hours
	display_time();
	
	//if user likes to do a hardreset after initial settings
	if(A == 0x00) {
		hours = 0;
		minutes = 0;
		seconds = 0;
		hours_set = 0x00;
		eco_num = 0x00;
		reptile_num = 0x00;
		eco_selected = 0x00;
		reptile_selected = 0x00;
		stateMinutes = startM;
		stateHours = start3;
		state1 = start1;
		state2 = start;
	}
}

//updates the LCD string for time display
void display_time()
{
	displaytime[6] = hours/10 + '0';
	displaytime[7] = hours%10 + '0';
	displaytime[9] = minutes/10 + '0';
	displaytime[10] = minutes%10 + '0';
	displaytime[12] = seconds/10 + '0';
	displaytime[13] = seconds%10 + '0';
	
	LCD_DisplayString(1, displaytime);
}

//converts time to seconds for light/step motor activation
// void calc_secondsII () {
// 	secondsII = secondsII + seconds;
// 	secondsII = secondsII + minutes * 60;
// 	secondsII = secondsII + hours * 60 * 60;
// }


//turns on/off LED light for day/night time simulation
void day_night_light_SM() {
	if(hours >= morning && hours < evening && manual_activated == 0x00 ) {
		PORTD = (PORTD & 0x02); 
	}
	else if(hours >= evening && manual_activated == 0x00) {
		PORTD = (PORTD & 0x00);
	}
	//if user manually activates a light, all lights will go back to automation at the 6 and 18 oclock hour
	if(hours == evening && minutes == 0x00 && seconds == 0x00 && manual_activated == 0x01) {
		manual_activated == 0x00;
		manual_activated == 0x00;
	}
	if(hours == morning && minutes == 0x00 && seconds == 0x00 && manual_activated == 0x01) {
		manual_activated == 0x00;
		manual_activated == 0x00;
	}
}

//allows the user to manually control LED light
void day_night_light_manual() {
	switch(stateManual) {
		case startManual:
			if(B == 0x01) {
				stateManual = pressed;
				manual_activated = 0x01;
			}
			else {
				stateManual = startManual;
			}
			break;
		case pressed:
			if(B == 0x01) {
				if(light1 == 0x00) {
					PORTD = (PORTD ^ 0x01); 
				}
				else if(light1 == 0x01) {
					PORTD = (PORTD ^ 0x01);
				}
			}
			stateManual = startManual;
			break;
		default:
			stateManual = startManual;
			break;
	}
}

//turns on/off heating light every 30 minutes at the 45 and 15 minute mark of every hour
void heat_light_SM() {
	if(minutes == 45 && seconds == 30 && manual_activated == 0x00 && eco_num != 0x01) {
		PORTD = (PORTD ^ 0x02);
	}
	if(minutes == 15 && seconds == 30 && manual_activated == 0x00 != 0x01) {
		PORTD = (PORTD ^ 0x02);
	}
	
	//sets different schedule for desert environment
	if(minutes == 45 && seconds == 30 && manual_activated == 0x00 && eco_num == 0x01 && hours <= night) {
		PORTD = (PORTD ^ 0x02);
	}
	if(minutes == 30 && seconds == 30 && manual_activated == 0x00 && eco_num == 0x01 && hours <= night) {
		PORTD = (PORTD ^ 0x02);
	}
	
	//if user manual activates a light, all lights will go back to automation at the 6 and 18 oclock hour
	if(hours == evening && minutes == 0x00 && seconds == 0x00 && manual_activated == 0x01) {
		manual_activated = 0x00;
		manual_activated2 = 0x00;
	}
	if(hours == morning && minutes == 0x00 && seconds == 0x00 && manual_activated == 0x01) {
		manual_activated = 0x00;
		manual_activated2 = 0x00;
	}
}

//allows the user to manually control heating light
void heat_light_manual() {
	switch(stateManual2) {
		case startManual2:
			if(B == 0x02) {
				stateManual2 = pressed2;
				manual_activated2 = 0x01;
			}
			else {
				stateManual2 = startManual2;
			}
			break;
		case pressed2:
			if(B == 0x02) {
				if(light2 == 0x00) {
					PORTD = (PORTD ^ 0x02); 
				}
				else if(light2 == 0x02) {
					PORTD = (PORTD ^ 0x02); 
				}
			}
			stateManual2 = startManual2;
			break;
		default:
			stateManual2 = startManual2;
		break;
	}
}

//activates 5v step motor to turn feeding cup
void step_motor_feeder() {
	if(feed_time == hours && minutes == 00 && seconds == 5) {
		PORTD = PORTD ^ 0x18;
	}
	if(feed_time == hours && minutes == 00 && seconds == 15) {
		PORTD = PORTD ^ 0x18;
	}
}



int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x03;
	
	
	state2 = start;
	
	LCD_init();
	LCD_Cursor(1);
	TimerSet(1000);
	TimerOn();
	
	
	while(1)
	{
		A = PINA & 0x07;
		B = PINB & 0x03;
		light1 = PIND & 0x01;
		light2 = PIND & 0x02;

		day_night_light_manual();
		heat_light_manual();
		if (eco_selected == 0x00) {
			eco_select();
		}
		if (eco_selected == 0x01 && reptile_selected == 0x00)  {
			reptile_select();
		}
		if (eco_selected == 0x01 && reptile_selected == 0x01 && hours_set == 0x00){
			set_timeHours();
		}
 		if(hours_set == 0x01) {
 			set_timeMinutes();
 		}
 		if(time_set == 0x01) {
			clock_SM_tick();
			day_night_light_SM();
			heat_light_SM();
			step_motor_feeder();
 		}
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
