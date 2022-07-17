#include "shifter.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TestOutput {
	u32 rd;
	u32 cpsr;
};

struct ShifterTest {
	const char* testName;
	void (*test)(struct TestOutput*);
	u32 inCpsr;
	u32 in0;
	u32 in1;
	struct TestOutput expected;
};

void movImm0(struct TestOutput*);
void movImmFF(struct TestOutput*);
void movImmFF000000(struct TestOutput*);
void movImmF000000F(struct TestOutput*);
void movR1(struct TestOutput*);
void movR1Lsl0(struct TestOutput*);
void movR1Lsl1(struct TestOutput*);
void movR1Lsl31(struct TestOutput*);
void movR1LslR0(struct TestOutput*);
void movR1Lsr1(struct TestOutput*);
void movR1Lsr31(struct TestOutput*);
void movR1Lsr32(struct TestOutput*);
void movR1LsrR0(struct TestOutput*);
void movR1Asr1(struct TestOutput*);
void movR1Asr31(struct TestOutput*);
void movR1Asr32(struct TestOutput*);
void movR1AsrR0(struct TestOutput*);
void movR1Ror1(struct TestOutput*);
void movR1Ror31(struct TestOutput*);
void movR1RorR0(struct TestOutput*);
void movR1Rrx(struct TestOutput*);
void movPC(struct TestOutput*);
void movPCLsl0(struct TestOutput*);
void movPCLsl1(struct TestOutput*);
void movPCLsl31(struct TestOutput*);
void movPCLslR0(struct TestOutput*);
void movPCLsr1(struct TestOutput*);
void movPCLsr31(struct TestOutput*);
void movPCLsr32(struct TestOutput*);
void movPCLsrR0(struct TestOutput*);
void movPCAsr1(struct TestOutput*);
void movPCAsr31(struct TestOutput*);
void movPCAsr32(struct TestOutput*);
void movPCAsrR0(struct TestOutput*);
void movPCRor1(struct TestOutput*);
void movPCRor31(struct TestOutput*);
void movPCRorR0(struct TestOutput*);
void movPCRrx(struct TestOutput*);
void addR0PCR1(struct TestOutput*);
void addR0PCR1Lsl0(struct TestOutput*);
void addR0PCR1Lsl1(struct TestOutput*);
void addR0PCR1Lsl31(struct TestOutput*);
void addR0PCR1LslR0(struct TestOutput*);
void addR0PCR1Lsr1(struct TestOutput*);
void addR0PCR1Lsr31(struct TestOutput*);
void addR0PCR1Lsr32(struct TestOutput*);
void addR0PCR1LsrR0(struct TestOutput*);
void addR0PCR1Asr1(struct TestOutput*);
void addR0PCR1Asr31(struct TestOutput*);
void addR0PCR1Asr32(struct TestOutput*);
void addR0PCR1AsrR0(struct TestOutput*);
void addR0PCR1Ror1(struct TestOutput*);
void addR0PCR1Ror31(struct TestOutput*);
void addR0PCR1RorR0(struct TestOutput*);

#define ADDR(F) ((u32)(F) + 8)

