#include <string.h>
#include <motor.h>
#include <EEPROM.h>
#include <DHT.h>
#include <net.h>

#define DHTPIN 5
#define DHTTYPE DHT22

#define UPDATE_INTERVAL_SECS 5

DHT dht(DHTPIN, DHTTYPE);

namespace Temp {
	char* last_state = NULL;

	void setup() {
		pinMode(5, INPUT);
		dht.begin();

		EEPROM.begin(512);
		EEPROM.get(0, last_state);
		if (strlen(last_state) == 0) {
			strcpy(last_state, "off");
		}
	}

	String get_state(float temp) {
		char* path = (char*) malloc(sizeof(char) * 40);
		sprintf(path, "/temperature/advise/%.1f", (double) temp);
		String result = Net::req("***REMOVED***", 80, path);
		free(path);
		return result;
	}

	void apply_state(String state) {
		strcpy(last_state, state.c_str());
		EEPROM.put(0, last_state);
		EEPROM.commit();

		if (strcmp(last_state, "on") == 0) {
			Motor::move_right();
		} else {
			Motor::move_left();
		}
	}

	void update_state(float temp) {
		String state = get_state(temp);
		if (state != "?" && strcmp(state.c_str(), last_state) != 0) {
			apply_state(state);
		}
	}

	int loop_counts = 0;
	void loop() {
		float temp = dht.readTemperature();
		delay(1000);

		if (loop_counts == 0) {
			update_state(temp);
		}

		loop_counts++;
		if (loop_counts >= UPDATE_INTERVAL_SECS) {
			loop_counts = 0;
		}
	}
}