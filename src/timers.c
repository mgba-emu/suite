#include "timers.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_timers.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TimerTest {
	const char* testName;
	u32 delay;
	u32 timer;
	u32 addTightLoop;
	u32 addLooseLoop;
};

static const struct TimerTest timerTests[] = {
	{ "0b, 0d, 0x0001",  0, 0xC0FFFF, 0x0002, 0x0010 },
	{ "0b, 0d, 0x0003",  0, 0xC0FFFC, 0x0002, 0x0010 },
	{ "0b, 0d, 0x0005",  0, 0xC0FFFB, 0x0003, 0x0010 },
	{ "0b, 0d, 0x000C",  0, 0xC0FFF4, 0x0003, 0x0020 },
	{ "0b, 0d, 0x000D",  0, 0xC0FFF3, 0x0004, 0x0020 },
	{ "0b, 0d, 0x0010",  0, 0xC0FFF0, 0x0004, 0x0020 },
	{ "0b, 0d, 0x0014",  0, 0xC0FFEC, 0x0004, 0x0020 },
	{ "0b, 0d, 0x0015",  0, 0xC0FFEB, 0x0005, 0x0020 },
	{ "0b, 0d, 0x0020",  0, 0xC0FFE0, 0x0006, 0x0020 },
	{ "0b, 0d, 0x0024",  0, 0xC0FFDC, 0x0006, 0x0030 },
	{ "0b, 0d, 0x0025",  0, 0xC0FFDB, 0x0007, 0x0030 },
	{ "0b, 0d, 0x0040",  0, 0xC0FFC0, 0x000A, 0x0040 },
	{ "0b, 0d, 0x0080",  0, 0xC0FF80, 0x0012, 0x0070 },
	{ "0b, 0d, 0x0100",  0, 0xC0FF00, 0x0022, 0x00C0 },
	{ "0b, 0d, 0x0200",  0, 0xC0FE00, 0x0042, 0x0170 },
	{ "0b, 0d, 0x0400",  0, 0xC0FC00, 0x0082, 0x02E0 },
	{ "0b, 0d, 0x0800",  0, 0xC0F800, 0x0102, 0x05A0 },
	{ "0b, 0d, 0x1000",  0, 0xC0F000, 0x0202, 0x0B30 },
	{ "0b, 0d, 0x2000",  0, 0xC0E000, 0x0402, 0x1650 },
	{ "0b, 0d, 0x4000",  0, 0xC0C000, 0x0802, 0x2C90 },
	{ "0b, 0d, 0x8000",  0, 0xC08000, 0x1002, 0x5920 },
	{ "6b, 0d, 0x0010",  0, 0xC1FFF0, 0x0082, 0x02D0 },
	{ "6b, 0d, 0x0011",  0, 0xC1FFEF, 0x008A, 0x0300 },
	{ "6b, 0d, 0x0012",  0, 0xC1FFEE, 0x0092, 0x0330 },
	{ "6b, 0d, 0x0013",  0, 0xC1FFED, 0x009A, 0x0360 },
	{ "8b, 0d, 0x0010",  0, 0xC2FFF0, 0x01EA, 0x0AB0 },
	{ "8b, 0d, 0x0011",  0, 0xC2FFEF, 0x020A, 0x0B60 },
	{ "8b, 0d, 0x0012",  0, 0xC2FFEE, 0x022A, 0x0C10 },
	{ "8b, 0d, 0x0013",  0, 0xC2FFED, 0x024A, 0x0CC0 },
	{ "10b, 0d, 0x0010", 0, 0xC3FFF0, 0x07EA, 0x2C10 },
	{ "10b, 0d, 0x0011", 0, 0xC3FFEF, 0x086A, 0x2ED0 },
	{ "10b, 0d, 0x0012", 0, 0xC3FFEE, 0x08EA, 0x31A0 },
	{ "10b, 0d, 0x0013", 0, 0xC3FFED, 0x096A, 0x3460 },
	{ "0b, 1d, 0x0010",  1, 0xC0FFF0, 0x0004, 0x0020 },
	{ "0b, 1d, 0x0014",  1, 0xC0FFEC, 0x0004, 0x0020 },
	{ "0b, 1d, 0x0015",  1, 0xC0FFEB, 0x0005, 0x0020 },
	{ "6b, 1d, 0x0010",  1, 0xC1FFF0, 0x0081, 0x02D0 },
	{ "6b, 1d, 0x0011",  1, 0xC1FFEF, 0x0089, 0x0300 },
	{ "8b, 1d, 0x0010",  1, 0xC2FFF0, 0x01E9, 0x0AA0 },
	{ "8b, 1d, 0x0011",  1, 0xC2FFEF, 0x0209, 0x0B60 },
	{ "10b, 1d, 0x0010", 1, 0xC3FFF0, 0x07E9, 0x2C10 },
	{ "10b, 1d, 0x0011", 1, 0xC3FFEF, 0x0869, 0x2ED0 },
	{ "0b, 2d, 0x0010",  2, 0xC0FFF0, 0x0004, 0x0020 },
	{ "0b, 2d, 0x0014",  2, 0xC0FFEC, 0x0004, 0x0020 },
	{ "0b, 2d, 0x0015",  2, 0xC0FFEB, 0x0005, 0x0020 },
	{ "6b, 2d, 0x0010",  2, 0xC1FFF0, 0x0080, 0x02D0 },
	{ "6b, 2d, 0x0011",  2, 0xC1FFEF, 0x0088, 0x0300 },
	{ "8b, 2d, 0x0010",  2, 0xC2FFF0, 0x01E8, 0x0AA0 },
	{ "8b, 2d, 0x0011",  2, 0xC2FFEF, 0x0208, 0x0B50 },
	{ "10b, 2d, 0x0010", 2, 0xC3FFF0, 0x07E8, 0x2C00 },
	{ "10b, 2d, 0x0011", 2, 0xC3FFEF, 0x0868, 0x2ED0 },
};

