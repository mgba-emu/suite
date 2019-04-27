#include "io-read.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_sound.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct IOReadTest {
	const char* testName;
	u32 address;
	bool storeFirst;
	bool restoreAfter;
	u16 expected;
	u16 restore;
};

static const struct IOReadTest ioReadTests[] = {
	{ "BG0CNT", 0x4000008, true, true, 0xDFFF, 0 },
	{ "BG1CNT", 0x400000A, true, true, 0xDFFF, 0 },
	{ "BG2CNT", 0x400000C, true, true, 0xFFFF, 0 },
	{ "BG3CNT", 0x400000E, true, true, 0xFFFF, 0 },
	{ "BG0HOFS", 0x4000010, true, false, 0xDEAD, 0 },
	{ "BG0VOFS", 0x4000012, true, false, 0xDEAD, 0 },
	{ "BG1HOFS", 0x4000014, true, false, 0xDEAD, 0 },
	{ "BG1VOFS", 0x4000016, true, false, 0xDEAD, -4 }, // Set by main
	{ "BG2HOFS", 0x4000018, true, false, 0xDEAD, 0 },
	{ "BG2VOFS", 0x400001A, true, false, 0xDEAD, 0 },
	{ "BG3HOFS", 0x400001C, true, false, 0xDEAD, 0 },
	{ "BG3VOFS", 0x400001E, true, false, 0xDEAD, 0 },
	{ "BG2PA", 0x4000020, true, false, 0xDEAD, 0 },
	{ "BG2PB", 0x4000022, true, false, 0xDEAD, 0 },
	{ "BG2PC", 0x4000024, true, false, 0xDEAD, 0 },
	{ "BG2PD", 0x4000026, true, false, 0xDEAD, 0 },
	{ "BG2X_LO", 0x4000028, true, false, 0xDEAD, 0 },
	{ "BG2X_HI", 0x400002A, true, false, 0xDEAD, 0 },
	{ "BG2Y_LO", 0x400002C, true, false, 0xDEAD, 0 },
	{ "BG2Y_HI", 0x400002E, true, false, 0xDEAD, 0 },
	{ "BG3PA", 0x4000030, true, false, 0xDEAD, 0 },
	{ "BG3PB", 0x4000032, true, false, 0xDEAD, 0 },
	{ "BG3PC", 0x4000034, true, false, 0xDEAD, 0 },
	{ "BG3PD", 0x4000036, true, false, 0xDEAD, 0 },
	{ "BG3X_LO", 0x4000038, true, false, 0xDEAD, 0 },
	{ "BG3X_HI", 0x400003A, true, false, 0xDEAD, 0 },
	{ "BG3Y_LO", 0x400003C, true, false, 0xDEAD, 0 },
	{ "BG3Y_HI", 0x400003E, true, false, 0xDEAD, 0 },
	{ "WIN0H", 0x4000040, true, false, 0xDEAD, 0 },
	{ "WIN1H", 0x4000042, true, false, 0xDEAD, 0 },
	{ "WIN0V", 0x4000044, true, false, 0xDEAD, 0 },
	{ "WIN1V", 0x4000046, true, false, 0xDEAD, 0 },
	{ "WININ", 0x4000048, true, true, 0x3F3F, 0 },
	{ "WINOUT", 0x400004A, true, true, 0x3F3F, 0 },
	{ "MOSAIC", 0x400004C, true, false, 0xDEAD, 0 },
	{ "INVALID (4E)", 0x400004E, true, false, 0xDEAD, 0 },
	{ "BLDCNT", 0x4000050, true, true, 0x3FFF, 0 },
	{ "BLDALPHA", 0x4000052, true, true, 0x1F1F, 0 },
	{ "BLDY", 0x4000054, true, true, 0xDEAD, 0 },
	{ "INVALID (56)", 0x4000056, true, false, 0xDEAD, 0 },
	{ "INVALID (58)", 0x4000058, true, false, 0xDEAD, 0 },
	{ "INVALID (5A)", 0x400005A, true, false, 0xDEAD, 0 },
	{ "INVALID (5C)", 0x400005C, true, false, 0xDEAD, 0 },
	{ "INVALID (5E)", 0x400005E, true, false, 0xDEAD, 0 },
	{ "SOUND1CNT_LO", 0x4000060, true, true, 0x007F, 0 },
	{ "SOUND1CNT_HI", 0x4000062, true, true, 0xFFC0, 0 },
	{ "SOUND1CNT_X", 0x4000064, true, true, 0x4000, 0 },
	{ "INVALID (66)", 0x4000066, true, false, 0, 0 },
	{ "SOUND2CNT_LO", 0x4000068, true, true, 0xFFC0, 0 },
	{ "SOUND2CNT_HI", 0x400006C, true, true, 0x4000, 0 },
	{ "INVALID (6E)", 0x400006E, true, false, 0, 0 },
	{ "SOUND3CNT_LO", 0x4000070, true, true, 0x00E0, 0 },
	{ "SOUND3CNT_HI", 0x4000072, true, true, 0xE000, 0 },
	{ "SOUND3CNT_X", 0x4000074, true, true, 0x4000, 0 },
	{ "INVALID (76)", 0x4000076, true, false, 0, 0 },
	{ "SOUND4CNT_LO", 0x4000078, true, true, 0xFF00, 0 },
	{ "INVALID (7A)", 0x400007A, true, false, 0, 0 },
	{ "SOUND4CNT_HI", 0x400007C, true, true, 0x40FF, 0 },
	{ "INVALID (7E)", 0x400007E, true, false, 0, 0 },
	{ "SOUNDCNT_LO", 0x4000080, true, true, 0xFF77, 0 },
	{ "SOUNDCNT_HI", 0x4000082, true, true, 0x770F, 0 },
	{ "SOUNDCNT_X", 0x4000084, true, true, 0x0080, 0x0080 },
	{ "INVALID (86)", 0x4000086, true, false, 0, 0 },
	{ "INVALID (8A)", 0x400008A, true, false, 0, 0 },
	{ "INVALID (8C)", 0x400008C, true, false, 0xDEAD, 0 },
	{ "INVALID (8E)", 0x400008E, true, false, 0xDEAD, 0 },
	{ "WAVE_RAM_0", 0x4000090, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_1", 0x4000092, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_2", 0x4000094, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_3", 0x4000096, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_4", 0x4000098, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_5", 0x400009A, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_6", 0x400009C, true, true, 0xFFFF, 0 },
	{ "WAVE_RAM_7", 0x400009E, true, true, 0xFFFF, 0 },
	{ "FIFO_A_LO", 0x40000A0, true, false, 0xDEAD, 0 },
	{ "FIFO_A_HI", 0x40000A2, true, false, 0xDEAD, 0 },
	{ "FIFO_B_LO", 0x40000A4, true, false, 0xDEAD, 0 },
	{ "FIFO_B_HI", 0x40000A6, true, false, 0xDEAD, 0 },
	{ "INVALID (A8)", 0x40000A8, true, false, 0xDEAD, 0 },
	{ "INVALID (AA)", 0x40000AA, true, false, 0xDEAD, 0 },
	{ "INVALID (AC)", 0x40000AC, true, false, 0xDEAD, 0 },
	{ "INVALID (AE)", 0x40000AE, true, false, 0xDEAD, 0 },
	{ "DMA0SAD_LO", 0x40000B0, true, false, 0xDEAD, 0 },
	{ "DMA0SAD_HI", 0x40000B2, true, false, 0xDEAD, 0 },
	{ "DMA0DAD_LO", 0x40000B4, true, false, 0xDEAD, 0 },
	{ "DMA0DAD_HI", 0x40000B6, true, false, 0xDEAD, 0 },
	{ "DMA0CNT_LO", 0x40000B8, true, true, 0, 1 },
	{ "DMA0CNT_HI", 0x40000BA, true, true, 0xF7E0, 0 },
	{ "DMA1SAD_LO", 0x40000BC, true, false, 0xDEAD, 0 },
	{ "DMA1SAD_HI", 0x40000BE, true, false, 0xDEAD, 0 },
	{ "DMA1DAD_LO", 0x40000C0, true, false, 0xDEAD, 0 },
	{ "DMA1DAD_HI", 0x40000C2, true, false, 0xDEAD, 0 },
	{ "DMA1CNT_LO", 0x40000C4, true, true, 0, 1 },
	{ "DMA1CNT_HI", 0x40000C6, true, true, 0xF7E0, 0 },
	{ "DMA2SAD_LO", 0x40000C8, true, false, 0xDEAD, 0 },
	{ "DMA2SAD_HI", 0x40000CA, true, false, 0xDEAD, 0 },
	{ "DMA2DAD_LO", 0x40000CC, true, false, 0xDEAD, 0 },
	{ "DMA2DAD_HI", 0x40000CE, true, false, 0xDEAD, 0 },
	{ "DMA2CNT_LO", 0x40000D0, true, true, 0, 1 },
	{ "DMA2CNT_HI", 0x40000D2, true, true, 0xF7E0, 0 },
	{ "DMA3SAD_LO", 0x40000D4, true, false, 0xDEAD, 0 },
	{ "DMA3SAD_HI", 0x40000D6, true, false, 0xDEAD, 0 },
	{ "DMA3DAD_LO", 0x40000D8, true, false, 0xDEAD, 0 },
	{ "DMA3DAD_HI", 0x40000DA, true, false, 0xDEAD, 0 },
	{ "DMA3CNT_LO", 0x40000DC, true, true, 0, 1 },
	{ "DMA3CNT_HI", 0x40000DE, true, true, 0xFFE0, 0 },
	{ "INVALID (E0)", 0x40000E0, true, false, 0xDEAD, 0 },
	{ "INVALID (E2)", 0x40000E2, true, false, 0xDEAD, 0 },
	{ "INVALID (E4)", 0x40000E4, true, false, 0xDEAD, 0 },
	{ "INVALID (E6)", 0x40000E6, true, false, 0xDEAD, 0 },
	{ "INVALID (E8)", 0x40000E8, true, false, 0xDEAD, 0 },
	{ "INVALID (EA)", 0x40000EA, true, false, 0xDEAD, 0 },
	{ "INVALID (EC)", 0x40000EC, true, false, 0xDEAD, 0 },
	{ "INVALID (EE)", 0x40000EE, true, false, 0xDEAD, 0 },
	{ "INVALID (F0)", 0x40000F0, true, false, 0xDEAD, 0 },
	{ "INVALID (F2)", 0x40000F2, true, false, 0xDEAD, 0 },
	{ "INVALID (F4)", 0x40000F4, true, false, 0xDEAD, 0 },
	{ "INVALID (F6)", 0x40000F6, true, false, 0xDEAD, 0 },
	{ "INVALID (F8)", 0x40000F8, true, false, 0xDEAD, 0 },
	{ "INVALID (FA)", 0x40000FA, true, false, 0xDEAD, 0 },
	{ "INVALID (FC)", 0x40000FC, true, false, 0xDEAD, 0 },
	{ "INVALID (FE)", 0x40000FE, true, false, 0xDEAD, 0 },
	{ "INVALID (100C)", 0x400100C, true, false, 0xDEAD, 0 },
};

