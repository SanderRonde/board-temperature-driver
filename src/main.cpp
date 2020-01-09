#include <Arduino.h>
#include <motor.h>
#include <temp.h>
#include <net.h>

void setup() {
	Serial.begin(115200);

	Motor::setup();
	Temp::setup();
	Net::setup();
}

void loop() {
	Temp::loop();
}