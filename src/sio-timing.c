#include "sio-timing.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_timers.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct SioTimingTestResult {
	s32 cycles;
	bool timedOut;
};

struct SioTimingTest {
	const char* testName;
	u16 siocnt;
	u16 rcnt;
	struct SioTimingTestResult result;
};

static const struct SioTimingTest sioTimingTests[] = {
	{ "Multi/9.6k", 0x6000, 0x0000, { 0x7D61, true } },
	{ "Multi/38.4k", 0x6001, 0x0000, { 0x2133, true } },
	{ "Multi/57.6k", 0x6002, 0x0000, { 0x16EF, true } },
	{ "Multi/115.2k", 0x6003, 0x0000, { 0xCBD, true } },
	{ "Normal8/256k", 0x4001, 0x0000, { 0x279, false } },
	{ "Normal8/2M", 0x4003, 0x0000, { 0xB9, false } },
	{ "Normal32/256k", 0x5001, 0x0000, { 0x879, false } },
	{ "Normal32/2M", 0x5003, 0x0000, { 0x179, false } },
};

static const u32 nSioTimingTests = sizeof(sioTimingTests) / sizeof(*sioTimingTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(sioTimingTests) / sizeof(*sioTimingTests)];

static bool timedOut = false;

IWRAM_CODE
static void testIrq(void) {
	REG_TM0CNT_H = 0;
	REG_TM1CNT_H = 0;
	timedOut = true;
}

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct SioTimingTest* test) {
	REG_IME = 0;
	irqInit();
	irqSet(IRQ_TIMER1, testIrq);
	int ie = REG_IE;
	REG_IE = IRQ_TIMER1 | IRQ_SERIAL;
	REG_IME = 1;
	timedOut = false;

	__asm__ volatile("ldr r0, =0f \n"
		"bx r0 \n"
		".arm; 0:\n"
		"ldr r4, =0x4000100 \n"
		"mov r0, #0 \n"
		"str r0, [r4] \n" // Clear TM0
		"ldr r0, =0x00C4FFFE \n"
		"str r0, [r4, #4] \n" // Clear TM1, make count-up
		"str %[rcnt], [r4, #0x34] \n"
		"ldr r0, =0xAA550000 \n"
		"orr r0, r0, %[siocnt] \n"
		"str r0, [r4, #0x28] \n" // Set up transfer
		"mov r0, #0x80 \n"
		"mov r5, #0 \n"
		"orr r1, %[siocnt], #0x80 \n"
		"strh r0, [r4, #2] \n"
		"strh r1, [r4, #0x28] \n" // Start
		"swi 0x20000 \n" // Wait for SIO or timeout
		"strh r5, [r4, #2] \n"
		"ldr r0, =1f \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; 1:"
		:
		: [rcnt]"r"(test->rcnt), [siocnt]"r"(test->siocnt)
		: "r0", "r1", "r4", "r5", "memory");

	test->result.timedOut = timedOut;
	if (!timedOut) {
		test->result.cycles = REG_TM0CNT_L;
		test->result.cycles |= (REG_TM1CNT_L & 0x1) << 16;
	}

	REG_IME = 0;
	REG_IE = ie;
	REG_IME = 1;
}

static void doResult(const char* testName, const struct SioTimingTestResult* result, const struct SioTimingTestResult* expected) {
	if (expected->timedOut && result->timedOut) {
		debugprintf("SKIPPED: %s", testName);
		return;
	}
	if (result->timedOut) {
		debugprintf("FAIL: %s", testName);
		savprintf("%s: Timed out: FAIL", testName);
	} else if (result->cycles != expected->cycles) {
		debugprintf("FAIL: %s", testName);
		savprintf("%s: Got %08lX vs %08lX: FAIL", testName, result->cycles, expected->cycles);
	} else {
		debugprintf("PASS: %s", testName);
		++passes;
	}

	++totalResults;
}

static void printResult(int offset, int line, const char* preface, const struct SioTimingTestResult* result, const struct SioTimingTestResult* expected) {
	static const int base = 3;
	if (offset > line || base + line - offset > 18) {
		return;
	}

	line += base - offset;
	if (expected->timedOut && result->timedOut) {
		markLinePass(line);
		snprintf(TEXT_LOC(line, 0), 31, "%-5s: SKIPPED", preface);
		return;
	}
	if (result->timedOut) {
		markLineFail(line);
		snprintf(TEXT_LOC(line, 0), 31, "%-5s: TIMED OUT", preface);
		return;
	}

	snprintf(TEXT_LOC(line, 0), 31, "%-5s: %08lX", preface, result->cycles);
	if (result->cycles == expected->cycles) {
		markLinePass(line);
		strncpy(TEXT_LOC(line, 17), "PASS", 10);
	} else {
		markLineFail(line);
		snprintf(TEXT_LOC(line, 17), 16, "!= %08lX", expected->cycles);
	}
}

static void printResults(const char* preface, const struct SioTimingTest* values, const struct SioTimingTest* expected, int base) {
	snprintf(&textGrid[32], 31, "SIO timing test: %s", preface);
	printResult(base, 0,  "Cycles", &values->result, &expected->result);
}

static void runSioTimingSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct SioTimingTest* activeTest = 0;
	int i;
	for (i = 0; i < nSioTimingTests; ++i) {
		struct SioTimingTest currentTest = {0};
		activeTest = &sioTimingTests[i];
		memcpy(&currentTest, activeTest, sizeof(currentTest));
		activeTestInfo.testId = i;
		runTest(&currentTest);

		unsigned failed = totalResults - passes;
		savprintf("SIO timing test: %s", activeTest->testName);
		doResult(activeTest->testName, &currentTest.result, &activeTest->result);
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static size_t listSioTimingSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nSioTimingTests) {
			break;
		}
		names[i] = sioTimingTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void showSioTimingSuite(size_t index) {
	const struct SioTimingTest* activeTest = &sioTimingTests[index];
	struct SioTimingTest currentTest = {0};
	size_t resultIndex = 0;
	memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
	memcpy(&currentTest, activeTest, sizeof(currentTest));
	activeTestInfo.testId = index;
	runTest(&currentTest);
	updateTextGrid();

	while (1) {
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_UP) {
			if (resultIndex > 0) {
				--resultIndex;
			}
		}
		if (keys & KEY_DOWN) {
			if (resultIndex < 20) {
				++resultIndex;
			}
		}

		if (keys & KEY_B) {
			return;
		}

		printResults(activeTest->testName, &currentTest, activeTest, resultIndex);
		updateTextGrid();
	}
	activeTestInfo.testId = 0;
}

const struct TestSuite sioTimingTestSuite = {
	.name = "SIO timing tests",
	.run = runSioTimingSuite,
	.list = listSioTimingSuite,
	.show = showSioTimingSuite,
	.nTests = sizeof(sioTimingTests) / sizeof(*sioTimingTests),
	.passes = &passes,
	.totalResults = &totalResults
};
