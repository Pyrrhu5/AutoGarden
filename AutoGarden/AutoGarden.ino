/*
 *  ===============================================================================
 *                            PLANT WATER LEVEL MONITORING
 *  ===============================================================================
 *
 * Blink a LED when the soil is not moister enough using two metal rodes
 * planted into the soil (as moister sensors have a lifespan of a cake in my flat).
 *
 * Meant to run on batteries, with the Arduino's power LED removed, 
 * it consumes around 1,35 mA.
 *
 * COMPONANTS
 * Schematics totally ripped off from this article: 
 * http://gardenbot.org/howTo/soilMoisture/#The_local_circuit_-_simple_voltage
 *
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
 * One resistor: 1 kOhms (lower it if the LED is not bright enough)
 * 		- R2
 * One LED
 * 		- L
 * Arduino Nano
 *
 * WIRING
 * Pin | Componant
 * A0 to S0 
 * D2 to R0 to S1
 * D3 to R1 to S0
 * D4 to R2 to L+
 * -     L- to GND
 * 
 * If everything is well wired, the moisture level should be 0
 * when the rodes are not in contact with anything, and close to 1023 when in water.
 */

#include "limits.h"
#include "LowPower.h"

/* ========================================================================== */
/*                              PINS DEFINITION                               */
/* ========================================================================== */
#define SENSOR_PIN 		A0 
#define VOLTAGE_PIN_1 	2
#define VOLTAGE_PIN_2 	3
#define LED_PIN 		4

/* ========================================================================== */
/*                                  SETTINGS                                  */
/* ========================================================================== */
// LED's Blink duration
const unsigned int LED_DELAY = 500;
// Duration between measure when soil has been detected moist enough
// 						 			hours 		 	  minutes 	     seconds
const unsigned long SENSOR_DELAY = (1 * 3600000UL) + (0 * 60000UL) + (0 * 1000UL);
// Level measure when the soil is considered too dry
// lower is dryer
const unsigned int WATER_REQUIRED_LEVEL = 950;

/* ========================================================================== */
/*                              GLOBAL VARIABLES                              */
/* ========================================================================== */
// Last time in ms for each tick
// initialized at max value to force the first tick
unsigned long lastTickLed = ULONG_MAX;
unsigned long lastTickSensor = ULONG_MAX;
// status of the LED
bool  shouldLedBlink= false;
// Correction of millis() in ms due to deep sleep
unsigned long timeDelta = 0;


/* ========================================================================== */
/*                                    UTILS                                   */
/* ========================================================================== */

/* Converts ms to humain-readable time */
void pretty_time(long int duration, char* buff) {
	long millis = duration % 1000;
	long second = (duration / 1000) % 60;
	long minute = (duration / (1000 * 60UL)) % 60;
	long hour = (duration / (1000 * 60UL * 60UL)) % 24;
	char nice[13];	
	sprintf(nice, "%02ld:%02ld:%02ld.%03ld", hour, minute, second, millis);
	strncpy(buff, nice, sizeof(nice));
}

/* Inacurrate way of keeping track of millis() after some deep sleep */
unsigned long now() {
	return millis() + timeDelta;
}

/* Deep sleep per around 500 ms cycles */
void sleep(unsigned long durationMs) {
	for (int i=durationMs/500; i>0; i--) {
		LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
		// With a (stupid) correctif ratio for time due to wake up inaccuracy
		timeDelta += 500 * 1.058594;
	}
}

/* ========================================================================== */
/*                                   SENSORS                                  */
/* ========================================================================== */

/* Mesure soil moistures */
unsigned int measure_moisture() {
	// Takes 8 measures 
	int recs = 0;
	for (int i=0; i<8; i++){
		// Flip the sensor polarity at each measurement to avoid electrolis
		int polarity = !digitalRead(VOLTAGE_PIN_1);
		digitalWrite(VOLTAGE_PIN_1, polarity);
		digitalWrite(VOLTAGE_PIN_2, !polarity);
		delay(5);
		
		// When the polarity is inverted, the measurement goes from 0 to -1023
		// Reverse the measurement in this case
		if (!polarity) {
			recs += 1023 - analogRead(SENSOR_PIN);
		} else {
			recs += analogRead(SENSOR_PIN);
		}
		delay(10);
	}

	// turn off the sensor
	digitalWrite(VOLTAGE_PIN_1, LOW);
	digitalWrite(VOLTAGE_PIN_2, LOW);

	// Returns the average => divided by 8
	return recs >> 3;
}


/* ========================================================================== */
/*                                 DUTY CYCLES                                */
/* ========================================================================== */

/* 
	Duty cycle for the moisture sensor
	Measure the moisture in the soil and
	set the LED accordingly
*/
void sensor_tick() {
	unsigned int moistureLevel = measure_moisture();
	shouldLedBlink = moistureLevel <= WATER_REQUIRED_LEVEL;

	lastTickSensor = now();

	// Output to serial
	char nice[13];
	pretty_time(now(), nice);
	char printBuffer[39];
	sprintf(printBuffer,"%s - Moisture level is %d\n", nice, moistureLevel);
	Serial.print(printBuffer);
	Serial.flush();
}

/* Duty cycle for the LED */
void led_tick() {
	if ( shouldLedBlink ) {
		digitalWrite( LED_PIN, !digitalRead(LED_PIN) );
	} else {
		digitalWrite ( LED_PIN, LOW );
	}
	lastTickLed = now();
}


/* ========================================================================== */
/*                                   MAINS                                    */
/* ========================================================================== */

void setup() {
	// Shut off all unused pins for power saving
	for (int i = 0; i < 20; i++) {
		// except for RX pin
		if (i == 1) continue;
		pinMode(i, OUTPUT);
	}

	Serial.begin(9600);
	// Resetting the OUPUT pins is totally useless, but explicit ;-)
	pinMode(VOLTAGE_PIN_1, OUTPUT);
	pinMode(VOLTAGE_PIN_2, OUTPUT);
	pinMode(SENSOR_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
}


void loop() {

	// Perform the duty cycles
	if ( shouldLedBlink && (unsigned long)(now() - lastTickLed) >= LED_DELAY ) {

		sensor_tick();
		led_tick();

	} else if ( (unsigned long) (now() - lastTickSensor) >= SENSOR_DELAY ) {

		sensor_tick();

	}
	
	// Set deep sleep
	if ( shouldLedBlink ) {
		sleep(LED_DELAY);
	} else {
		sleep(SENSOR_DELAY);
	}
	
}
