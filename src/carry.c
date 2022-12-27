#include "carry.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TestCarry {
	s32 in0;
	s32 in1;
	s32 inCpsr;
	s32 outAdc;
	s32 outAdcCpsr;
	s32 outSbc;
	s32 outSbcCpsr;
	s32 outRsc;
	s32 outRscCpsr;
};

struct CarryTest {
	const char* testName;
	struct TestCarry expected;
};

static const struct CarryTest carryTests[] = {
	{ "0, 0 (.)", { 0, 0, 0x0000001F, 0, 0x4000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFF, 0x8000001F } },
	{ "0, 0 (C)", { 0, 0, 0x2000001F, 1, 0x0000001F, 0, 0x6000001F, 0, 0x6000001F } },
	{ "0, 0x7FFFFFFF (.)", { 0, 0x7FFFFFFF, 0x0000001F, 0x7FFFFFFF, 0x0000001F, 0x80000000, 0x8000001F, 0x7FFFFFFE, 0x2000001F } },
	{ "0, 0x7FFFFFFF (C)", { 0, 0x7FFFFFFF, 0x2000001F, 0x80000000, 0x9000001F, 0x80000001, 0x8000001F, 0x7FFFFFFF, 0x2000001F } },
	{ "0, 0x80000000 (.)", { 0, 0x80000000, 0x0000001F, 0x80000000, 0x8000001F, 0x7FFFFFFF, 0x0000001F, 0x7FFFFFFF, 0x3000001F } },
	{ "0, 0x80000000 (C)", { 0, 0x80000000, 0x2000001F, 0x80000001, 0x8000001F, 0x80000000, 0x9000001F, 0x80000000, 0xA000001F } },
	{ "0, 0xFFFFFFFF (.)", { 0, 0xFFFFFFFF, 0x0000001F, 0xFFFFFFFF, 0x8000001F, 0, 0x4000001F, 0xFFFFFFFE, 0xA000001F } },
	{ "0, 0xFFFFFFFF (C)", { 0, 0xFFFFFFFF, 0x2000001F, 0, 0x6000001F, 1, 0x0000001F, 0xFFFFFFFF, 0xA000001F } },
	{ "0x7FFFFFFF, 0 (.)", { 0x7FFFFFFF, 0, 0x0000001F, 0x7FFFFFFF, 0x0000001F, 0x7FFFFFFE, 0x2000001F, 0x80000000, 0x8000001F } },
	{ "0x7FFFFFFF, 0 (C)", { 0x7FFFFFFF, 0, 0x2000001F, 0x80000000, 0x9000001F, 0x7FFFFFFF, 0x2000001F, 0x80000001, 0x8000001F } },
	{ "0x7FFFFFFF, 0x7FFFFFFF (.)", { 0x7FFFFFFF, 0x7FFFFFFF, 0x0000001F, 0xFFFFFFFE, 0x9000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFF, 0x8000001F } },
	{ "0x7FFFFFFF, 0x7FFFFFFF (C)", { 0x7FFFFFFF, 0x7FFFFFFF, 0x2000001F, 0xFFFFFFFF, 0x9000001F, 0, 0x6000001F, 0, 0x6000001F } },
	{ "0x7FFFFFFF, 0x80000000 (.)", { 0x7FFFFFFF, 0x80000000, 0x0000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFE, 0x9000001F, 0, 0x7000001F } },
	{ "0x7FFFFFFF, 0x80000000 (C)", { 0x7FFFFFFF, 0x80000000, 0x2000001F, 0, 0x6000001F, 0xFFFFFFFF, 0x9000001F, 1, 0x3000001F } },
	{ "0x7FFFFFFF, 0xFFFFFFFF (.)", { 0x7FFFFFFF, 0xFFFFFFFF, 0x0000001F, 0x7FFFFFFE, 0x2000001F, 0x7FFFFFFF, 0x0000001F, 0x7FFFFFFF, 0x3000001F } },
	{ "0x80000000, 0 (.)", { 0x80000000, 0, 0x0000001F, 0x80000000, 0x8000001F, 0x7FFFFFFF, 0x3000001F, 0x7FFFFFFF, 0x0000001F } },
	{ "0x80000000, 0 (C)", { 0x80000000, 0, 0x2000001F, 0x80000001, 0x8000001F, 0x80000000, 0xA000001F, 0x80000000, 0x9000001F } },
	{ "0x80000000, 0x7FFFFFFF (.)", { 0x80000000, 0x7FFFFFFF, 0x0000001F, 0xFFFFFFFF, 0x8000001F, 0, 0x7000001F, 0xFFFFFFFE, 0x9000001F } },
	{ "0x80000000, 0x7FFFFFFF (C)", { 0x80000000, 0x7FFFFFFF, 0x2000001F, 0, 0x6000001F, 1, 0x3000001F, 0xFFFFFFFF, 0x9000001F } },
	{ "0x80000000, 0x80000000 (.)", { 0x80000000, 0x80000000, 0x0000001F, 0, 0x7000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFF, 0x8000001F } },
	{ "0x80000000, 0x80000000 (C)", { 0x80000000, 0x80000000, 0x2000001F, 1, 0x3000001F, 0, 0x6000001F, 0, 0x6000001F } },
	{ "0x80000000, 0xFFFFFFFF (.)", { 0x80000000, 0xFFFFFFFF, 0x0000001F, 0x7FFFFFFF, 0x3000001F, 0x80000000, 0x8000001F, 0x7FFFFFFE, 0x2000001F } },
	{ "0x80000000, 0xFFFFFFFF (C)", { 0x80000000, 0xFFFFFFFF, 0x2000001F, 0x80000000, 0xA000001F, 0x80000001, 0x8000001F, 0x7FFFFFFF, 0x2000001F } },
	{ "0xFFFFFFFF, 0 (.)", { 0xFFFFFFFF, 0, 0x0000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFE, 0xA000001F, 0, 0x4000001F } },
	{ "0xFFFFFFFF, 0 (C)", { 0xFFFFFFFF, 0, 0x2000001F, 0, 0x6000001F, 0xFFFFFFFF, 0xA000001F, 1, 0x0000001F } },
	{ "0xFFFFFFFF, 0x7FFFFFFF (.)", { 0xFFFFFFFF, 0x7FFFFFFF, 0x0000001F, 0x7FFFFFFE, 0x2000001F, 0x7FFFFFFF, 0x3000001F, 0x7FFFFFFF, 0x0000001F } },
	{ "0xFFFFFFFF, 0x7FFFFFFF (C)", { 0xFFFFFFFF, 0x7FFFFFFF, 0x2000001F, 0x7FFFFFFF, 0x2000001F, 0x80000000, 0xA000001F, 0x80000000, 0x9000001F } },
	{ "0xFFFFFFFF, 0x80000000 (.)", { 0xFFFFFFFF, 0x80000000, 0x0000001F, 0x7FFFFFFF, 0x3000001F, 0x7FFFFFFE, 0x2000001F, 0x80000000, 0x8000001F } },
	{ "0xFFFFFFFF, 0x80000000 (C)", { 0xFFFFFFFF, 0x80000000, 0x2000001F, 0x80000000, 0xA000001F, 0x7FFFFFFF, 0x2000001F, 0x80000001, 0x8000001F } },
	{ "0xFFFFFFFF, 0xFFFFFFFF (.)", { 0xFFFFFFFF, 0xFFFFFFFF, 0x0000001F, 0xFFFFFFFE, 0xA000001F, 0xFFFFFFFF, 0x8000001F, 0xFFFFFFFF, 0x8000001F } },
	{ "0xFFFFFFFF, 0xFFFFFFFF (C)", { 0xFFFFFFFF, 0xFFFFFFFF, 0x2000001F, 0xFFFFFFFF, 0xA000001F, 0, 0x6000001F, 0, 0x6000001F } },
};

