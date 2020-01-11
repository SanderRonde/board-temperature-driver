#include <Arduino.h>
#include <telnet.h>

#define MOTOR_ON_PIN D6
#define MOTOR_RIGHT_PIN D7

#define FULL_MOVE_SECS 30

namespace Motor {
	typedef enum DIR {
		LEFT,
		RIGHT
	} dir_t;

	void setup() {
		pinMode(MOTOR_ON_PIN, OUTPUT);
		pinMode(MOTOR_RIGHT_PIN, OUTPUT);
	}

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
		LOGN("Moving left");
		start_motor(LEFT);
		delay(FULL_MOVE_SECS * 1000);
		stop_motor();
	}
	void move_right() {
		LOGN("Moving right");
		start_motor(RIGHT);
		delay(FULL_MOVE_SECS * 1000);
		stop_motor();
	}
}