static const u32 nTests = sizeof(ioReadTests) / sizeof(*ioReadTests);

static unsigned passes;
static unsigned totalResults;

static u16 _runTest(const struct IOReadTest* test) {
	REG_IME = 0;
	vu16* address = (vu16*) test->address;
	u16 original;
	if (test->restoreAfter) {
		original = *address;
	}
	if (test->storeFirst) {
		*address = 0xFFFF;
	}
	u16 result;
	__asm__ __volatile__ (
			"ldrh %[result], %[address] \n"
			"b 1f\n"
			".word 0xDEADDEAD \n"
			"1: \n"
		: [result]"=r"(result)
		: [address]"m"(*address)
		);
	if (test->restoreAfter) {
		*address = original;
	} else {
		*address = test->restore;
	}
	REG_IME = 1;
	return result;
}

static void printResults(int offset, int line, const struct IOReadTest* test) {
	static const int base = GRID_STRIDE * 3;
	if (offset > line * 2 || base + GRID_STRIDE * (line * 2 - offset + 1) > 576) {
		return;
	}

	snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset)], 31, "%s:", test->testName);
	u16 value = _runTest(test);
	if (value == test->expected) {
		snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], 28, "%04X PASS", value);
	} else {
		snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], 28, "%04X != %04X", value, test->expected);
	}
}

static void doResult(const struct IOReadTest* test) {
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

static size_t listIOReadSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = ioReadTests[i + offset].testName;
	}
	return i;
}

static void runIOReadSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct IOReadTest* activeTest = 0;
	REG_SOUNDCNT_X = 0x80;
	int i;
	for (i = 0; i < nTests; ++i) {
		activeTest = &ioReadTests[i];
		doResult(activeTest);
	}
	REG_SOUNDCNT_X = 0;
}

static void showIOReadSuite(size_t index) {
	const struct IOReadTest* activeTest = &ioReadTests[index];
	size_t resultIndex = 0;
	REG_SOUNDCNT_X = 0x80;
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
	REG_SOUNDCNT_X = 0;
}

const struct TestSuite ioReadTestSuite = {
	.name = "I/O read tests",
	.run = runIOReadSuite,
	.list = listIOReadSuite,
	.show = showIOReadSuite,
	.nTests = sizeof(ioReadTests) / sizeof(*ioReadTests),
	.passes = &passes,
	.totalResults = &totalResults
};
