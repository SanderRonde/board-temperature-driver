#include <ArduinoOTA.h>
#include <Arduino.h>
#include <telnet.h>
#include <ota.h>
#include <motor.h>
#include <temp.h>
#include <net.h>

void setup() {
	Serial.begin(115200);
	Serial.println("Booting");

	// Setup telnet
	Telnet::setup("temp-board");

	// Setup OTA and wait
	OTA::setup();
	LOGN("Booted. Waiting for possible OTAs");
	OTA::wait_for_otas();
	LOGN("Stopped waiting");

	// Setup the rest
	Motor::setup();
	Temp::setup();
	Net::setup();

	// Done
	LOGN("Booted");
}

void loop() {
	OTA::loop();
	Temp::loop();
}
