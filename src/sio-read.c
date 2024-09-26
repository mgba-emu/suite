#include "sio-read.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_sio.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct SIOReadTest {
	const char* testName;
	u32 address;
	u16 siocnt;
	u16 rcnt;
	u16 expected;
};

static const struct SIOReadTest sioReadTests[] = {
	{ "M: SIOMULTI0",     0x04000120, 0x2000, 0, 0 },
	{ "M: SIOMULTI1",     0x04000122, 0x2000, 0, 0 },
	{ "M: SIOMULTI2",     0x04000124, 0x2000, 0, 0 },
	{ "M: SIOMULTI3",     0x04000126, 0x2000, 0, 0 },
	{ "M: SIOCNT",        0x04000128, 0x2000, 0, 0x6F8F },
	{ "M: SIOMLT_SEND",   0x0400012A, 0x2000, 0, 0xFFFF },
	{ "M: RCNT",          0x04000134, 0x2000, 0, 0x01FF },
	{ "M: INVALID (136)", 0x04000136, 0x2000, 0, 0 },
	{ "M: JOYCNT",        0x04000140, 0x2000, 0, 0x0040 },
	{ "M: INVALID (142)", 0x04000142, 0x2000, 0, 0 },
	{ "M: JOY_RECV_L",    0x04000150, 0x2000, 0, 0 },
	{ "M: JOY_RECV_H",    0x04000152, 0x2000, 0, 0 },
	{ "M: JOY_TRANS_L",   0x04000154, 0x2000, 0, 0 },
	{ "M: JOY_TRANS_H",   0x04000156, 0x2000, 0, 0 },
	{ "M: JOYSTAT",       0x0400015A, 0x2000, 0, 0 },

	{ "N8: SIODATA32_L",   0x04000120, 0x0000, 0, 0 },
	{ "N8: SIODATA32_H",   0x04000122, 0x0000, 0, 0 },
	{ "N8: SIOMULTI2",     0x04000124, 0x0000, 0, 0 },
	{ "N8: SIOMULTI3",     0x04000126, 0x0000, 0, 0 },
	{ "N8: SIOCNT",        0x04000128, 0x0000, 0, 0x4F8F },
	{ "N8: SIODATA8",      0x0400012A, 0x0000, 0, 0xFFFF },
	{ "N8: RCNT",          0x04000134, 0x0000, 0, 0x01F5 },
	{ "N8: INVALID (136)", 0x04000136, 0x0000, 0, 0 },
	{ "N8: JOYCNT",        0x04000140, 0x0000, 0, 0x0040 },
	{ "N8: INVALID (142)", 0x04000142, 0x0000, 0, 0 },
	{ "N8: JOY_RECV_L",    0x04000150, 0x0000, 0, 0 },
	{ "N8: JOY_RECV_H",    0x04000152, 0x0000, 0, 0 },
	{ "N8: JOY_TRANS_L",   0x04000154, 0x0000, 0, 0 },
	{ "N8: JOY_TRANS_H",   0x04000156, 0x0000, 0, 0 },
	{ "N8: JOYSTAT",       0x0400015A, 0x0000, 0, 0 },

	{ "N32: SIODATA32_L",   0x04000120, 0x1000, 0, 0xFFFF },
	{ "N32: SIODATA32_H",   0x04000122, 0x1000, 0, 0xFFFF },
	{ "N32: SIOMULTI2",     0x04000124, 0x1000, 0, 0 },
	{ "N32: SIOMULTI3",     0x04000126, 0x1000, 0, 0 },
	{ "N32: SIOCNT",        0x04000128, 0x1000, 0, 0x5F8F },
	{ "N32: SIODATA8",      0x0400012A, 0x1000, 0, 0xFFFF },
	{ "N32: RCNT",          0x04000134, 0x1000, 0, 0x01F5 },
	{ "N32: INVALID (136)", 0x04000136, 0x1000, 0, 0 },
	{ "N32: JOYCNT",        0x04000140, 0x1000, 0, 0x0040 },
	{ "N32: INVALID (142)", 0x04000142, 0x1000, 0, 0 },
	{ "N32: JOY_RECV_L",    0x04000150, 0x1000, 0, 0 },
	{ "N32: JOY_RECV_H",    0x04000152, 0x1000, 0, 0 },
	{ "N32: JOY_TRANS_L",   0x04000154, 0x1000, 0, 0 },
	{ "N32: JOY_TRANS_H",   0x04000156, 0x1000, 0, 0 },
	{ "N32: JOYSTAT",       0x0400015A, 0x1000, 0, 0 },

	{ "U: SIODATA32_L",   0x04000120, 0x3000, 0, 0 },
	{ "U: SIODATA32_H",   0x04000122, 0x3000, 0, 0 },
	{ "U: SIOMULTI2",     0x04000124, 0x3000, 0, 0 },
	{ "U: SIOMULTI3",     0x04000126, 0x3000, 0, 0 },
	{ "U: SIOCNT",        0x04000128, 0x3000, 0, 0x7FAF },
	{ "U: SIODATA8",      0x0400012A, 0x3000, 0, 0 },
	{ "U: RCNT",          0x04000134, 0x3000, 0, 0x01FF },
	{ "U: INVALID (136)", 0x04000136, 0x3000, 0, 0 },
	{ "U: JOYCNT",        0x04000140, 0x3000, 0, 0x0040 },
	{ "U: INVALID (142)", 0x04000142, 0x3000, 0, 0 },
	{ "U: JOY_RECV_L",    0x04000150, 0x3000, 0, 0 },
	{ "U: JOY_RECV_H",    0x04000152, 0x3000, 0, 0 },
	{ "U: JOY_TRANS_L",   0x04000154, 0x3000, 0, 0 },
	{ "U: JOY_TRANS_H",   0x04000156, 0x3000, 0, 0 },
	{ "U: JOYSTAT",       0x0400015A, 0x3000, 0, 0 },

	{ "G: SIODATA32_L",   0x04000120, 0, 0x8000, 0 },
	{ "G: SIODATA32_H",   0x04000122, 0, 0x8000, 0 },
	{ "G: SIOMULTI2",     0x04000124, 0, 0x8000, 0 },
	{ "G: SIOMULTI3",     0x04000126, 0, 0x8000, 0 },
	{ "G: SIOCNT",        0x04000128, 0, 0x8000, 0x4F8F },
	{ "G: SIODATA8",      0x0400012A, 0, 0x8000, 0xFFFF },
	{ "G: RCNT",          0x04000134, 0, 0x8000, 0x81FF },
	{ "G: INVALID (136)", 0x04000136, 0, 0x8000, 0 },
	{ "G: JOYCNT",        0x04000140, 0, 0x8000, 0x0040 },
	{ "G: INVALID (142)", 0x04000142, 0, 0x8000, 0 },
	{ "G: JOY_RECV_L",    0x04000150, 0, 0x8000, 0 },
	{ "G: JOY_RECV_H",    0x04000152, 0, 0x8000, 0 },
	{ "G: JOY_TRANS_L",   0x04000154, 0, 0x8000, 0 },
	{ "G: JOY_TRANS_H",   0x04000156, 0, 0x8000, 0 },
	{ "G: JOYSTAT",       0x0400015A, 0, 0x8000, 0 },

	{ "J: SIODATA32_L",   0x04000120, 0, 0xC000, 0 },
	{ "J: SIODATA32_H",   0x04000122, 0, 0xC000, 0 },
	{ "J: SIOMULTI2",     0x04000124, 0, 0xC000, 0 },
	{ "J: SIOMULTI3",     0x04000126, 0, 0xC000, 0 },
	{ "J: SIOCNT",        0x04000128, 0, 0xC000, 0x4F8F },
	{ "J: SIODATA8",      0x0400012A, 0, 0xC000, 0xFFFF },
	{ "J: RCNT",          0x04000134, 0, 0xC000, 0xC1FC },
	{ "J: INVALID (136)", 0x04000136, 0, 0xC000, 0 },
	{ "J: JOYCNT",        0x04000140, 0, 0xC000, 0x0040 },
	{ "J: INVALID (142)", 0x04000142, 0, 0xC000, 0 },
	{ "J: JOY_RECV_L",    0x04000150, 0, 0xC000, 0 },
	{ "J: JOY_RECV_H",    0x04000152, 0, 0xC000, 0 },
	{ "J: JOY_TRANS_L",   0x04000154, 0, 0xC000, 0 },
	{ "J: JOY_TRANS_H",   0x04000156, 0, 0xC000, 0 },
	{ "J: JOYSTAT",       0x0400015A, 0, 0xC000, 0 },
};

