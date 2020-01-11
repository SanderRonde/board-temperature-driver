#include <Arduino.h>
#include <string.h>

namespace Util {
	const int SPLIT_LEN = 10;
	char** split_string(String str) {
		char** strings = (char**) malloc(sizeof(char*) * SPLIT_LEN);
		for (int i = 0; i < SPLIT_LEN; i++) {
			strings[i] = NULL;
		}

		int str_index = 0;
		const char* delim = { " " };
		char* copy = (char*) malloc(sizeof(char) * str.length());
		strcpy(copy, str.c_str());
		char* ptr = strtok(copy, delim);
		while (ptr != NULL) {
			strings[str_index++] = strdup(ptr);
			ptr = strtok(NULL, delim);
		}
		free(copy);

		return strings;
	}

	void free_split(char** strings) {
		for (int i = 0; i < SPLIT_LEN; i++) {
			if (strings[i] != NULL) {
				free(strings[i]);
			}
		}
		free(strings);
	}
}