static const u32 nCarryTests = sizeof(carryTests) / sizeof(*carryTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(carryTests) / sizeof(*carryTests)];

__attribute__((noinline))
static void runTest(struct TestCarry* test) {
	activeTestInfo.subtestId = 0;
	__asm__ __volatile__("ldr r0, =enterA \n"
		"bx r0 \n"
		".arm; enterA:\n"
		"msr cpsr_f, %[psrIn]\n"
		"adcs %[out], %[in0], %[in1]\n"
		"mrs %[psrOut], cpsr\n"
		"ldr r0, =exitA \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitA:"
		: [out]"=r"(test->outAdc), [psrOut]"=r"(test->outAdcCpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1), [psrIn]"r"(test->inCpsr)
		: "r0");
	activeTestInfo.subtestId = 1;
	__asm__ __volatile__("ldr r0, =enterS \n"
		"bx r0 \n"
		".arm; enterS:\n"
		"msr cpsr_f, %[psrIn]\n"
		"sbcs %[out], %[in0], %[in1]\n"
		"mrs %[psrOut], cpsr\n"
		"ldr r0, =exitS \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitS:"
		: [out]"=r"(test->outSbc), [psrOut]"=r"(test->outSbcCpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1), [psrIn]"r"(test->inCpsr)
		: "r0");
	activeTestInfo.subtestId = 2;
	__asm__ __volatile__("ldr r0, =enterR \n"
		"bx r0 \n"
		".arm; enterR:\n"
		"msr cpsr_f, %[psrIn]\n"
		"rscs %[out], %[in0], %[in1]\n"
		"mrs %[psrOut], cpsr\n"
		"ldr r0, =exitR \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitR:"
		: [out]"=r"(test->outRsc), [psrOut]"=r"(test->outRscCpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1), [psrIn]"r"(test->inCpsr)
		: "r0");
	activeTestInfo.subtestId = -1;
}