static const u32 nTests = sizeof(sioReadTests) / sizeof(*sioReadTests);

static unsigned passes;
static unsigned totalResults;
EWRAM_DATA static bool results[sizeof(sioReadTests) / sizeof(*sioReadTests)];

static u16 _runTest(const struct SIOReadTest* test) {
	REG_IME = 0;
	activeTestInfo.subtestId = 0;
	REG_RCNT = test->rcnt;
	REG_SIOCNT = test->siocnt;
	u16 value = 0xFFFF;
	if (test->address == 0x04000128) {
		value = (REG_SIOCNT & 0x3000) | 0xCFFF;
	} else if (test->address == 0x04000134) {
		value = (REG_RCNT & 0xC000) | 0x3FFF;
	}
	vu16* address = (vu16*) test->address;
	*address = value;
	u16 result;
	__asm__ __volatile__ (
			"ldrh %[result], %[address] \n"
			"b 1f\n"
			".word 0xDEADDEAD \n"
			"1: \n"
		: [result]"=r"(result)
		: [address]"m"(*address),[siocnt]"r"(test->siocnt), [rcnt]"r"(test->rcnt)
		: "r0"
		);
	activeTestInfo.subtestId = -1;
	if (test->address != 0x04000134) {
		*address = 0;
	}
	REG_SIOCNT = 0;
	REG_RCNT = R_GPIO;
	REG_IME = 1;
	return result;
}