static const struct ShifterTest shifterTests[] = {
	{ "#0", movImm0, 0, 0, 0, { 0, 0x4000001F } },
	{ "#FF", movImmFF, 0, 0, 0, { 0xFF, 0x1F } },
	{ "#FF000000", movImmFF000000, 0, 0, 0, { 0xFF000000, 0xA000001F } },
	{ "#F000000F", movImmF000000F, 0, 0, 0, { 0xF000000F, 0xA000001F } },
	{ "r1", movR1, 0, 0, 0xFFF, { 0xFFF, 0x1F } },
	{ "r1, lsl #0", movR1Lsl0, 0, 0, 0xFFF, { 0xFFF, 0x1F } },
	{ "r1, lsl #1", movR1Lsl1, 0, 0, 0xFFF, { 0x1FFE, 0x1F } },
	{ "r1, lsl #31", movR1Lsl31, 0, 0, 0x3, { 0x80000000, 0xA000001F } },
	{ "r1, lsl r0 (0)", movR1LslR0, 0, 0, 0xFFF, { 0xFFF, 0x1F } },
	{ "r1, lsl r0 (1)", movR1LslR0, 0, 1, 0xFFF, { 0x1FFE, 0x1F } },
	{ "r1, lsl r0 (31)", movR1LslR0, 0, 31, 0x3, { 0x80000000, 0xA000001F } },
	{ "r1, lsl r0 (32)", movR1LslR0, 0, 32, 0x3, { 0, 0x6000001F } },
	{ "r1, lsl r0 (33)", movR1LslR0, 0, 33, 0x3, { 0, 0x4000001F } },
	{ "r1, lsl r0 (-1)", movR1LslR0, 0, -1, 0x3, { 0, 0x4000001F } },
	{ "r1, lsr #1", movR1Lsr1, 0, 0, 0xFFF, { 0x7FF, 0x2000001F } },
	{ "r1, lsr #31", movR1Lsr31, 0, 0, 0xC0000000, { 0x1, 0x2000001F } },
	{ "r1, lsr #32", movR1Lsr32, 0, 0, 0x80000FFF, { 0, 0x6000001F } },
	{ "r1, lsr r0 (0)", movR1LsrR0, 0, 0, 0xFFF, { 0xFFF, 0x1F } },
	{ "r1, lsr r0 (1)", movR1LsrR0, 0, 1, 0xFFF, { 0x7FF, 0x2000001F } },
	{ "r1, lsr r0 (31)", movR1LsrR0, 0, 31, 0xC0000000, { 0x1, 0x2000001F } },
	{ "r1, lsr r0 (32)", movR1LsrR0, 0, 32, 0xC0000000, { 0, 0x6000001F } },
	{ "r1, lsr r0 (33)", movR1LsrR0, 0, 33, 0xC0000000, { 0, 0x4000001F } },
	{ "r1, lsr r0 (-1)", movR1LsrR0, 0, -1, 0xC0000000, { 0, 0x4000001F } },
	{ "r1, asr #1", movR1Asr1, 0, 0, 0x80000FFF, { 0xC00007FF, 0xA000001F } },
	{ "r1, asr #31", movR1Asr31, 0, 0, 0xC0000000, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, asr #32", movR1Asr32, 0, 0, 0x80000FFF, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, asr r0 (0)", movR1AsrR0, 0, 0, 0x80000FFF, { 0x80000FFF, 0x8000001F } },
	{ "r1, asr r0 (1)", movR1AsrR0, 0, 1, 0x80000FFF, { 0xC00007FF, 0xA000001F } },
	{ "r1, asr r0 (31)", movR1AsrR0, 0, 31, 0xC0000000, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, asr r0 (32)", movR1AsrR0, 0, 32, 0xC0000000, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, asr r0 (33)", movR1AsrR0, 0, 33, 0xC0000000, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, asr r0 (-1)", movR1AsrR0, 0, -1, 0xC0000000, { 0xFFFFFFFF, 0xA000001F } },
	{ "r1, ror #1", movR1Ror1, 0, 0, 0x12345678, { 0x091A2B3C, 0x1F } },
	{ "r1, ror #31", movR1Ror31, 0, 0, 0x12345678, { 0x2468ACF0, 0x1F } },
	{ "r1, ror r0 (0)", movR1RorR0, 0, 0, 0x12345678, { 0x12345678, 0x1F } },
	{ "r1, ror r0 (1)", movR1RorR0, 0, 1, 0x12345678, { 0x091A2B3C, 0x1F } },
	{ "r1, ror r0 (31)", movR1RorR0, 0, 31, 0x12345678, { 0x2468ACF0, 0x1F } },
	{ "r1, ror r0 (32)", movR1RorR0, 0, 32, 0x12345678, { 0x12345678, 0x1F } },
	{ "r1, ror r0 (33)", movR1RorR0, 0, 33, 0x12345678, { 0x091A2B3C, 0x1F } },
	{ "r1, ror r0 (-1)", movR1RorR0, 0, -1, 0x12345678, { 0x2468ACF0, 0x1F } },
	{ "r1, rrx (+)", movR1Rrx, 0, 0, 0x12345678, { 0x091A2B3C, 0x1F } },
	{ "r1, rrx (-)", movR1Rrx, 0, 0, 0x87654321, { 0x43B2A190, 0x2000001F } },
	{ "pc", movPC, 0, 0, 0, { ADDR(movPC), 0x1F } },
	{ "pc, lsl #0", movPCLsl0, 0, 0, 0, { ADDR(movPCLsl0), 0x1F } },
	{ "pc, lsl #31", movPCLsl31, 0, 0, 0, { 0, 0x4000001F } },
	{ "pc, lsl r0 (0)", movPCLslR0, 0, 0, 0, { ADDR(movPCLslR0) + 4, 0x1F } },
	{ "pc, lsr #31", movPCLsr31, 0, 0, 0, { 0, 0x4000001F } },
	{ "pc, lsr #32", movPCLsr32, 0, 0, 0, { 0, 0x4000001F } },
	{ "pc, lsr r0 (0)", movPCLsrR0, 0, 0, 0, { ADDR(movPCLsrR0) + 4, 0x1F } },
	{ "pc, lsr r0 (24)", movPCLsrR0, 0, 24, 0, { 0x8, 0x1F } },
	{ "pc, asr #31", movPCAsr31, 0, 0, 0, { 0, 0x4000001F } },
	{ "pc, asr #32", movPCAsr32, 0, 0, 0, { 0, 0x4000001F } },
	{ "pc, asr r0 (0)", movPCAsrR0, 0, 0, 0, { ADDR(movPCAsrR0) + 4, 0x1F } },
	{ "pc, asr r0 (24)", movPCAsrR0, 0, 24, 0, { 0x8, 0x1F } },
	{ "add r0, pc, r1", addR0PCR1, 0, 0, 0, { ADDR(addR0PCR1), 0x1F } },
	{ "add r0, pc, r1, lsl #0", addR0PCR1Lsl0, 0, 0, 1, { ADDR(addR0PCR1Lsl0) + 1, 0x1F } },
	{ "add r0, pc, r1, lsl #1", addR0PCR1Lsl1, 0, 0, 1, { ADDR(addR0PCR1Lsl1) + 2, 0x1F } },
	{ "add r0, pc, r1, lsl r0 (0)", addR0PCR1LslR0, 0, 0, 1, { ADDR(addR0PCR1LslR0) + 5, 0x1F } },
	{ "add r0, pc, r1, lsl r0 (1)", addR0PCR1LslR0, 0, 1, 1, { ADDR(addR0PCR1LslR0) + 6, 0x1F } },
	{ "add r0, pc, r1, lsr #1", addR0PCR1Lsr1, 0, 0, 1, { ADDR(addR0PCR1Lsr1), 0x1F } },
	{ "add r0, pc, r1, lsr #32", addR0PCR1Lsr32, 0, 0, 1, { ADDR(addR0PCR1Lsr32), 0x1F } },
	{ "add r0, pc, r1, lsr r0 (0)", addR0PCR1LsrR0, 0, 0, 1, { ADDR(addR0PCR1LsrR0) + 5, 0x1F } },
	{ "add r0, pc, r1, lsr r0 (1)", addR0PCR1LsrR0, 0, 1, 1, { ADDR(addR0PCR1LsrR0) + 4, 0x1F } },
	{ "add r0, pc, r1, asr #1", addR0PCR1Asr1, 0, 0, 0x80000000, { ADDR(addR0PCR1Asr1) + 0xC0000000, 0x8000001F } },
	{ "add r0, pc, r1, asr #32", addR0PCR1Asr32, 0, 0, 0x80000000, { ADDR(addR0PCR1Asr32) - 1, 0x2000001F } },
	{ "add r0, pc, r1, asr r0 (0)", addR0PCR1AsrR0, 0, 0, 0x80000000, { ADDR(addR0PCR1AsrR0) + 0x80000004, 0x8000001F } },
	{ "add r0, pc, r1, asr r0 (1)", addR0PCR1AsrR0, 0, 1, 0x80000000, { ADDR(addR0PCR1AsrR0) + 0xC0000004, 0x8000001F } },
	{ "add r0, pc, r1, ror #1", addR0PCR1Ror1, 0, 0, 0xA0000005, { ADDR(addR0PCR1Ror1) + 0xD0000002, 0x8000001F } },
	{ "add r0, pc, r1, ror r0 (0)", addR0PCR1RorR0, 0, 0, 0xA0000005, { ADDR(addR0PCR1RorR0) + 0xA0000009, 0x8000001F } },
	{ "add r0, pc, r1, ror r0 (1)", addR0PCR1RorR0, 0, 1, 0xA0000005, { ADDR(addR0PCR1RorR0) + 0xD0000006, 0x8000001F } },
};

static const u32 nShifterTests = sizeof(shifterTests) / sizeof(*shifterTests);

static unsigned passes;
static unsigned totalResults;

__attribute__((noinline))
static void runTest(const struct ShifterTest* test, struct TestOutput* out) {
	activeTestInfo.subtestId = 0;
	__asm__ __volatile__("ldr r0, =enter \n"
		"bx r0 \n"
		".arm; enter:\n"
		"msr cpsr_f, %[psrIn] \n"
		"mov r0, %[in0] \n"
		"mov r1, %[in1] \n"
		"mov lr, pc \n"
		"bx %[test] \n"
		"mrs %[psrOut], cpsr \n"
		"str r0, %[out] \n"
		"ldr r0, =exit \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exit:"
		: [out]"=m"(out->rd), [psrOut]"=r"(out->cpsr)
		: [in0]"r"(test->in0), [in1]"r"(test->in1), [psrIn]"r"(test->inCpsr), [test]"r"(test->test)
		: "r0", "r1");
}

static void printResult(int offset, int line, const char* preface, u32 value, u32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-2s: %08lX", preface, value);
	if (value == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 21], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 19], 16, "!= %08lX", expected);
	}
}

