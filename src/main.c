#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "font.h"

struct TestTimings {
	s32 arm_text;
	s32 arm_ewram;
	s32 arm_iwram;
	s32 thumb_text;
	s32 thumb_ewram;
	s32 thumb_iwram;
};

void calibrate(struct TestTimings*);
void testNop(struct TestTimings*);
void testLdrh(struct TestTimings*);
void testStrh(struct TestTimings*);
void testLdmia1(struct TestTimings*);
void testLdmia2(struct TestTimings*);
void testLdmia6(struct TestTimings*);
void testStmia1(struct TestTimings*);
void testStmia2(struct TestTimings*);
void testStmia6(struct TestTimings*);
void testMul0(struct TestTimings*);
void testMul1(struct TestTimings*);
void testMul2(struct TestTimings*);
void testMul3(struct TestTimings*);
void testMul4(struct TestTimings*);
void testDiv(struct TestTimings*);
void testCpuSet(struct TestTimings*);

#define TEST_ARM 1
#define TEST_THUMB 2
#define VIEW_SIZE 16

struct TimingTest {
	const char* testName;
	void (*test)(struct TestTimings*);
	int modes;
	struct TestTimings expected;
} const tests[] = {
	{ "Calibration", 0, TEST_ARM | TEST_THUMB, { 7, 5, 0, 4, 2, 0 } },
	{ "nop", testNop, TEST_ARM | TEST_THUMB, { 6, 6, 1, 3, 3, 1 } },
	{ "ldrh r2, [sp]", testLdrh, TEST_ARM | TEST_THUMB, { 10, 8, 3, 7, 5, 3} },
	{ "strh r3, [sp]", testStrh, TEST_ARM | TEST_THUMB, { 9, 7, 2, 6, 4, 2} },
	{ "ldmia sp, {r2}", testLdmia1, TEST_ARM, { 10, 8, 3 } },
	{ "ldmia sp, {r2, r3}", testLdmia2, TEST_ARM, { 11, 9, 4 } },
	{ "ldmia sp, {r2-r7}", testLdmia6, TEST_ARM, { 24, 20, 10 } },
	{ "stmia sp, {r2}", testStmia1, TEST_ARM, { 9, 7, 2 } },
	{ "stmia sp, {r2, r3}", testStmia2, TEST_ARM, { 10, 8, 3 } },
	{ "stmia sp, {r2-r7}", testStmia6, TEST_ARM, { 27, 23, 13 } },
	{ "mul #0x00000000, #0xFF", testMul0, TEST_ARM | TEST_THUMB, { 9, 7, 2, 6, 4, 2 } },
	{ "mul #0x00000078, #0xFF", testMul1, TEST_ARM | TEST_THUMB, { 9, 7, 2, 6, 4, 2 } },
	{ "mul #0x00005678, #0xFF", testMul2, TEST_ARM | TEST_THUMB, { 10, 8, 3, 7, 5, 3 } },
	{ "mul #0x00345678, #0xFF", testMul3, TEST_ARM | TEST_THUMB, { 11, 9, 4, 8, 6, 4 } },
	{ "mul #0x12345678, #0xFF", testMul4, TEST_ARM | TEST_THUMB, { 12, 10, 5, 9, 7, 5 } },
	{ "Division", testDiv, TEST_ARM | TEST_THUMB, { 409, 399, 342, 379, 369, 342 } },
	{ "CpuSet", testCpuSet, TEST_ARM | TEST_THUMB, { 3465, 3459, 3402, 3468, 3450, 3409 } },
};

u16* textBase = (u16*) VRAM;
char textGrid[32 * 32];

#define REG_WAITCNT (*(vu16*) 0x4000204)

EWRAM_DATA const int _anchor = 0xABAD1DEA; // There seems to be a bug in the ld script that this fixes

static void updateTextGrid(void) {
	int i;
	for (i = 0; i < 20 * 32; ++i) {
		textBase[i] = textGrid[i] ? textGrid[i] - ' ' : 0;
	}
}

