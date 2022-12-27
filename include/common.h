#pragma once

#include <gba_types.h>

#define TEST_ARM 1
#define TEST_THUMB 2
#define VIEW_SIZE 16

#define REG_WAITCNT (*(vu16*) 0x4000204)

#define GRID_STRIDE 32
extern u16* textBase;
extern char textGrid[GRID_STRIDE * 32];

#define TEXT_LOC(Y, X) &textGrid[GRID_STRIDE * (Y) + (X)]

void updateTextGrid(void);
__attribute__((format(printf, 1, 2))) void savprintf(const char* fmt, ...);
__attribute__((format(printf, 1, 2))) int debugprintf(const char* fmt, ...);

extern struct ActiveInfo {
	const char magic[4];
	uint16_t subtestId;
	uint8_t testId;
	uint8_t suiteId;
} activeTestInfo;

static void markLine(int line, int mark) {
	int i;
	for (i = 0; i < 30; ++i) {
		u16 text = textBase[GRID_STRIDE * line + i];
		text &= 0xFFF;
		textBase[GRID_STRIDE * line + i] = text | (mark << 12);
	}
}

static inline void clearLine(int line) {
	markLine(line, 0);
}

static inline void clearAll(void) {
	int i;
	for (i = 0; i < 32; ++i) {
		clearLine(i);
	}
}

static inline void markLinePass(int line) {
	markLine(line, 0);
}

static inline void markLineFail(int line) {
	markLine(line, 1);
}
