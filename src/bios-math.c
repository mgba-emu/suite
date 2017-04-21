#include "bios-math.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TestMath {
	int syscall;
	s32 in0;
	s32 in1;
	s32 outR0;
	s32 outR1;
	s32 outR2;
	s32 outR3;
	s32 outPsr;
};

struct MathTest {
	const char* testName;
	struct TestMath expected;
};

static const struct MathTest mathTests[] = {
	{ "ArcTan 0",          { 9,          0,          0,          0,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan 1",          { 9,          1,          0,          0,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan 0x3FFF",     { 9,     0x3FFF,          0,     0x1FFF, 0xFFFFC002, 0xEF090000,  0x8001, 0x0000001F } },
	{ "ArcTan 0x4000",     { 9,     0x4000,          0,     0x2000, 0xFFFFC000, 0xEF090000,  0x8000, 0x0000001F } },
	{ "ArcTan 0x4001",     { 9,     0x4001,          0,     0x1FFF, 0xFFFFBFFE, 0xEF090000,  0x7FFE, 0x0000001F } },
	{ "ArcTan 0x7FFF",     { 9,     0x7FFF,          0,     0x16D8, 0xFFFF0004, 0xEF090000, 0x22DB6, 0x0000001F } },
	{ "ArcTan 0x8000",     { 9,     0x8000,          0,     0x16A2, 0xFFFF0000, 0xEF090000, 0x22D45, 0x0000001F } },
	{ "ArcTan 0x8001",     { 9,     0x8001,          0,     0x1005, 0xFFFEFFFC, 0xEF090000, 0x22006, 0x0000001F } },
	{ "ArcTan 0xBFFF",     { 9,     0xBFFF,          0,     0x1F64, 0x0001C006, 0xEF090000, 0x17F33, 0x0000001F } },
	{ "ArcTan 0xC000",     { 9,     0xC000,          0, 0xFFFFC360, 0x0001C000, 0xEF090000, 0x10480, 0x0000001F } },
	{ "ArcTan 0xC001",     { 9,     0xC001,          0, 0xFFFFD550, 0x0001BFFA, 0xEF090000, 0x271BD, 0x0000001F } },
	{ "ArcTan 0xFFFF",     { 9,     0xFFFF,          0, 0xFFFFA2FE,          8, 0xEF090000,  0xA2FF, 0x0000001F } },
	{ "ArcTan 0xFFFF0000", { 9, 0xFFFF0000,          0,     0x5D07,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan 0xFFFF0001", { 9, 0xFFFF0001,          0,     0x5D01,          8, 0xEF090000,  0xA2FF, 0x0000001F } },
	{ "ArcTan 0xFFFF3FFF", { 9, 0xFFFF3FFF,          0,     0x2AAF, 0x0001BFFA, 0xEF090000, 0x271BD, 0x0000001F } },
	{ "ArcTan 0xFFFF4000", { 9, 0xFFFF4000,          0,     0x3CA0, 0x0001C000, 0xEF090000, 0x10480, 0x0000001F } },
	{ "ArcTan 0xFFFF4001", { 9, 0xFFFF4001,          0, 0xFFFFE09B, 0x0001C006, 0xEF090000, 0x17F33, 0x0000001F } },
	{ "ArcTan 0xFFFF7FFF", { 9, 0xFFFF7FFF,          0, 0xFFFFEFFA, 0xFFFEFFFC, 0xEF090000, 0x22006, 0x0000001F } },
	{ "ArcTan 0xFFFF8000", { 9, 0xFFFF8000,          0, 0xFFFFE95D, 0xFFFF0000, 0xEF090000, 0x22D45, 0x0000001F } },
	{ "ArcTan 0xFFFF8001", { 9, 0xFFFF8001,          0, 0xFFFFE927, 0xFFFF0004, 0xEF090000, 0x22DB6, 0x0000001F } },
	{ "ArcTan 0xFFFFBFFF", { 9, 0xFFFFBFFF,          0, 0xFFFFE000, 0xFFFFBFFE, 0xEF090000,  0x7FFE, 0x0000001F } },
	{ "ArcTan 0xFFFFC000", { 9, 0xFFFFC000,          0, 0xFFFFE000, 0xFFFFC000, 0xEF090000,  0x8000, 0x0000001F } },
	{ "ArcTan 0xFFFFC001", { 9, 0xFFFFC001,          0, 0xFFFFE000, 0xFFFFC002, 0xEF090000,  0x8001, 0x0000001F } },
	{ "ArcTan 0xFFFFFFFF", { 9, 0xFFFFFFFF,          0, 0xFFFFFFFF,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
};

static const u32 nMathTests = sizeof(mathTests) / sizeof(*mathTests);

static unsigned passes;
static unsigned totalResults;

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct TestMath* test) {
	__asm__("ldr r0, =enterM \n"
		"bx r0 \n"
		".arm; enterM:\n"
		"ldr r0, %[in0] \n"
		"ldr r1, %[in1] \n"
		"orr r2, %[syscall], #0xEF00 \n"
		"lsl r2, r2, #16 \n"
		"str r2, [pc, #4] \n"
		"mov r3, #0 \n"
		"msr cpsr_f, #0 \n"
		"nop \n"
		"str r0, %[R0] \n"
		"str r1, %[R1] \n"
		"str r2, %[R2] \n"
		"str r3, %[R3] \n"
		"mrs %[psr], cpsr \n"
		"ldr r0, =exitM \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitM:"
		: [R0]"=m"(test->outR0), [R1]"=m"(test->outR1), [R2]"=m"(test->outR2), [R3]"=m"(test->outR3), [psr]"=r"(test->outPsr)
		: [in0]"m"(test->in0), [in1]"m"(test->in1), [syscall]"r"(test->syscall)
		: "r0", "r1", "r2", "r3");
}

static void printResult(int offset, int line, const char* preface, s32 value, s32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-4s: %08lX", preface, value);
	if (value == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 15], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 15], 16, "!= %08lX", expected);
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

static void printResults(const char* preface, const struct TestMath* values, const struct TestMath* expected, int base) {
	snprintf(&textGrid[32], 31, "BIOS math test: %s", preface);
	printResult(base, 0, "r0", values->outR0, expected->outR0);
	printResult(base, 1, "r1", values->outR1, expected->outR1);
	printResult(base, 2, "r2", values->outR2, expected->outR2);
	printResult(base, 3, "r3", values->outR3, expected->outR3);
	printResult(base, 4, "cpsr", values->outPsr, expected->outPsr);
}

static void runMathSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct MathTest* activeTest = 0;
	int i;
	for (i = 0; i < nMathTests; ++i) {
		struct TestMath currentTest = {0};
		VBlankIntrWait();
		activeTest = &mathTests[i];
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		runTest(&currentTest);

		savprintf("Math test: %s", activeTest->testName);
		doResult("r0", currentTest.outR0, activeTest->expected.outR0);
		doResult("r1", currentTest.outR1, activeTest->expected.outR1);
		doResult("r2", currentTest.outR2, activeTest->expected.outR2);
		doResult("r3", currentTest.outR3, activeTest->expected.outR3);
		doResult("cpsr", currentTest.outPsr, activeTest->expected.outPsr);
	}
}

static size_t listMathSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nMathTests) {
			break;
		}
		names[i] = mathTests[i + offset].testName;
	}
	return i;
}

static void showMathSuite(size_t index) {
	const struct MathTest* activeTest = &mathTests[index];
	struct TestMath currentTest = {0};
	size_t resultIndex = 0;
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
			if (resultIndex < 4) {
				++resultIndex;
			}
		}
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		runTest(&currentTest);
		printResults(activeTest->testName, &currentTest, &activeTest->expected, resultIndex);
		updateTextGrid();
	}
}

const struct TestSuite biosMathTestSuite = {
	.name = "BIOS math tests",
	.run = runMathSuite,
	.list = listMathSuite,
	.show = showMathSuite,
	.nTests = sizeof(mathTests) / sizeof(*mathTests),
	.passes = &passes,
	.totalResults = &totalResults
};
