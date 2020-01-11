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

	String get_state(float temp) {
		char* path = (char*) malloc(sizeof(char) * 40);
		sprintf(path, "/temperature/advise/%.1f", (double) temp);
		String result = Net::req("***REMOVED***", 80, path);
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

	typedef struct xhr_result {
		String state;
		String mode;
		bool is_on;
	} xhr_result_t;

	xhr_result_t* parse_xhr_result(String result) {
		xhr_result_t* parsed = (xhr_result_t*) malloc(sizeof(xhr_result_t));
		
		char str_1[50];
		char str_2[50];
		int str_1_len = 0;
		int str_2_len = 0;

		bool saw_space = false;
		const char* c_str = result.c_str();
		for (int i = 0; i < result.length(); i++) {
			char cur_char = c_str[i];
			if (cur_char == ' ') {
				saw_space = true;
			} else if (saw_space) {
				str_2[str_2_len++] = cur_char;
			} else {
				str_1[str_1_len++] = cur_char;
			}
		}

		str_1[str_1_len++] = '\0';
		str_2[str_2_len++] = '\0';

		parsed->state = String(str_1);
		parsed->mode = String(str_2);
		parsed->is_on = parsed->state == "on";

		return parsed;
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
		xhr_result_t* parsed = parse_xhr_result(state);

		if (parsed->state != "?") {
			update_mode(parsed->is_on, parsed->mode != "auto");
		}
		free(parsed);
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

			if (millis() > cooldown_start + POST_SWITCH_WAIT_TIME * 1000) {
				// Cooldown over
				is_in_cooldown = false;
				update_mode(cooldown_state_on, false);
			}

			loop_counts++;
			if (loop_counts >= UPDATE_INTERVAL_SECS) {
				loop_counts = 0;
			}
		}
	}
}