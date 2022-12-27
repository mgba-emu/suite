#include "multiply-long.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct MultiplyLong {
	s32 in0;
	s32 in1;
	s32 outSmull0;
	s32 outSmull1;
	s32 outSmullCpsr;
	s32 outUmull0;
	s32 outUmull1;
	s32 outUmullCpsr;
	s32 in2;
	s32 in3;
	s32 outSmlal0;
	s32 outSmlal1;
	s32 outSmlalCpsr;
	s32 outUmlal0;
	s32 outUmlal1;
	s32 outUmlalCpsr;
};

struct MultiplyLongTest {
	const char* testName;
	struct MultiplyLong expected;
};

static const struct MultiplyLongTest multiplyLongTests[] = {
	{ "  0 *   0", { 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 *   0", { 0x00000001, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ " -1 *   0", { 0xFFFFFFFF, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "$7F *   0", { 0x7FFFFFFF, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "$80 *   0", { 0x80000000, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "$81 *   0", { 0x80000001, 0x00000000,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},

	{ "  0 *   1", { 0x00000000, 0x00000001,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 *   1", { 0x00000001, 0x00000001,
		0x00000000, 0x00000001, 0x0000001F,
		0x00000000, 0x00000001, 0x0000001F, }},
	{ " -1 *   1", { 0xFFFFFFFF, 0x00000001,
		0xFFFFFFFF, 0xFFFFFFFF, 0x8000001F,
		0x00000000, 0xFFFFFFFF, 0x0000001F, }},
	{ "$7F *   1", { 0x7FFFFFFF, 0x00000001,
		0x00000000, 0x7FFFFFFF, 0x0000001F,
		0x00000000, 0x7FFFFFFF, 0x0000001F, }},
	{ "$80 *   1", { 0x80000000, 0x00000001,
		0xFFFFFFFF, 0x80000000, 0x8000001F,
		0x00000000, 0x80000000, 0x0000001F, }},
	{ "$81 *   1", { 0x80000001, 0x00000001,
		0xFFFFFFFF, 0x80000001, 0x8000001F,
		0x00000000, 0x80000001, 0x0000001F, }},

	{ "  0 *  -1", { 0x00000000, 0xFFFFFFFF,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 *  -1", { 0x00000001, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0x8000001F,
		0x00000000, 0xFFFFFFFF, 0x0000001F, }},
	{ " -1 *  -1", { 0xFFFFFFFF, 0xFFFFFFFF,
		0x00000000, 0x00000001, 0x0000001F,
		0xFFFFFFFE, 0x00000001, 0xA000001F, }},
	{ "$7F *  -1", { 0x7FFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0x80000001, 0x8000001F,
		0x7FFFFFFE, 0x80000001, 0x2000001F, }},
	{ "$80 *  -1", { 0x80000000, 0xFFFFFFFF,
		0x00000000, 0x80000000, 0x0000001F,
		0x7FFFFFFF, 0x80000000, 0x0000001F, }},
	{ "$81 *  -1", { 0x80000001, 0xFFFFFFFF,
		0x00000000, 0x7FFFFFFF, 0x0000001F,
		0x80000000, 0x7FFFFFFF, 0x8000001F, }},

	{ "  0 * $7F", { 0x00000000, 0x7FFFFFFF,
		0x00000000, 0x00000000, 0x4000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 * $7F", { 0x00000001, 0x7FFFFFFF,
		0x00000000, 0x7FFFFFFF, 0x0000001F,
		0x00000000, 0x7FFFFFFF, 0x0000001F, }},
	{ " -1 * $7F", { 0xFFFFFFFF, 0x7FFFFFFF,
		0xFFFFFFFF, 0x80000001, 0xA000001F,
		0x7FFFFFFE, 0x80000001, 0x2000001F, }},
	{ "$7F * $7F", { 0x7FFFFFFF, 0x7FFFFFFF,
		0x3FFFFFFF, 0x00000001, 0x0000001F,
		0x3FFFFFFF, 0x00000001, 0x0000001F, }},
	{ "$80 * $7F", { 0x80000000, 0x7FFFFFFF,
		0xC0000000, 0x80000000, 0xA000001F,
		0x3FFFFFFF, 0x80000000, 0x2000001F, }},
	{ "$81 * $7F", { 0x80000001, 0x7FFFFFFF,
		0xC0000000, 0xFFFFFFFF, 0xA000001F,
		0x3FFFFFFF, 0xFFFFFFFF, 0x2000001F, }},

	{ "  0 * $80", { 0x00000000, 0x80000000,
		0x00000000, 0x00000000, 0x6000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 * $80", { 0x00000001, 0x80000000,
		0xFFFFFFFF, 0x80000000, 0xA000001F,
		0x00000000, 0x80000000, 0x0000001F, }},
	{ " -1 * $80", { 0xFFFFFFFF, 0x80000000,
		0x00000000, 0x80000000, 0x0000001F,
		0x7FFFFFFF, 0x80000000, 0x2000001F, }},
	{ "$7F * $80", { 0x7FFFFFFF, 0x80000000,
		0xC0000000, 0x80000000, 0xA000001F,
		0x3FFFFFFF, 0x80000000, 0x0000001F, }},
	{ "$80 * $80", { 0x80000000, 0x80000000,
		0x40000000, 0x00000000, 0x0000001F,
		0x40000000, 0x00000000, 0x2000001F, }},
	{ "$81 * $80", { 0x80000001, 0x80000000,
		0x3FFFFFFF, 0x80000000, 0x0000001F,
		0x40000000, 0x80000000, 0x2000001F, }},

	{ "  0 * $81", { 0x00000000, 0x80000001,
		0x00000000, 0x00000000, 0x6000001F,
		0x00000000, 0x00000000, 0x4000001F, }},
	{ "  1 * $81", { 0x00000001, 0x80000001,
		0xFFFFFFFF, 0x80000001, 0xA000001F,
		0x00000000, 0x80000001, 0x0000001F, }},
	{ " -1 * $81", { 0xFFFFFFFF, 0x80000001,
		0x00000000, 0x7FFFFFFF, 0x0000001F,
		0x80000000, 0x7FFFFFFF, 0xA000001F, }},
	{ "$7F * $81", { 0x7FFFFFFF, 0x80000001,
		0xC0000000, 0xFFFFFFFF, 0xA000001F,
		0x3FFFFFFF, 0xFFFFFFFF, 0x0000001F, }},
	{ "$80 * $81", { 0x80000000, 0x80000001,
		0x3FFFFFFF, 0x80000000, 0x0000001F,
		0x40000000, 0x80000000, 0x2000001F, }},
	{ "$81 * $81", { 0x80000001, 0x80000001,
		0x3FFFFFFF, 0x00000001, 0x0000001F,
		0x40000001, 0x00000001, 0x2000001F, }},
};

static const u32 nMultiplyLongTests = sizeof(multiplyLongTests) / sizeof(*multiplyLongTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(multiplyLongTests) / sizeof(*multiplyLongTests)];

__attribute__((noinline))
static void runTest(struct MultiplyLong* test) {
	activeTestInfo.subtestId = 0;
	__asm__ __volatile__("ldr r0, =enterSl \n"
		"bx r0 \n"
		".arm; enterSl:\n"
		"msr cpsr_f, #0\n"
		"smulls r9, r8, %[in0], %[in1]\n"
		"mrs %[psrOut], cpsr\n"
		"str r8, %[out0]\n"
		"str r9, %[out1]\n"
		"ldr r0, =exitSl \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitSl:"
		: [out0]"=m"(test->outSmull0), [out1]"=m"(test->outSmull1), [psrOut]"=r"(test->outSmullCpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1)
		: "r0", "r8", "r9");
	activeTestInfo.subtestId = 1;
	__asm__ __volatile__("ldr r0, =enterUl \n"
		"bx r0 \n"
		".arm; enterUl:\n"
		"msr cpsr_f, #0\n"
		"umulls r9, r8, %[in0], %[in1]\n"
		"mrs %[psrOut], cpsr\n"
		"str r8, %[out0]\n"
		"str r9, %[out1]\n"
		"ldr r0, =exitUl \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitUl:"
		: [out0]"=m"(test->outUmull0), [out1]"=m"(test->outUmull1),  [psrOut]"=r"(test->outUmullCpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1)
		: "r0", "r8", "r9");
	activeTestInfo.subtestId = 2;
	if (test->in2 || test->in3) {
		__asm__ __volatile__("ldr r0, =enterSa \n"
			"bx r0 \n"
			".arm; enterSa:\n"
			"msr cpsr_f, #0\n"
			"ldr r8, %[in3]\n"
			"ldr r9, %[in2]\n"
			"smlals r9, r8, %[in0], %[in1]\n"
			"mrs %[psrOut], cpsr\n"
			"str r8, %[out0]\n"
			"str r9, %[out1]\n"
			"ldr r0, =exitSa \n"
			"add r0, #1 \n"
			"bx r0 \n"
			".ltorg \n"
			".thumb; exitSa:"
			: [out0]"=m"(test->outSmlal0), [out1]"=m"(test->outSmlal1), [psrOut]"=r"(test->outSmlalCpsr)
			: [in0]"r"(test->in0), [in1]"r"(test->in1), [in2]"m"(test->in2), [in3]"m"(test->in3)
			: "r0", "r8", "r9");
		activeTestInfo.subtestId = 3;
		__asm__ __volatile__("ldr r0, =enterUa \n"
			"bx r0 \n"
			".arm; enterUa:\n"
			"msr cpsr_f, #0\n"
			"ldr r8, %[in3]\n"
			"ldr r9, %[in2]\n"
			"umlals r9, r8, %[in0], %[in1]\n"
			"mrs %[psrOut], cpsr\n"
			"str r8, %[out0]\n"
			"str r9, %[out1]\n"
			"ldr r0, =exitUa \n"
			"add r0, #1 \n"
			"bx r0 \n"
			".ltorg \n"
			".thumb; exitUa:"
			: [out0]"=m"(test->outUmlal0), [out1]"=m"(test->outUmlal1), [psrOut]"=r"(test->outUmlalCpsr)
			: [in0]"r"(test->in0), [in1]"r"(test->in1), [in2]"m"(test->in2), [in3]"m"(test->in3)
			: "r0", "r8", "r9");
	}
	activeTestInfo.subtestId = -1;
}

static void printResult(int offset, int line, const char* preface, s32 value0, s32 value1, u32 cpsr, s32 expected0, s32 expected1, u32 expectedCpsr) {
	static const int base = 3;
	if (offset > line || base + line - offset > 18) {
		return;
	}

	line += base - offset;
	snprintf(TEXT_LOC(line, 0), 31, "%-4s: %08lX:%08lX (%lX)", preface, value0, value1, cpsr >> 28);
	if (value0 == expected0 && value1 == expected1 && cpsr == expectedCpsr) {
		markLinePass(line);
		markLinePass(line + 1);
		strncpy(TEXT_LOC(line + 1, 26), "PASS", 10);
	} else {
		markLineFail(line);
		markLineFail(line + 1);
		snprintf(TEXT_LOC(line + 1, 4), 30, "!= %08lX:%08lX (%lX)", expected0, expected1, expectedCpsr >> 28);
	}
}

static void doResult(const char* preface, const char* testName, s32 value0, s32 value1, u32 cpsr, s32 expected0, s32 expected1, u32 expectedCpsr) {
	if (value0 != expected0 || value1 != expected1 || cpsr != expectedCpsr) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("%s: Got %08lX:%08lX (CSPR %lX) vs %08lX:%08lX (CSPR %lX): FAIL", preface, value0, value1, cpsr >> 28, expected0, expected1, expectedCpsr >> 28);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct MultiplyLong* values, const struct MultiplyLong* expected, int base) {
	snprintf(&textGrid[32], 31, "Multiply long test: %s", preface);
	printResult(base, 0, "smull", values->outSmull0, values->outSmull1, values->outSmullCpsr, expected->outSmull0, expected->outSmull1, expected->outSmullCpsr);
	printResult(base, 2, "umull", values->outUmull0, values->outUmull1, values->outUmullCpsr, expected->outUmull0, expected->outUmull1, expected->outUmullCpsr);
	if (values->in2 || values->in3) {
		printResult(base, 4, "smlal", values->outSmlal0, values->outSmlal1, values->outSmlalCpsr, expected->outSmlal0, expected->outSmlal1, expected->outSmlalCpsr);
		printResult(base, 6, "umlal", values->outUmlal0, values->outUmlal1, values->outUmlalCpsr, expected->outUmlal0, expected->outUmlal1, expected->outUmlalCpsr);
	}
}

static void runMultiplyLongSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct MultiplyLongTest* activeTest = 0;
	int i;
	for (i = 0; i < nMultiplyLongTests; ++i) {
		struct MultiplyLong currentTest = {0};
		activeTest = &multiplyLongTests[i];
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		activeTestInfo.testId = i;
		runTest(&currentTest);

		unsigned failed = totalResults - passes;
		savprintf("Multiply Long test: %s", activeTest->testName);
		doResult("smulls", activeTest->testName, currentTest.outSmull0, currentTest.outSmull1, currentTest.outSmullCpsr, activeTest->expected.outSmull0, activeTest->expected.outSmull1, activeTest->expected.outSmullCpsr);
		doResult("umulls", activeTest->testName, currentTest.outUmull0, currentTest.outUmull1, currentTest.outUmullCpsr, activeTest->expected.outUmull0, activeTest->expected.outUmull1, activeTest->expected.outUmullCpsr);
		if (currentTest.in2 || currentTest.in3) {
			doResult("smlals", activeTest->testName, currentTest.outSmlal0, currentTest.outSmlal1, currentTest.outSmlalCpsr, activeTest->expected.outSmlal0, activeTest->expected.outSmlal1, activeTest->expected.outSmlalCpsr);
			doResult("umlals", activeTest->testName, currentTest.outUmlal0, currentTest.outUmlal1, currentTest.outUmlalCpsr, activeTest->expected.outUmlal0, activeTest->expected.outUmlal1, activeTest->expected.outUmlalCpsr);
		}
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static size_t listMultiplyLongSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nMultiplyLongTests) {
			break;
		}
		names[i] = multiplyLongTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void showMultiplyLongSuite(size_t index) {
	const struct MultiplyLongTest* activeTest = &multiplyLongTests[index];
	struct MultiplyLong currentTest = {0};
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

const struct TestSuite multiplyLongTestSuite = {
	.name = "Multiply long tests",
	.run = runMultiplyLongSuite,
	.list = listMultiplyLongSuite,
	.show = showMultiplyLongSuite,
	.nTests = sizeof(multiplyLongTests) / sizeof(*multiplyLongTests),
	.passes = &passes,
	.totalResults = &totalResults
};
