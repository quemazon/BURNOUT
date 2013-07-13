#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

const int far1 = 1;    //10-80 cm sensor
const int far2 = 0;    //10-80 cm sensor

#define LED 13
 
// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  300 // microseconds
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     300 // 0 is stopped, 400 is full speed
#define TURN_SPEED        300
#define FORWARD_SPEED     300
#define FAST_SPEED     	  400 // 0 is stopped, 400 is full speed
#define MEDIUM_SPEED      300
#define SLOW_SPEED     	  200
#define LINE_DETECTED		1
#define SEARCH_ENEMY		2
#define ENEMY_LONG	        3
#define STOP_ROBOT	        4
#define SENSOR_FR		1
#define SENSOR_FL		2
#define SENSOR_RL		4
#define SENSOR_RR		8
//#define REVERSE_DURATION  200 // ms
//#define TURN_DURATION     200 // ms
 
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

// Numbers of sensors and pins
#define NUM_SENSORS 3 
unsigned int sensor_values[NUM_SENSORS];
byte state, sensors_detected = 0;
int val[4];
byte pins[] = {4, 5, 11, 6};
ZumoReflectanceSensorArray sensors(pins, 4, 700, QTR_NO_EMITTER_PIN);
 

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
   
  // play audible countdown
  for (int i = 0; i < 4; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);  
 // delay(1000);
}

// byte stopRobot()
// {
	    // motors.setSpeeds(0, 0);
		// button.waitForRelease();
		// waitForButtonAndCountDown();
		// state = SEARCH_ENEMY;
	
// }
byte lineDetected()
{
	if(sensors_detected & SENSOR_RL) {
		readReflectorValues();
		motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
		buzzer.playNote(NOTE_G(4), 500, 15);
		while(readLineSensors());
		delay(100);
	}
	
	else {
	    readReflectorValues();
		motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
		buzzer.playNote(NOTE_G(3), 200, 15);
		while(readLineSensors());
		delay(100);
	}
    return(SEARCH_ENEMY);
}

byte lineDetected2()
{
	Serial.println(sensors_detected, BIN);
	switch (sensors_detected) {
		case (SENSOR_FL):
			motors.setSpeeds(-MEDIUM_SPEED, -SLOW_SPEED);
			Serial.println("Front Left");
		break;
		
		case (SENSOR_FR):
			motors.setSpeeds(-SLOW_SPEED, -MEDIUM_SPEED);
			Serial.println("Front Right");
		break;
		  
		case (SENSOR_FL+SENSOR_FR):
			motors.setSpeeds(-MEDIUM_SPEED, -MEDIUM_SPEED);
			Serial.println("Front Both");
		break;

		case (SENSOR_RL):
			motors.setSpeeds(MEDIUM_SPEED, SLOW_SPEED);
			Serial.println("Rear Left");
		break;

		case (SENSOR_RR):
			motors.setSpeeds(SLOW_SPEED, MEDIUM_SPEED);
			Serial.println("Rear Right");
		break;

		case (SENSOR_RL+SENSOR_RR):
			motors.setSpeeds(MEDIUM_SPEED, MEDIUM_SPEED);
			Serial.println("Rear Both");
		break;
		
		default: 
			Serial.println("None");
		break;
		}
	delay(300);
	return(SEARCH_ENEMY);
}

byte readLineSensors()
{
	byte edge_sensors=0;
	sensors.read(sensor_values);
    if (sensor_values[0] < QTR_THRESHOLD) edge_sensors += SENSOR_FL;
    if (sensor_values[1] < QTR_THRESHOLD) edge_sensors += SENSOR_FR;
    if (sensor_values[2] < QTR_THRESHOLD) edge_sensors += SENSOR_RL;
    if (sensor_values[3] < QTR_THRESHOLD) edge_sensors += SENSOR_RR;
	//Serial.println(edge_sensors, BIN);
	return (edge_sensors);
}

