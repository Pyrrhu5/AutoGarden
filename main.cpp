/*
 *  ===============================================================================
 *                            PLANT WATER LEVEL MONITORING
 *  ===============================================================================
 *
 * Blink a LED when the soil is not moister enough using two metal rodes
 * planted into the soil (as moister sensors have a lifespan of a cake in my flat).
 *
 * COMPONANTS
 * Two metal rodes (like screws, nails, the more rusty possible if you are antivax) 
 * consistently spaced by some non-conductive material
 * (like packaging foam) and partially isolated up to the depth of the soil to measure
 * (up to the point where to measure moisture, not the surface):
 * 		- S0
 * 		- S1
 * One resistor : between 47k to 100k Ohms (depends on observed measures)
 * 		- R0
 * One resistor: 100 Ohms
 * 		- R1
 * Arduino Nano
 *
 * WIRING
 * A0 to R0 and S0 (in serie)
 * R0 to D6
 * R1 to R1
 * R1 to D7
 * 
 * If everything is well wired, the moisture level should be 1023
 * when the rodes are not in contact with anything.
 */

#include "Arduino.h"
#include "math.h"

/* ========================================================================== */
/*                              PINS DEFINITION                               */
/* ========================================================================== */
#define SENSOR_PIN 		14 // A0
#define VOLTAGE_PIN_1 	6 // D6
#define VOLTAGE_PIN_2 	7 // D7
#define LED_PIN 		2 // D2

/* ========================================================================== */
/*                                  SETTINGS                                  */
/* ========================================================================== */
// Delay to let the arduino switch the poles
// and take a measure
const int FLIP_DELAY = 10;
// LED's Blink duration
const int LED_DELAY = 500;
// Duration between measure when soil has been detected moist enough
// 						  hours 		 minutes 	    seconds
const long SENSOR_DELAY = (4 * 3600000UL) + (0 * 60000UL) + (0 * 1000UL);
// Level measure when the soil is considered too dry
// higher is dryer
const int WATER_REQUIRED_LEVEL = 150;

/* ========================================================================== */
/*                              GLOBAL VARIABLES                              */
/* ========================================================================== */
float nextTickLed = millis();
float nextTickSensor = millis();
bool  shouldLedBlink= false;
// Time to sleep between loops
int DELAY = 0;


void pretty_time(long int duration, char* buff) {
	/* 
 	 * Convert Arduino's time in milliseconds
 	 * to human readable values
 	 */

	long millis = duration % 1000;
	long second = (duration / 1000) % 60;
	long minute = (duration / (1000 * 60UL)) % 60;
	long hour = (duration / (1000 * 60UL * 60UL)) % 24;
	char nice[13];	
	sprintf(nice, "%02ld:%02ld:%02ld.%03ld", hour, minute, second, millis);
	strncpy(buff, nice, sizeof(nice));

	
}

void set_sensor_polarity(bool flip) {
	/* 
	 * To prevent electrolysis, the polarity
 	 * of the rodes are flipped for each measure
 	 */

	digitalWrite(VOLTAGE_PIN_1, flip);
	digitalWrite(VOLTAGE_PIN_2, !flip);

}


void disable_sensor() {

	digitalWrite(VOLTAGE_PIN_1, LOW);
	digitalWrite(VOLTAGE_PIN_2, LOW);

}

int measure_moisture() {
	/*
	 * Take two measurements from the sensor
	 * and average them
	 */

	// take the first recording
	set_sensor_polarity(true);
	delay(FLIP_DELAY);
	int record1 = analogRead(SENSOR_PIN);
	// take the second recording
	delay(FLIP_DELAY);
	set_sensor_polarity(false);
	delay(FLIP_DELAY);
	int record2 = 1023 - analogRead(SENSOR_PIN);

	disable_sensor();

	// average the two records
	return ((record1 + record2) / 2);

}


void sensor_tick() {
	int val = measure_moisture();
	long now = millis();

	if ( val >= WATER_REQUIRED_LEVEL ) {
		shouldLedBlink = true;
		nextTickSensor = now + ( LED_DELAY * 4 );
	} else {
		shouldLedBlink = false;
		nextTickSensor = now + SENSOR_DELAY;
	}

	char nice[13];
	pretty_time(now, nice);
	char printBuffer[39];
	sprintf(printBuffer,"%s - Moisture level is %d\n", nice, val);
	Serial.print(printBuffer);

}

void led_tick() {

	if ( shouldLedBlink ) {
		digitalWrite( LED_PIN, !digitalRead(LED_PIN) );
		nextTickLed = millis() + LED_DELAY;
	} else {
		digitalWrite ( LED_PIN, LOW );
		nextTickLed = millis() + SENSOR_DELAY;
	}

}


void setup() {
	Serial.begin(115200);
	pinMode(VOLTAGE_PIN_1, OUTPUT);
	pinMode(VOLTAGE_PIN_2, OUTPUT);
	pinMode(SENSOR_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
}


void loop() {
	float now = millis();
	
	if ( now >= nextTickSensor ) {
		sensor_tick();
	}
	if ( now >= nextTickLed ) {
		led_tick();
	}
	
	// sleep until the closest next tick
	delay(fmin(nextTickSensor, nextTickLed) - now);
	
}


int main(void) {
	init();
	setup();
	while (true) {
		loop();
	}
}