static void printResults(const char* preface, const struct TestTimings* values, const struct TestTimings* calibration, const struct TestTimings* expected, int mode) {
	snprintf(&textGrid[32], 30, "Timing test: %s", preface);
	snprintf(&textGrid[96], 30, "ARM/ROM:     %5u", values->arm_text - calibration->arm_text);
	if (values->arm_text - calibration->arm_text == expected->arm_text) {
		strncpy(&textGrid[96 + 19], "PASS", 11);
	} else {
		snprintf(&textGrid[96 + 19], 11, "!= %5u", expected->arm_text);
	}

	snprintf(&textGrid[128], 30, "ARM/WRAM:    %5u", values->arm_ewram - calibration->arm_ewram);
	if (values->arm_ewram - calibration->arm_ewram == expected->arm_ewram) {
		strncpy(&textGrid[128 + 19], "PASS", 11);
	} else {
		snprintf(&textGrid[128 + 19], 11, "!= %5u", expected->arm_ewram);
	}

	snprintf(&textGrid[160], 30, "ARM/IWRAM:   %5u", values->arm_iwram - calibration->arm_iwram);
	if (values->arm_iwram - calibration->arm_iwram == expected->arm_iwram) {
		strncpy(&textGrid[160 + 19], "PASS", 11);
	} else {
		snprintf(&textGrid[160 + 19], 11, "!= %5u", expected->arm_iwram);
	}

	if (mode & TEST_THUMB) {
		snprintf(&textGrid[192], 30, "Thumb/ROM:   %5u", values->thumb_text - calibration->thumb_text);
		if (values->thumb_text - calibration->thumb_text == expected->thumb_text) {
			strncpy(&textGrid[192 + 19], "PASS", 11);
		} else {
			snprintf(&textGrid[192 + 19], 11, "!= %5u", expected->thumb_text);
		}

		snprintf(&textGrid[224], 30, "Thumb/WRAM:  %5u", values->thumb_ewram - calibration->thumb_ewram);
		if (values->thumb_ewram - calibration->thumb_ewram == expected->thumb_ewram) {
			strncpy(&textGrid[224 + 19], "PASS", 11);
		} else {
			snprintf(&textGrid[224 + 19], 11, "!= %5u", expected->thumb_ewram);
		}

		snprintf(&textGrid[256], 30, "Thumb/IWRAM: %5u", values->thumb_iwram - calibration->thumb_iwram);
		if (values->thumb_iwram - calibration->thumb_iwram == expected->thumb_iwram) {
			strncpy(&textGrid[256 + 19], "PASS", 11);
		} else {
			snprintf(&textGrid[256 + 19], 11, "!= %5u", expected->thumb_iwram);
		}
	} else {
		strncpy(&textGrid[192], "Thumb not applicable", 30);		
	}
}

int main(void) {
	irqInit();

	BG_PALETTE[0] = 0x7FFF;
	DMA3COPY(fontTiles, TILE_BASE_ADR(1), DMA16 | DMA_IMMEDIATE | (fontTilesLen >> 1));
	REG_BG1CNT = CHAR_BASE(1) | SCREEN_BASE(0);
	REG_BG1VOFS = -4;
	memset(textGrid, 0, sizeof(textGrid));
	strcpy(&textGrid[2], "Game Boy Advance Test Suite");

	updateTextGrid();
	REG_DISPCNT = MODE_0 | BG1_ON;

	irqEnable(IRQ_VBLANK);

	REG_WAITCNT &= ~0x4000;

	const struct TimingTest* activeTest = 0;
	int testIndex = 0;
	int viewIndex = 0;

	while (1) {
		memset(&textGrid[32], 0, sizeof(textGrid) - 32);
		struct TestTimings calibration;
		calibrate(&calibration);

		scanKeys();
		u16 keys = keysDown();

		if (keys & KEY_B) {
			activeTest = 0;
		}
		if (keys & KEY_A) {
			activeTest = &tests[testIndex];
		}

		if (activeTest) {
			struct TestTimings currentTest = {0};
			if (activeTest->test) {
				activeTest->test(&currentTest);
				printResults(activeTest->testName, &currentTest, &calibration, &activeTest->expected, activeTest->modes);
			} else {
				printResults(activeTest->testName, &calibration, &currentTest, &activeTest->expected, activeTest->modes);
			}
		} else {
			if (keys & KEY_UP) {
				--testIndex;
				if (testIndex < 0) {
					testIndex = sizeof(tests) / sizeof(*tests) - 1;
				}
			}
			if (keys & KEY_DOWN) {
				++testIndex;
				testIndex %= sizeof(tests) / sizeof(*tests);
			}
			if (testIndex < viewIndex) {
				viewIndex = testIndex;
			} else if (testIndex >= viewIndex + VIEW_SIZE) {
				viewIndex = testIndex - VIEW_SIZE + 1;
			}
			strcpy(&textGrid[32], "Timing tests");
			size_t i;
			for (i = 0; i < sizeof(tests) / sizeof(*tests) && i < VIEW_SIZE; ++i) {
				snprintf(&textGrid[96 + i * 32], 30, "%c%s", (i + viewIndex == testIndex) ? '>' : ' ', tests[i + viewIndex].testName);
			}
		}

		updateTextGrid();
		VBlankIntrWait();
	}

	return 0;
}
