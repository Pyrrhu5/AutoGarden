#include <Arduino.h>

const int PROBE_PIN = 15;

void setup() {
	pinMode(PROBE_PIN, INPUT);
	Serial.begin(115200);
}

void loop() {
	int moisture = analogRead(PROBE_PIN);
	Serial.println(moisture);
}

int main(void) {
    init();
    setup();
    while (true) {
       loop();
    }
}