byte enemyLong ()
{
	
	while(true) {
	readReflectorValues();
		if (val[0] > 90 && val[1]<90)    // long range turn right
			{
				motors.setSpeeds(TURN_SPEED, TURN_SPEED*4); //turn right slow
				sensors_detected = readLineSensors();
				if(sensors_detected > 0) return(LINE_DETECTED);
			//delay(50);
			}
		else if (val[0] < 90 && val[1]>90)    // long range turn right
			{
				motors.setSpeeds(TURN_SPEED*.2, TURN_SPEED); //turn left slow
				sensors_detected = readLineSensors();
				if(sensors_detected > 0) return(LINE_DETECTED);
				//delay(50);
			}	
		if (val[0] > 390 && val[1]<90)    // long range turn right
			{
				motors.setSpeeds(TURN_SPEED, -TURN_SPEED*5); //turn right slow
				sensors_detected = readLineSensors();
				if(sensors_detected > 0) return(LINE_DETECTED);
			//delay(50);
			}
		else if (val[0] < 90 && val[1]>300)    // long range turn right
			{
				motors.setSpeeds(-TURN_SPEED*.5, TURN_SPEED); //turn left slow
				sensors_detected = readLineSensors();
				if(sensors_detected > 0) return(LINE_DETECTED);
				//delay(50);
			}	
		
		// if (val[0] > 150 && val[1]<160)
			// {
				// motors.setSpeeds(TURN_SPEED, -TURN_SPEED*.3); //short range turn right medium
				// sensors_detected = readLineSensors();
				// if(sensors_detected > 0) return(LINE_DETECTED);//    delay(50);
			// }
	
		// else 
		
		// if (val[0] <150 && val[1]>160)
			// {
				// motors.setSpeeds(-TURN_SPEED*.3, TURN_SPEED); //short range turn right medium
			    // sensors_detected = readLineSensors();
				// if(sensors_detected > 0) return(LINE_DETECTED);// delay(50);
			// }
	
		// else

 
		//else

		else if (val[0] >200 && val[1]>200)
		{
			motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); //attack
		    sensors_detected = readLineSensors();
		    if(sensors_detected > 0) return(LINE_DETECTED);
		}
		
		// if (button.isPressed())
		// {
		// // if button is pressed, stop and wait for another press to go again
			// return(STOP_ROBOT);
		// }   
		
		else return(SEARCH_ENEMY);

	}
}

byte searchEnemy()
{
	while(true) {
		readReflectorValues();
		if (val[0] < 90 || val[1]<90) //search spin turn right
			{
				motors.setSpeeds(TURN_SPEED*.7, -TURN_SPEED*.7);
				sensors_detected = readLineSensors();
				if(sensors_detected > 0) return(LINE_DETECTED);
			}
		else {
		//motors.setSpeeds(0, 0);
		return(ENEMY_LONG);
		}
	}
}

void readReflectorValues() {	// Read the sharp distance sensors
  val[0] = analogRead(far1);    // the far sensors are analog
  val[1] = analogRead(far2);
 // //Serial.print(val[0]);
  // //Serial.print("\t");      // this prints a tab
  // Serial.print(state);
  // Serial.print("\t");
  // Serial.print(val[0]);
  // Serial.print("\t");
  // Serial.println(val[1]);  //serial.println make a new line
}
 
void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
   Serial.begin(115200);
  state = SEARCH_ENEMY; 
  pinMode(LED, HIGH);
   
  waitForButtonAndCountDown();
}

void loop()
{
//  readReflectorValues();
//  sensors.read(sensor_values);


switch (state) {
    case LINE_DETECTED:
		state = lineDetected2();
 //do something when var equals 1
      break;
    case SEARCH_ENEMY:
	state = searchEnemy();
      break;
	case ENEMY_LONG:
	state = enemyLong();
      break;
	
//	case STOP_ROBOT:
//	state = stopRobot();
      //do something when var equals 2
//      break;
    //default: 
      // if nothing else matches, do the default
      // default is optional
  }

}