static void doResult(const char* testName, const char* preface, u32 value, u32 expected) {
	if (value != expected) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("Got %08lX vs %08lX: FAIL", value, expected);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct TestOutput* values, const struct TestOutput* expected, int base) {
	snprintf(&textGrid[32], 31, "Shifter test: %s", preface);
	printResult(base, 0, "r0", values->rd, expected->rd);
	printResult(base, 1, "cpsr", values->cpsr, expected->cpsr);
}

static void runShifterSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct ShifterTest* activeTest = 0;
	int i;
	for (i = 0; i < nShifterTests; ++i) {
		struct TestOutput currentTest = {0};
		activeTest = &shifterTests[i];
		activeTestInfo.testId = i;
		runTest(activeTest, &currentTest);

		savprintf("Shifter test: %s", activeTest->testName);
		doResult(activeTest->testName, "r0", currentTest.rd, activeTest->expected.rd);
		doResult(activeTest->testName, "cpsr", currentTest.cpsr, activeTest->expected.cpsr);
	}
	activeTestInfo.testId = -1;
}

static size_t listShifterSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nShifterTests) {
			break;
		}
		names[i] = shifterTests[i + offset].testName;
	}
	return i;
}

static void showShifterSuite(size_t index) {
	const struct ShifterTest* activeTest = &shifterTests[index];
	struct TestOutput currentTest = {0};
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
		memset(&currentTest, 0, sizeof(currentTest));
		runTest(activeTest, &currentTest);
		printResults(activeTest->testName, &currentTest, &activeTest->expected, resultIndex);
		updateTextGrid();
	}
	activeTestInfo.testId = -1;
}

const struct TestSuite shifterTestSuite = {
	.name = "Shifter tests",
	.run = runShifterSuite,
	.list = listShifterSuite,
	.show = showShifterSuite,
	.nTests = sizeof(shifterTests) / sizeof(*shifterTests),
	.passes = &passes,
	.totalResults = &totalResults
};
