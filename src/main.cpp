#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
	Serial.begin(115200);
	pinMode(5, INPUT);
	dht.begin();
	Serial.write("Hello World\n");
}

void loop() {
	float temp = dht.readTemperature();  
	Serial.print("Temp: ");
	Serial.println(temp);
	delay(1000);
}