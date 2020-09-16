#include "Arduino.h"

/* ========================================================================== */
/*                              PINS DEFINITION                               */
/* ========================================================================== */
#define MOISTURE_SENSOR 0
#define LED 2


/* ========================================================================== */
/*                              GLOBAL VARIABLES                              */
/* ========================================================================== */
// value of resistance when soil is dry
/* #define VALUE_WATER_REQ 300  */
 #define VALUE_WATER_REQ 150

char printBuffer[128];
bool shouldLedBlink = false;

float nextTickLed;
float tickIntervalLed = 500;
float nextTickSensor;
// 							hours 			minutes 	 seconds
float tickIntervalSensor = (1 * 3600000) + (0 * 60000) + (0 * 1000);


void sensor_tick() {
	int val = analogRead(MOISTURE_SENSOR);

	if ( val <= VALUE_WATER_REQ ) {
		shouldLedBlink = true;
		nextTickSensor = millis() + ( tickIntervalLed * 4 );
	} else {
		shouldLedBlink = false;
		nextTickSensor = millis() + tickIntervalSensor;
	}

	sprintf(printBuffer,"Moisture level is %d\n", val);
	Serial.print(printBuffer);

}

void led_tick() {

	if ( shouldLedBlink ) {
		digitalWrite( LED, !digitalRead(LED) );
	} else {
		digitalWrite ( LED, LOW );
	}

	nextTickLed = millis() + tickIntervalLed;
}


void setup() {
	pinMode(MOISTURE_SENSOR, INPUT);
	pinMode(LED, OUTPUT);
	Serial.begin(115200);
	nextTickLed = millis();
	nextTickSensor = millis();
}

void loop() {
	if ( millis() >= nextTickSensor ) {
		sensor_tick();
	}
	if ( millis() >= nextTickLed ) {
		led_tick();
	}
}

int main(void) {
    init();
    setup();
    while (true) {
       loop();
		delay(500);
    }
}


