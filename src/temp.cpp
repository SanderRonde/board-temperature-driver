#include <secrets.h>
#include <string.h>
#include <EEPROM.h>
#include <config.h>
#include <telnet.h>
#include <motor.h>
#include <util.h>
#include <math.h>
#include <DHT.h>
#include <net.h>

#define DHTPIN D5
#define DHTTYPE DHT22

#define UPDATE_INTERVAL_SECS 60

// Don't keep switching back and forth
#define POST_SWITCH_WAIT_TIME 300

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

	#ifndef USE_MOTOR
	void report_state(float temp) {
		char* path = (char*) malloc(sizeof(char) * 40);
		sprintf(path, "/temperature/report/%.1f", (double) temp);
		Net::req(BASE_NAME, 80, path);
		free(path);
	}
	
	#else

	String get_state(float temp) {
		char* path = (char*) malloc(sizeof(char) * 40);
		sprintf(path, "/temperature/advise/%.1f", (double) temp);
		String result = Net::req(BASE_NAME, 80, path);
		free(path);
		return result;
	}

	void apply_state(bool is_on) {
		data.on = is_on;

		EEPROM.put(0, data);
		EEPROM.commit();

		if (is_on) {
			Motor::move_right();
		} else {
			Motor::move_left();
		}
	}

	bool is_in_cooldown = false;
	unsigned int cooldown_start = 0;
	bool cooldown_state_on = false;
	void update_mode(bool is_on, bool force) {
		if ((is_on && !data.on) || (!is_on && data.on)) {
			if (is_in_cooldown && !force) {
				// Don't switch, just store this and
				// switch post-cooldown
				cooldown_state_on = is_on;
			} else {
				LOGF("Changing state to %s\n", is_on ? "on" : "off");
				apply_state(is_on);
				
				is_in_cooldown = true;
				cooldown_start = millis();
				cooldown_state_on = is_on;
			}
		}
	}

	void update_state(float temp) {
		String state = get_state(temp);
		char** strings = Util::split_string(state);
		bool is_on = strcmp(strings[0], "on") == 0;

		if (strcmp(strings[0], "?") != 0) {
			update_mode(is_on, strcmp(strings[1], "auto") != 0);
		}

		Util::free_split(strings);
	}
	#endif

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
				LOGF("Temp is %.1f\n", temp);
				char* log = (char*) malloc(sizeof(char) * 100);
				sprintf(log, "Temp is %.1f\n", temp);
				LOG(log);
				free(log);

				#ifdef USE_MOTOR
				update_state(temp);
				#else
				report_state(temp);
				#endif
			}

			#ifdef USE_MOTOR
			if (millis() > cooldown_start + POST_SWITCH_WAIT_TIME * 1000) {
				// Cooldown over
				is_in_cooldown = false;
				update_mode(cooldown_state_on, false);
			}
			#endif

			loop_counts++;
			if (loop_counts >= UPDATE_INTERVAL_SECS) {
				loop_counts = 0;
			}
		}
	}
}