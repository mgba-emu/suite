#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "font.h"
#include "timing.h"

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

static void printResult(int offset, int line, const char* preface, s32 value, s32 calibration, s32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-13s: %5i", preface, value - calibration);
	if (value - calibration == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 21], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 21], 10, "!= %5i", expected);
	}
}

static void printResults(const char* preface, const struct TestTimings* values, const struct TestTimings* calibration, const struct TestTimings* expected, int mode, int base) {
	snprintf(&textGrid[32], 31, "Timing test: %s", preface);

	printResult(base, 0, "ARM/ROM ...", values->arm_text_0000, calibration->arm_text_0000, expected->arm_text_0000);
	printResult(base, 1, "ARM/ROM P..", values->arm_text_4000, calibration->arm_text_4000, expected->arm_text_4000);
	printResult(base, 2, "ARM/ROM .N.", values->arm_text_0004, calibration->arm_text_0004, expected->arm_text_0004);
	printResult(base, 3, "ARM/ROM PN.", values->arm_text_4004, calibration->arm_text_4004, expected->arm_text_4004);
	printResult(base, 4, "ARM/ROM ..S", values->arm_text_0010, calibration->arm_text_0010, expected->arm_text_0010);
	printResult(base, 5, "ARM/ROM P.S", values->arm_text_4010, calibration->arm_text_4010, expected->arm_text_4010);
	printResult(base, 6, "ARM/ROM .NS", values->arm_text_0014, calibration->arm_text_0014, expected->arm_text_0014);
	printResult(base, 7, "ARM/ROM PNS", values->arm_text_4014, calibration->arm_text_4014, expected->arm_text_4014);
	printResult(base, 8, "ARM/WRAM", values->arm_ewram, calibration->arm_ewram, expected->arm_ewram);
	printResult(base, 9, "ARM/IWRAM", values->arm_iwram, calibration->arm_iwram, expected->arm_iwram);

	if (mode & TEST_THUMB) {
		printResult(base, 10, "Thumb/ROM ...", values->thumb_text_0000, calibration->thumb_text_0000, expected->thumb_text_0000);
		printResult(base, 11, "Thumb/ROM P..", values->thumb_text_4000, calibration->thumb_text_4000, expected->thumb_text_4000);
		printResult(base, 12, "Thumb/ROM .N.", values->thumb_text_0004, calibration->thumb_text_0004, expected->thumb_text_0004);
		printResult(base, 13, "Thumb/ROM PN.", values->thumb_text_4004, calibration->thumb_text_4004, expected->thumb_text_4004);
		printResult(base, 14, "Thumb/ROM ..S", values->thumb_text_0010, calibration->thumb_text_0010, expected->thumb_text_0010);
		printResult(base, 15, "Thumb/ROM P.S", values->thumb_text_4010, calibration->thumb_text_4010, expected->thumb_text_4010);
		printResult(base, 16, "Thumb/ROM .NS", values->thumb_text_0014, calibration->thumb_text_0014, expected->thumb_text_0014);
		printResult(base, 17, "Thumb/ROM PNS", values->thumb_text_4014, calibration->thumb_text_4014, expected->thumb_text_4014);
		printResult(base, 18, "Thumb/WRAM", values->thumb_ewram, calibration->thumb_ewram, expected->thumb_ewram);
		printResult(base, 19, "Thumb/IWRAM", values->thumb_iwram, calibration->thumb_iwram, expected->thumb_iwram);
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

	strcpy(&textGrid[139], "Testing...");
	updateTextGrid();
	REG_DISPCNT = MODE_0 | BG1_ON;

	irqEnable(IRQ_VBLANK);

	const struct TimingTest* activeTest = 0;
	int testIndex = 0;
	int viewIndex = 0;
	int resultIndex = 0;

	struct TestTimings calibration;

	int passes = 0;
	int totalResults = 0;
	int i;
	for (i = 0; i < nTimingTests; ++i) {
		struct TestTimings currentTest = {0};
		VBlankIntrWait();
		REG_IME = 0;
		calibrate(&calibration);
		activeTest = &timingTests[i];
		if (activeTest->test) {
			activeTest->test(&currentTest);
		} else {
			currentTest = calibration;
			memset(&calibration, 0, sizeof(calibration));
		}
		REG_IME = 1;
		if (activeTest->expected.arm_text_0000 == currentTest.arm_text_0000 - calibration.arm_text_0000) {
			++passes;
		}
		if (activeTest->expected.arm_text_4000 == currentTest.arm_text_4000 - calibration.arm_text_4000) {
			++passes;
		}
		if (activeTest->expected.arm_text_0004 == currentTest.arm_text_0004 - calibration.arm_text_0004) {
			++passes;
		}
		if (activeTest->expected.arm_text_4004 == currentTest.arm_text_4004 - calibration.arm_text_4004) {
			++passes;
		}
		if (activeTest->expected.arm_text_0010 == currentTest.arm_text_0010 - calibration.arm_text_0010) {
			++passes;
		}
		if (activeTest->expected.arm_text_4010 == currentTest.arm_text_4010 - calibration.arm_text_4010) {
			++passes;
		}
		if (activeTest->expected.arm_text_0014 == currentTest.arm_text_0014 - calibration.arm_text_0014) {
			++passes;
		}
		if (activeTest->expected.arm_text_4014 == currentTest.arm_text_4014 - calibration.arm_text_4014) {
			++passes;
		}
		if (activeTest->expected.arm_ewram == currentTest.arm_ewram - calibration.arm_ewram) {
			++passes;
		}
		if (activeTest->expected.arm_iwram == currentTest.arm_iwram - calibration.arm_iwram) {
			++passes;
		}
		totalResults += 10;
		if (activeTest->modes & TEST_THUMB) {
			if (activeTest->expected.thumb_text_0000 == currentTest.thumb_text_0000 - calibration.thumb_text_0000) {
				++passes;
			}
			if (activeTest->expected.thumb_text_4000 == currentTest.thumb_text_4000 - calibration.thumb_text_4000) {
				++passes;
			}
			if (activeTest->expected.thumb_text_0004 == currentTest.thumb_text_0004 - calibration.thumb_text_0004) {
				++passes;
			}
			if (activeTest->expected.thumb_text_4004 == currentTest.thumb_text_4004 - calibration.thumb_text_4004) {
				++passes;
			}
			if (activeTest->expected.thumb_text_0010 == currentTest.thumb_text_0010 - calibration.thumb_text_0010) {
				++passes;
			}
			if (activeTest->expected.thumb_text_4010 == currentTest.thumb_text_4010 - calibration.thumb_text_4010) {
				++passes;
			}
			if (activeTest->expected.thumb_text_0014 == currentTest.thumb_text_0014 - calibration.thumb_text_0014) {
				++passes;
			}
			if (activeTest->expected.thumb_text_4014 == currentTest.thumb_text_4014 - calibration.thumb_text_4014) {
				++passes;
			}
			if (activeTest->expected.thumb_ewram == currentTest.thumb_ewram - calibration.thumb_ewram) {
				++passes;
			}
			if (activeTest->expected.thumb_iwram == currentTest.thumb_iwram - calibration.thumb_iwram) {
				++passes;
			}
			totalResults += 10;
		}
	}

	activeTest = 0;
	while (1) {
		memset(&textGrid[32], 0, sizeof(textGrid) - 32);
		scanKeys();
		u16 keys = keysDown();

		if (keys & KEY_B) {
			activeTest = 0;
			resultIndex = 0;
		}
		if (keys & KEY_A) {
			activeTest = &timingTests[testIndex];
		}

		if (activeTest) {
			struct TestTimings currentTest = {0};
			if (keys & KEY_UP) {
				if (resultIndex > 0) {
					--resultIndex;
				}
			}
			if (keys & KEY_DOWN) {
				if (resultIndex < 4) {
					++resultIndex;
				}
			}
			if (activeTest->test) {
				activeTest->test(&currentTest);
				printResults(activeTest->testName, &currentTest, &calibration, &activeTest->expected, activeTest->modes, resultIndex);
			} else {
				printResults(activeTest->testName, &calibration, &currentTest, &activeTest->expected, activeTest->modes, resultIndex);
			}
		} else {
			if (keys & KEY_UP) {
				--testIndex;
				if (testIndex < 0) {
					testIndex = nTimingTests - 1;
				}
			}
			if (keys & KEY_DOWN) {
				++testIndex;
				testIndex %= nTimingTests;
			}
			if (testIndex < viewIndex) {
				viewIndex = testIndex;
			} else if (testIndex >= viewIndex + VIEW_SIZE) {
				viewIndex = testIndex - VIEW_SIZE + 1;
			}
			strcpy(&textGrid[32], "Timing tests");
			sprintf(&textGrid[53], "%4u/%-4u", passes, totalResults);
			size_t i;
			for (i = 0; i < nTimingTests && i < VIEW_SIZE; ++i) {
				snprintf(&textGrid[96 + i * 32], 31, "%c%s", (i + viewIndex == testIndex) ? '>' : ' ', timingTests[i + viewIndex].testName);
			}
		}

		updateTextGrid();
		VBlankIntrWait();
	}

	return 0;
}
