#include <Arduino.h>

#define MOTOR_ON_PIN 6
#define MOTOR_RIGHT_PIN 7

// TODO: change/measure
#define FULL_MOVE_SECS 20
#define MAX_PROG 1000
#define PROG_PER_SEC MAX_PROG / FULL_MOVE_SECS

namespace Motor {
	int progress = 0;

	typedef enum DIR {
		LEFT,
		RIGHT
	} dir_t;

	void start_motor(dir_t dir) {
		if (dir == LEFT) {
			digitalWrite(MOTOR_RIGHT_PIN, LOW);
		} else {
			digitalWrite(MOTOR_RIGHT_PIN, HIGH);
		}

		digitalWrite(MOTOR_ON_PIN, HIGH);
	}

	void stop_motor() {
		digitalWrite(MOTOR_ON_PIN, LOW);
		digitalWrite(MOTOR_RIGHT_PIN, LOW);
	}

	void move_left() {
		start_motor(LEFT);
		delay((progress * 1000) / PROG_PER_SEC);
		progress = 0;
		stop_motor();
	}
	void move_right() {
		start_motor(RIGHT);
		delay(((MAX_PROG - progress) * 1000) / PROG_PER_SEC);
		progress = MAX_PROG;
		stop_motor();
	}

	void setup() {
		pinMode(MOTOR_ON_PIN, OUTPUT);
		pinMode(MOTOR_RIGHT_PIN, OUTPUT);
	}
}