#include "misc-edge.h"

#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_timers.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct MiscEdgeTest {
	const char* testName;
	void (*test)(u32* outputs);
	const char* valueNames[32];
	u32 expected[32];
};

static void dmaPrefetch(u32* out);
static void hblankBit(u32* out);

static const struct MiscEdgeTest miscEdgeTests[] = {
	{ "DMA Prefetch", dmaPrefetch, { "Break", "Read", 0 }, { 0x10002A64, 0xDEAD0000 } },
	{ "H-blank bit start", hblankBit, { "Start", "Hblank", "Flip 1", "Flip 2", "Flip 3", "Flip 4", "Flip 5", "Flip 6", 0 }, { 0x1B, 0x4D1, 0x84, 0x3EC, 0xE4, 0x3EC, 0xE4, 0x3F5 } },
};

static const u32 nTests = sizeof(miscEdgeTests) / sizeof(*miscEdgeTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(miscEdgeTests) / sizeof(*miscEdgeTests)];

void dmaPrefetch(u32* out) {
	u32* ptr = (u32*) 0x10000000;
	out[0] = 0;
	u32 a[8] = { 0xDEAD0000, 0xDEAD0001, 0xDEAD0002, 0xDEAD0003, 0xDEAD0004, 0xDEAD0005, 0xDEAD0006, 0xDEAD0007 };
	u32 b[8] = { 0 };
	DMA3COPY(&a, &b, DMA_HBLANK | DMA_SRC_FIXED | DMA_DST_FIXED | DMA_REPEAT | DMA32 | 1);
	VBlankIntrWait();
	REG_IME = 0;
	int i;
	for (i = 0; i < 0x00008000; ++i) {
		u32 value = *ptr;
		++ptr;
		out[1] = value;
		if (value != 0x428A428A) {
			out[0] = (u32) ptr;
			REG_DMA3CNT = 0;
			return;
		}
	}
	REG_DMA3CNT = 0;
	REG_IME = 1;
}

IWRAM_CODE
void hblankBit(u32* out) {
	irqInit();
	VBlankIntrWait();
	REG_TM0CNT = TIMER_START << 16;
	VBlankIntrWait();
	REG_IME = 0;
	REG_DISPSTAT |= 0x10;
	u16 ie = REG_IE;
	REG_IE = IRQ_HBLANK;
	REG_IME = 1;
	Halt(); // Halt
	int calibration = REG_TM0CNT_L;
	Halt(); // Halt
	int value = REG_TM0CNT_L;
	out[1] = value - calibration;
	calibration = value;
	u16 bit = REG_DISPSTAT;
	REG_IME = 0;
	out[0] = bit;
	bit ^= 2;
	while (((bit ^ REG_DISPSTAT) & 2));
	value = REG_TM0CNT_L;
	out[2] = value - calibration;
	calibration = value;
	bit ^= 2;
	while ((bit ^ REG_DISPSTAT) & 2);
	value = REG_TM0CNT_L;
	out[3] = value - calibration;
	calibration = value;
	bit ^= 2;
	while (((bit ^ REG_DISPSTAT) & 2));
	value = REG_TM0CNT_L;
	out[4] = value - calibration;
	calibration = value;
	bit ^= 2;
	while ((bit ^ REG_DISPSTAT) & 2);
	value = REG_TM0CNT_L;
	out[5] = value - calibration;
	calibration = value;
	bit ^= 2;
	while (((bit ^ REG_DISPSTAT) & 2));
	value = REG_TM0CNT_L;
	out[6] = value - calibration;
	calibration = value;
	bit ^= 2;
	while ((bit ^ REG_DISPSTAT) & 2);
	value = REG_TM0CNT_L;
	out[7] = value - calibration;
	calibration = value;
	REG_DISPSTAT &= ~0x10;
	REG_TM0CNT = 0;
	REG_IE = ie;
	REG_IME = 1;
}

static void printResult(int offset, int line, const char* preface, u32 expected, u32 value) {
	static const int base = 3;
	if (offset > line * 2 || base + (line * 2 - offset + 1) > 18) {
		return;
	}

	line += line - offset + base;
	snprintf(TEXT_LOC(line, 0), 31, "%s:", preface);
	if (value == expected) {
		markLinePass(line);
		markLinePass(line + 1);
		snprintf(TEXT_LOC(line + 1, 4), 28, "%08lX PASS", value);
	} else {
		markLineFail(line);
		markLineFail(line + 1);
		snprintf(TEXT_LOC(line + 1, 4), 28, "%08lX != %08lX", value, expected);
	}
}

static void doResult(const char* preface, const char* testName, s32 value, s32 expected) {
	if (value != expected) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("%s: Got 0x%08lX vs 0x%08lX: FAIL", preface, value, expected);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const struct MiscEdgeTest* test, u32* values, int base) {
	snprintf(&textGrid[GRID_STRIDE], 31, "Miscellaneous: %s", test->testName);

	int i;
	for (i = 0; test->valueNames[i]; ++i) {
		printResult(base, i,  test->valueNames[i], test->expected[i], values[i]);
	}
}

static size_t listMiscEdgeSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = miscEdgeTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void runMiscEdgeSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct MiscEdgeTest* activeTest = 0;
	int i;
	for (i = 0; i < nTests; ++i) {
		activeTestInfo.testId = i;
		activeTest = &miscEdgeTests[i];
		u32 currentTest[32] = {0};
		activeTest->test(currentTest);
		int j;
		unsigned failed = totalResults - passes;
		for (j = 0; activeTest->valueNames[j]; ++j) {
			doResult(activeTest->valueNames[j], activeTest->testName, activeTest->expected[j], currentTest[j]);
		}
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static void showMiscEdgeSuite(size_t index) {
	const struct MiscEdgeTest* activeTest = &miscEdgeTests[index];
	size_t resultIndex = 0;
	activeTestInfo.testId = index;
	u32 currentTest[32] = {0};
	activeTest->test(currentTest);
	while (1) {
		memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_B) {
			return;
		}

		if (keys & KEY_UP) {
			if (resultIndex > 0) {
				--resultIndex;
			}
		}
		if (keys & KEY_DOWN) {
			if (resultIndex < 0) {
				++resultIndex;
			}
		}
		printResults(activeTest, currentTest, resultIndex);
		updateTextGrid();
	}
	activeTestInfo.testId = -1;
}

const struct TestSuite miscEdgeTestSuite = {
	.name = "Misc. edge case tests",
	.run = runMiscEdgeSuite,
	.list = listMiscEdgeSuite,
	.show = showMiscEdgeSuite,
	.nTests = sizeof(miscEdgeTests) / sizeof(*miscEdgeTests),
	.passes = &passes,
	.totalResults = &totalResults
};