static void printResults(int offset, int line, const struct SIOReadTest* test) {
	static const int base = 3;
	if (offset > line * 2 || base + (line * 2 - offset + 1) > 18) {
		return;
	}

	line += line - offset + base;
	snprintf(TEXT_LOC(line, 0), 31, "%s:", test->testName);
	u16 value = _runTest(test);
	if (value == test->expected) {
		markLinePass(line);
		markLinePass(line + 1);
		snprintf(TEXT_LOC(line + 1, 4), 28, "%04X PASS", value);
	} else {
		markLineFail(line);
		markLineFail(line + 1);
		snprintf(TEXT_LOC(line + 1, 4), 28, "%04X != %04X", value, test->expected);
	}
}

static void doResult(const struct SIOReadTest* test) {
	u16 value = _runTest(test);
	if (value != test->expected) {
		debugprintf("FAIL: %s", test->testName);
		savprintf("%s: Got 0x%04X vs 0x%04X: FAIL", test->testName, value, test->expected);
	} else {
		debugprintf("PASS: %s", test->testName);
		++passes;
	}
	++totalResults;
}

static size_t listSIOReadSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = sioReadTests[i + offset].testName;
		passed[i] = results[i + offset];
	}
	return i;
}

static void runSIOReadSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct SIOReadTest* activeTest = 0;
	int i;
	for (i = 0; i < nTests; ++i) {
		activeTestInfo.testId = i;
		activeTest = &sioReadTests[i];
		unsigned failed = totalResults - passes;
		doResult(activeTest);
		results[i] = failed == totalResults - passes;
	}
	activeTestInfo.testId = -1;
}

static void showSIOReadSuite(size_t index) {
	const struct SIOReadTest* activeTest = &sioReadTests[index];
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
			if (resultIndex < 0) {
				++resultIndex;
			}
		}
		printResults(0, resultIndex, activeTest);
		updateTextGrid();
	}
	activeTestInfo.testId = -1;
}

const struct TestSuite sioReadTestSuite = {
	.name = "SIO register R/W tests",
	.run = runSIOReadSuite,
	.list = listSIOReadSuite,
	.show = showSIOReadSuite,
	.nTests = sizeof(sioReadTests) / sizeof(*sioReadTests),
	.passes = &passes,
	.totalResults = &totalResults
};
