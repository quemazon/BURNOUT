/*fail_safe.ino
This file is intended for the arduino pro-mini to monitor channel 3 from the radio and will toggle the muxer and communicate the channel 3 position to the main MCU board (i.e. teensy 3.1).

This code concept is based on the arduipilot fail safe developed by Chris Anderon, Jordi Munoz, Nathan (SparkFun).

Description and Functionality
1. if the switch is toggled from high to low and back 3 times in less than 1 second, a hard reset will be performed on the main MCU board.
2. the LED on the arduino will flash as follows, depending on the controlling state:
	manual = 0
	state 1 = quick flashing
	state 2 = slow flashing
	automatic = constant LED

3. a servo pulse range is 1000us ~ 2000us. the channel 3 switch positions should be configured as follows to ensure the proper state is detected and enabled:
	manual -> manual (set to ~1000us) <1250us
	state 1 -> 1250us <= state 1 (set to ~1375) < 1500us
	state 2 -> 1500us <= state 2 (set to ~1625) < 1750us
	automatic -> 1750us <= automatic (set to ~2000us)
*/

#define SWITCH_POSITION_MANUAL 0		//full manual control of the car
#define SWITCH_POSITION_1 1			//switch state 1
#define SWITCH_POSITION_2 2			//switch state 2
#define SWITCH_POSITION_AUTOMATIC 3	//full autonomous mode
#define TIME_TO_FLIP_SWITCH 1000000	//time in us to flip the switch 3 times
#define RESET_SWITCH_COUNTER 3	//number to reset the main MCU

int rest_pin = 1;		//used to reset the external MCU
int ch_3_in = 2;		//pin to monitor radio channel 3
int ch_3_state_1 = 3;		//output line 1 to external MCU
int ch_3_state_2 = 4;		//output line 2 to external MCU
int multiplexor = 5;	//multiplexor toggle
int state_led = 13;		//MCU state led
int pulse_length = 0;
int switch_position = SWITCH_POSITION_MANUAL;

void setup(){
	pinMode(rest_pin, OUTPUT);
	pinMode(ch_3_in, INPUT);
	pinMode(ch_3_state_1, OUTPUT);
	pinMode(ch_3_state_2, OUTPUT);
	pinMode(state_led, OUTPUT);
	pinMode(multiplexor, OUTPUT);

	digitalWrite(rest_pin, LOW);
    digitalWrite(multiplexor, LOW);

	set_vehile_state();
	flash_led();
}

void loop(){
	pulse_length = pulseIn(ch_3_in, HIGH, 50000);	//function will time out after 50000 uS if no pulse is received
	determine_switch_position();
	set_vehile_state();
	flash_led();
	check_if_reset_requested();
}

void check_if_reset_requested(){
	static int toggle_timer = 0;
	static int high_position_counter = 0;
	static int low_position_counter = 0;
	static int previous_pulse_length = 0;
	
	if(pulse_length >= 1500){	//checks to see if ch3 has just been toggled HIGH
		if(previous_pulse_length >= 1500) ;
		else{
			high_position_counter++;
			toggle_timer = millis();
		}
	}

	if(pulse_length <= 1250){	//checks to see if ch3 has just been toggled LOW
		if(previous_pulse_length <= 1250) ;
		else low_position_counter++;
	}
	
	if(toggle_timer >= 1000){
		high_position_counter = 0;
		low_position_counter = 0;
	}
	else if((high_position_counter > RESET_SWITCH_COUNTER) && (low_position_counter > RESET_SWITCH_COUNTER)){
		digitalWrite(rest_pin, LOW);
		delay(250);
		digitalWrite(rest_pin, HIGH);
	}
	
	return;
}

void determine_switch_position(){
	if(pulse_length < 1250) switch_position = SWITCH_POSITION_MANUAL;
	else if((pulse_length >= 1250) && (pulse_length < 1500)) switch_position = SWITCH_POSITION_1;
	else if((pulse_length >= 1500) && (pulse_length < 1750)) switch_position = SWITCH_POSITION_2;
	else switch_position = SWITCH_POSITION_AUTOMATIC;
	
	return;
}

void set_vehile_state(){
	switch(switch_position){
		case SWITCH_POSITION_1:
			digitalWrite(multiplexor, LOW);
			digitalWrite(ch_3_state_1, LOW);
			digitalWrite(ch_3_state_2, LOW);
			digitalWrite(multiplexor, HIGH);
			break;

		case SWITCH_POSITION_2:
			digitalWrite(multiplexor, LOW);
			digitalWrite(ch_3_state_1, LOW);
			digitalWrite(ch_3_state_2, HIGH);
			digitalWrite(multiplexor, HIGH);
			break;
		
		case SWITCH_POSITION_AUTOMATIC:
			digitalWrite(multiplexor, LOW);
			digitalWrite(ch_3_state_1, HIGH);
			digitalWrite(ch_3_state_2, LOW);
			digitalWrite(multiplexor, HIGH);
			break;
		
		default:	//default state is SWITCH_POSITION_MANUAL
			digitalWrite(multiplexor, LOW);
	}
	
	return;
}

void flash_led(){
	static int led_time_old = 0;
	
	switch(switch_position){
		case SWITCH_POSITION_1:
			if((millis() - led_time_old) > 250){
				digitalWrite(state_led, !digitalRead(state_led));
				led_time_old = millis();
			}
			break;

		case SWITCH_POSITION_2:
			if((millis() - led_time_old) > 1000){
				digitalWrite(state_led, !digitalRead(state_led));
				led_time_old = millis();
			}
			break;

		case SWITCH_POSITION_AUTOMATIC:
			digitalWrite(state_led, HIGH);
			break;

		default:
			digitalWrite(state_led, LOW);
			break;
	}
	
	return;
}