#include <ArduinoOTA.h>
#include <Arduino.h>
#include <telnet.h>
#include <config.h>
#include <motor.h>
#include <temp.h>
#include <ota.h>
#include <net.h>

void setup() {
	Serial.begin(115200);
	Serial.println("Booting");

	// Setup telnet
	Telnet::setup((String("temp-board-") + TEMPERATURE_NAME).c_str());

	// Setup OTA and wait
	OTA::setup();
	LOGN("Booted. Waiting for possible OTAs");
	OTA::wait_for_otas();
	LOGN("Stopped waiting");

	// Setup the rest
	#ifdef USE_MOTOR
	Motor::setup();
	#endif
	Temp::setup();
	Net::setup();

	// Done
	LOGN("Booted");
}

void loop() {
	OTA::loop();
	Temp::loop();
	Telnet::loop();
}
