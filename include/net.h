#pragma once

#include <string.h>

namespace Net {
	String req(const char* host, int port, const char* path);

	void setup();
}