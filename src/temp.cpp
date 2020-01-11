#include <string.h>
#include <EEPROM.h>
#include <telnet.h>
#include <motor.h>
#include <math.h>
#include <DHT.h>
#include <net.h>

#define DHTPIN D5
#define DHTTYPE DHT22

#define UPDATE_INTERVAL_SECS 60

DHT dht(DHTPIN, DHTTYPE);

typedef struct data {
	bool on;
} data_t;

namespace Temp {
	char* last_state = NULL;
	data_t data = { 0 };

	void setup() {
		pinMode(DHTPIN, INPUT);
		dht.begin();

		EEPROM.begin(512);
		EEPROM.get(0, data);
	}

	String get_state(float temp) {
		char* path = (char*) malloc(sizeof(char) * 40);
		sprintf(path, "/temperature/advise/%.1f", (double) temp);
		String result = Net::req("***REMOVED***", 80, path);
		free(path);
		return result;
	}

	void apply_state(String state) {
		bool new_state = state == "on";
		data.on = new_state;

		EEPROM.put(0, data);
		EEPROM.commit();

		if (new_state) {
			Motor::move_right();
		} else {
			Motor::move_left();
		}
	}

	void update_state(float temp) {
		String state = get_state(temp);
		bool is_on = state == "on";
		if (state != "?") {
			if ((is_on && !data.on) || (!is_on && data.on)) {
				LOGF("Changing state to %s\n", state.c_str());
				apply_state(state);
			}
		}
	}

	int loop_counts = 0;
	unsigned int last_loop = 0;
	void loop() {
		if (millis() > last_loop + 1000) {
			last_loop = millis();

			float temp = dht.readTemperature();

			if (isnan(temp)) {
				return;
			}

			if (loop_counts == 0) {
				char* log = (char*) malloc(sizeof(char) * 100);
				sprintf(log, "Temp is %.1f\n", temp);
				LOG(log);
				free(log);

				update_state(temp);
			}

			loop_counts++;
			if (loop_counts >= UPDATE_INTERVAL_SECS) {
				loop_counts = 0;
			}
		}
	}
}