#pragma once

#include <gba_types.h>

#define TEST_ARM 1
#define TEST_THUMB 2
#define VIEW_SIZE 16

#define REG_WAITCNT (*(vu16*) 0x4000204)

#define GRID_STRIDE 32
extern char textGrid[GRID_STRIDE * 32];

void updateTextGrid(void);
__attribute__((format(printf, 1, 2))) void savprintf(const char* fmt, ...);
__attribute__((format(printf, 1, 2))) int debugprintf(const char* fmt, ...);

extern struct ActiveInfo {
	const char magic[4];
	uint16_t subtestId;
	uint8_t testId;
	uint8_t suiteId;
} activeTestInfo;