static const u32 nTimerTests = sizeof(timerTests) / sizeof(*timerTests);

static unsigned passes;
static unsigned totalResults;

static void testIrq(void) {
	REG_TM0CNT_H = 0;
}

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct TimerTest* test) {
	irqSet(IRQ_TIMER0, testIrq);
	irqDisable(IRQ_VBLANK);
	irqEnable(IRQ_TIMER0);

	__asm__ volatile("ldr r0, =0f \n"
		"bx r0 \n"
		".arm; 0:\n"
		"ldr r4, =0x4000100 \n"
		"ldr r5, =0x0C3FFFE \n"

		"str r5, [r4] \n"
		"mov r0, #1 \n"
		"mov r1, #8 \n"
		"swi 0x40000 \n"
		"mov r0, #0 \n"
		"1: \n"
		"cmp r0, %[delay] \n"
		"addne r0, #1 \n"
		"bne 1b \n"
		"mov r0, #0 \n"
		"str %[timer], [r4] \n"
		"1: \n"
		"add r0, #1 \n"
		"ldrh r2, [r4, #2] \n"
		"tst r2, #0x80 \n"
		"bne 1b \n"
		"str r0, %[tightLoop] \n"

		"str r5, [r4] \n"
		"mov r0, #1 \n"
		"mov r1, #8 \n"
		"swi 0x40000 \n"
		"mov r0, #0 \n"
		"1: \n"
		"cmp r0, %[delay] \n"
		"addne r0, #1 \n"
		"bne 1b \n"
		"mov r0, #0 \n"
		"str %[timer], [r4] \n"
		"1: \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"add r0, #1 \n"
		"ldrh r2, [r4, #2] \n"
		"tst r2, #0x80 \n"
		"bne 1b \n"
		"str r0, %[looseLoop] \n"
		"ldr r0, =2f \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; 2:"
		: [tightLoop]"=m"(test->addTightLoop), [looseLoop]"=m"(test->addLooseLoop)
		: [timer]"r"(test->timer), [delay]"r"(test->delay)
		: "r0", "r1", "r2", "r3", "r4", "r5");

	irqDisable(IRQ_TIMER0);
	irqEnable(IRQ_VBLANK);
}

static void printResult(int offset, int line, const char* preface, s32 value, s32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-4s: %06lX", preface, value);
	if (value == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 19], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 19], 16, "!= %06lX", expected);
	}
}

static void doResult(const char* preface, s32 value, s32 expected) {
	if (value != expected) {
		savprintf("%s: Got %08lX vs %08lX: FAIL", preface, value, expected);
	} else {
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct TimerTest* values, const struct TimerTest* expected, int base) {
	snprintf(&textGrid[32], 31, "Timer count-up test: %s", preface);
	printResult(base, 0, "Tight loop", values->addTightLoop, expected->addTightLoop);
	printResult(base, 1, "Loose loop", values->addLooseLoop, expected->addLooseLoop);
}

static void runTimersSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct TimerTest* activeTest = 0;
	int i;
	for (i = 0; i < nTimerTests; ++i) {
		struct TimerTest currentTest = {0};
		activeTest = &timerTests[i];
		memcpy(&currentTest, activeTest, sizeof(currentTest));
		runTest(&currentTest);

		savprintf("Math test: %s", activeTest->testName);
		doResult("Tight loop", currentTest.addTightLoop, activeTest->addTightLoop);
		doResult("Loose loop", currentTest.addLooseLoop, activeTest->addLooseLoop);
	}
}

static size_t listTimersSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTimerTests) {
			break;
		}
		names[i] = timerTests[i + offset].testName;
	}
	return i;
}

static void showTimersSuite(size_t index) {
	const struct TimerTest* activeTest = &timerTests[index];
	struct TimerTest currentTest = {0};
	size_t resultIndex = 0;
	memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
	memcpy(&currentTest, activeTest, sizeof(currentTest));
	runTest(&currentTest);
	printResults(activeTest->testName, &currentTest, activeTest, resultIndex);
	updateTextGrid();

	while (1) {
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_B) {
			return;
		}
	}
}

const struct TestSuite timersTestSuite = {
	.name = "Timer count-up tests",
	.run = runTimersSuite,
	.list = listTimersSuite,
	.show = showTimersSuite,
	.nTests = sizeof(timerTests) / sizeof(*timerTests),
	.passes = &passes,
	.totalResults = &totalResults
};
