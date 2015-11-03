#pragma once

#include <stddef.h>

struct TestSuite {
	const char* name;
	void (*run)(void);
	size_t (*list)(const char** names, size_t size, size_t offset);
	void (*show)(size_t index);
	const size_t nTests;
	const unsigned* passes;
	const unsigned* totalResults;
};