static void printResult(int offset, int line, const char* preface, s32 value, u32 cpsr, s32 expected, u32 expectedCpsr) {
	static const int base = 3;
	if (offset > line || base + line - offset > 18) {
		return;
	}

	line += base - offset;
	snprintf(TEXT_LOC(line, 0), 31, "%-4s: %08lX (%lX)", preface, value, cpsr >> 28);
	if (value == expected && cpsr == expectedCpsr) {
		markLinePass(line);
		strncpy(TEXT_LOC(line, 21), "PASS", 10);
	} else {
		markLineFail(line);
		snprintf(TEXT_LOC(line, 16), 16, "!= %08lX (%lX)", expected, expectedCpsr >> 28);
	}
}

static void doResult(const char* preface, const char* testName, s32 value, u32 cpsr, s32 expected, u32 expectedCpsr) {
	if (value != expected || cpsr != expectedCpsr) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("%s: Got %08lX (CSPR %lX) vs %08lX (CSPR %lX): FAIL", preface, value, cpsr >> 28, expected, expectedCpsr >> 28);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct TestCarry* values, const struct TestCarry* expected, int base) {
	snprintf(&textGrid[32], 31, "Carry test: %s", preface);
	printResult(base, 0, "adcs", values->outAdc, values->outAdcCpsr, expected->outAdc, expected->outAdcCpsr);
	printResult(base, 2, "sbcs", values->outSbc, values->outSbcCpsr, expected->outSbc, expected->outSbcCpsr);
	printResult(base, 4, "rscs", values->outRsc, values->outRscCpsr, expected->outRsc, expected->outRscCpsr);
}

static void runCarrySuite(void) {
	passes = 0;
	totalResults = 0;
	const struct CarryTest* activeTest = 0;
	int i;
	for (i = 0; i < nCarryTests; ++i) {
		struct TestCarry currentTest = {0};
		activeTest = &carryTests[i];
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		activeTestInfo.testId = i;
		runTest(&currentTest);

		unsigned failed = totalResults - passes;
		savprintf("Carry test: %s", activeTest->testName);
		doResult("adcs", activeTest->testName, currentTest.outAdc, currentTest.outAdcCpsr, activeTest->expected.outAdc, activeTest->expected.outAdcCpsr);
		doResult("sbcs", activeTest->testName, currentTest.outSbc, currentTest.outSbcCpsr, activeTest->expected.outSbc, activeTest->expected.outSbcCpsr);
		doResult("rscs", activeTest->testName, currentTest.outRsc, currentTest.outRscCpsr, activeTest->expected.outRsc, activeTest->expected.outRscCpsr);
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static size_t listCarrySuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nCarryTests) {
			break;
		}
		names[i] = carryTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void showCarrySuite(size_t index) {
	const struct CarryTest* activeTest = &carryTests[index];
	struct TestCarry currentTest = {0};
	size_t resultIndex = 0;
	activeTestInfo.testId = index;
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
	activeTestInfo.testId = -1;
}

const struct TestSuite carryTestSuite = {
	.name = "Carry tests",
	.run = runCarrySuite,
	.list = listCarrySuite,
	.show = showCarrySuite,
	.nTests = sizeof(carryTests) / sizeof(*carryTests),
	.passes = &passes,
	.totalResults = &totalResults
};
