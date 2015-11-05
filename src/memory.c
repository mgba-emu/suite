#include "memory.h"

#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

static const u32 r8[2] = {
	0xDEADBEEF, 0x33333333
};

static EWRAM_DATA u32 r2[2] = {
	0xFEEDFACE, 0x33333333
};

static IWRAM_DATA u32 r3[2] = {
	0xCAFEBABE, 0x33333333
};

struct TestConfigurations {
	u32 _u8;
	u32 _s8;
	u32 _u16;
	u32 _u16u1;
	u32 _s16;
	u32 _s16u1;
	u32 _u32;
	u32 _u32u1;
	u32 _u32u2;
	u32 _u32u3;
	u32 _d0_16;
	u32 _d0_16u1;
	u32 _d0_32;
	u32 _d0_32u1;
	u32 _d0_32u2;
	u32 _d0_32u3;
	u32 _d1_16;
	u32 _d1_16u1;
	u32 _d1_32;
	u32 _d1_32u1;
	u32 _d1_32u2;
	u32 _d1_32u3;
	u32 _d2_16;
	u32 _d2_16u1;
	u32 _d2_32;
	u32 _d2_32u1;
	u32 _d2_32u2;
	u32 _d2_32u3;
	u32 _d3_16;
	u32 _d3_16u1;
	u32 _d3_32;
	u32 _d3_32u1;
	u32 _d3_32u2;
	u32 _d3_32u3;
	u32 _c16[8];
	u32 _c16u1[8];
	u32 _c32[8];
	u32 _c32u1[8];
	u32 _c32u2[8];
	u32 _c32u3[8];
	u32 _cf32[8];
	u32 _cf32u1[8];
	u32 _cf32u2[8];
	u32 _cf32u3[8];
};

struct MemoryTest {
	const char* testName;
	void (*test)(struct TestConfigurations*);
	struct TestConfigurations expected;
};

__attribute__((noinline))
static void testLoad(struct TestConfigurations* config, u8* base) {
	__asm__ __volatile__(
			"mov r1, %[i] \n"
			"ldr r0, =enter \n"
			"bx r0 \n"
			".arm; enter:\n"
			"ldrb   %[u8],    [r1] \n"
			"ldrsb  %[s8],    [r1] \n"
			"ldrh   %[u16],   [r1] \n"
			"ldrh   %[u16u1], [r1, #1] \n"
			"ldrsh  %[s16],   [r1] \n"
			"ldrsh  %[s16u1], [r1, #1] \n"
			"ldr    %[u32],   [r1] \n"
			"ldr    %[u32u1], [r1, #1] \n"
			"ldr    %[u32u2], [r1, #2] \n"
			"ldr    %[u32u3], [r1, #3] \n"
			"ldr r0, =exit \n"
			"add r0, #1 \n"
			"bx r0 \n"
			".ltorg \n"
			".thumb; exit:"
		:
			[u8]"=r"(config->_u8),
			[s8]"=r"(config->_s8),
			[u16]"=r"(config->_u16),
			[u16u1]"=r"(config->_u16u1),
			[s16]"=r"(config->_s16),
			[s16u1]"=r"(config->_s16u1),
			[u32]"=r"(config->_u32),
			[u32u1]"=r"(config->_u32u1),
			[u32u2]"=r"(config->_u32u2),
			[u32u3]"=r"(config->_u32u3)
		: [i]"r"(base)
		: "r0", "r1");
	// Clear stored DMA state
	DMA0COPY(r2, &config->_d0_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(r2, &config->_d1_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(r2, &config->_d2_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(r2, &config->_d3_32u1, DMA32 | DMA_IMMEDIATE | 1);

	DMA0COPY(base, &config->_d0_16, DMA16 | DMA_IMMEDIATE | 1);
	DMA0COPY(base + 1, &config->_d0_16u1, DMA16 | DMA_IMMEDIATE | 1);
	DMA0COPY(base, &config->_d0_32, DMA32 | DMA_IMMEDIATE | 1);
	DMA0COPY(base + 1, &config->_d0_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA0COPY(base + 2, &config->_d0_32u2, DMA32 | DMA_IMMEDIATE | 1);
	DMA0COPY(base + 3, &config->_d0_32u3, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(base, &config->_d1_16, DMA16 | DMA_IMMEDIATE | 1);
	DMA1COPY(base + 1, &config->_d1_16u1, DMA16 | DMA_IMMEDIATE | 1);
	DMA1COPY(base, &config->_d1_32, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(base + 1, &config->_d1_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(base + 2, &config->_d1_32u2, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(base + 3, &config->_d1_32u3, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(base, &config->_d2_16, DMA16 | DMA_IMMEDIATE | 1);
	DMA2COPY(base + 1, &config->_d2_16u1, DMA16 | DMA_IMMEDIATE | 1);
	DMA2COPY(base, &config->_d2_32, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(base + 1, &config->_d2_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(base + 2, &config->_d2_32u2, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(base + 3, &config->_d2_32u3, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(base, &config->_d3_16, DMA16 | DMA_IMMEDIATE | 1);
	DMA3COPY(base + 1, &config->_d3_16u1, DMA16 | DMA_IMMEDIATE | 1);
	DMA3COPY(base, &config->_d3_32, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(base + 1, &config->_d3_32u1, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(base + 2, &config->_d3_32u2, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(base + 3, &config->_d3_32u3, DMA32 | DMA_IMMEDIATE | 1);
	CpuSet(base, config->_c16, 8 | BIT(26));
	CpuSet(base + 1, config->_c16u1, 8 | BIT(26));
	CpuSet(base, config->_c32, 8);
	CpuSet(base + 1, config->_c32u1, 8);
	CpuSet(base + 2, config->_c32u2, 8);
	CpuSet(base + 3, config->_c32u3, 8);
	CpuFastSet(base, config->_cf32, 8);
	CpuFastSet(base + 1, config->_cf32u1, 8);
	CpuFastSet(base + 2, config->_cf32u2, 8);
	CpuFastSet(base + 3, config->_cf32u3, 8);
}

static void testLoadEWRAM(struct TestConfigurations* config) {
	testLoad(config, (u8*) r2);
}

static void testLoadEWRAMMirror(struct TestConfigurations* config) {
	testLoad(config, (u8*) ((u32) r2 | 0x00800000));
}

static void testLoadIWRAM(struct TestConfigurations* config) {
	testLoad(config, (u8*) r3);
}

static void testLoadIWRAMMirror(struct TestConfigurations* config) {
	testLoad(config, (u8*) ((u32) r3 | 0x00800000));
}

static void testLoadROM(struct TestConfigurations* config) {
	testLoad(config, (u8*) r8);
}

static void testLoadROMBad(struct TestConfigurations* config) {
	testLoad(config, (u8*) 0x092468AC);
}

static void testLoadBIOS(struct TestConfigurations* config) {
	REG_IME = 0;
	VBlankIntrWait();
	testLoad(config, (u8*) 0);
	REG_IME = 1;
}

static void testLoadBIOSBad(struct TestConfigurations* config) {
	testLoad(config, (u8*) 0x10000);
}

static void testLoadBad(struct TestConfigurations* config) {
	testLoad(config, (u8*) 0x01000000);
}

static const struct MemoryTest memoryTests[] = {
	{ "ROM load", testLoadROM, {
		0xEF, 0xFFFFFFEF, 0xBEEF, 0xEF0000BE, 0xFFFFBEEF, 0xFFFFFFBE, 0xDEADBEEF, 0xEFDEADBE, 0xBEEFDEAD, 0xADBEEFDE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		{ 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDE00BE }, { 0x3333DEAD }, { 0x003300DE },
		{ 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }
	}},
	{ "ROM out-of-bounds load", testLoadROMBad, {
		0x14, 0x14, 0x2314, 0x14000023, 0x2314, 0x23, 0x5112314, 0x14051123, 0x23140511, 0x11231405,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0x2314, 0x2314, 0x5112314, 0x5112314, 0x5112314, 0x5112314,
		0x2314, 0x2314, 0x5112314, 0x5112314, 0x5112314, 0x5112314,
		0x2314, 0x2314, 0x5112314, 0x5112314, 0x5112314, 0x5112314,
		{ 0x5112314 }, { 0x5112314 }, { 0x5112314 }, { 0x50023 }, { 0x20160511 }, { 0x200005 },
		{ 0x5112314 }, { 0x5112314 }, { 0x5112314 }, { 0x5112314 }

	}},
	{ "IWRAM load", testLoadIWRAM, {
		0xBE, 0xFFFFFFBE, 0xBABE, 0xBE0000BA, 0xFFFFBABE, 0xFFFFFFBA, 0xCAFEBABE, 0xBECAFEBA, 0xBABECAFE, 0xFEBABECA,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCA00BA }, { 0x3333CAFE }, { 0x003300CA },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
	}},
	{ "IWRAM mirror load", testLoadIWRAMMirror, {
		0xBE, 0xFFFFFFBE, 0xBABE, 0xBE0000BA, 0xFFFFBABE, 0xFFFFFFBA, 0xCAFEBABE, 0xBECAFEBA, 0xBABECAFE, 0xFEBABECA,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCA00BA }, { 0x3333CAFE }, { 0x003300CA },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
		
	}},
	{ "EWRAM load", testLoadEWRAM, {
		0xCE, 0xFFFFFFCE, 0xFACE, 0xCE0000FA, 0xFFFFFACE, 0xFFFFFFFA, 0xFEEDFACE, 0xCEFEEDFA, 0xFACEFEED, 0xEDFACEFE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFE00FA }, { 0x3333FEED }, { 0x003300FE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "EWRAM mirror load", testLoadEWRAMMirror, {
		0xCE, 0xFFFFFFCE, 0xFACE, 0xCE0000FA, 0xFFFFFACE, 0xFFFFFFFA, 0xFEEDFACE, 0xCEFEEDFA, 0xFACEFEED, 0xEDFACEFE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFE00FA }, { 0x3333FEED }, { 0x003300FE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "BIOS load", testLoadBIOS, {
		0x4, 0x4, 0x2004, 0x04000020, 0x2004, 0x20, 0xE3A02004, 0x4E3A020, 0x2004E3A0, 0xA02004E3,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
		{ 0 }, { 0 }, { 0 }, { 0 }
	}},
	{ "BIOS out-of-bounds load", testLoadBIOSBad, {
		0xB0, 0xFFFFFFB1, 0xC0F0, 0xF1000070, 0x6000, 0x50, 0xE5912002, 0x3E59130, 0x8E59F, 0x800001E2,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
		{ 0 }, { 0 }, { 0 }, { 0 }
	}},
	{ "Out-of-bounds load", testLoadBad, {
		0xB0, 0xFFFFFFB1, 0xC0F0, 0xF1000070, 0x6000, 0x50, 0xE5912002, 0x3E59130, 0x8E59F, 0x800001E2,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
		{ 0 }, { 0 }, { 0 }, { 0 }
	}},
};

static const u32 nTests = sizeof(memoryTests) / sizeof(*memoryTests);

static unsigned passes;
static unsigned totalResults;

static void printResult(int offset, int line, const char* preface, u32 value, u32 expected) {
	static const int base = GRID_STRIDE * 3;
	if (offset > line * 2 || base + GRID_STRIDE * (line * 2 - offset + 1) > 576) {
		return;
	}

	snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset)], 31, "%s:", preface);
	if (value == expected) {
		strncpy(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], "PASS", 10);
	} else {
		snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], 28, "%08X != %08X", value, expected);
	}
}

static void printResults(const char* preface, const struct TestConfigurations* values,  const struct TestConfigurations* expected, int base) {
	snprintf(&textGrid[GRID_STRIDE], 31, "Memory test: %s", preface);

	printResult(base,  0, "U8", values->_u8, expected->_u8);
	printResult(base,  1, "S8", values->_s8, expected->_s8);
	printResult(base,  2, "U16", values->_u16, expected->_u16);
	printResult(base,  3, "U16 (unaligned)", values->_u16u1, expected->_u16u1);
	printResult(base,  4, "S16", values->_s16, expected->_s16);
	printResult(base,  5, "S16 (unaligned)", values->_s16u1, expected->_s16u1);
	printResult(base,  6, "32", values->_u32, expected->_u32);
	printResult(base,  7, "32 (unaligned 1)", values->_u32u1, expected->_u32u1);
	printResult(base,  8, "32 (unaligned 2)", values->_u32u2, expected->_u32u2);
	printResult(base,  9, "32 (unaligned 3)", values->_u32u3, expected->_u32u3);
	printResult(base, 10, "DMA0 16", values->_d0_16, expected->_d0_16);
	printResult(base, 11, "DMA0 16 (unaligned 1)", values->_d0_16u1, expected->_d0_16u1);
	printResult(base, 12, "DMA0 32", values->_d0_32, expected->_d0_32);
	printResult(base, 13, "DMA0 32 (unaligned 1)", values->_d0_32u1, expected->_d0_32u1);
	printResult(base, 14, "DMA0 32 (unaligned 2)", values->_d0_32u2, expected->_d0_32u2);
	printResult(base, 15, "DMA0 32 (unaligned 3)", values->_d0_32u3, expected->_d0_32u3);
	printResult(base, 16, "DMA1 16", values->_d1_16, expected->_d1_16);
	printResult(base, 17, "DMA1 16 (unaligned 1)", values->_d1_16u1, expected->_d1_16u1);
	printResult(base, 18, "DMA1 32", values->_d1_32, expected->_d1_32);
	printResult(base, 19, "DMA1 32 (unaligned 1)", values->_d1_32u1, expected->_d1_32u1);
	printResult(base, 20, "DMA1 32 (unaligned 2)", values->_d1_32u2, expected->_d1_32u2);
	printResult(base, 21, "DMA1 32 (unaligned 3)", values->_d1_32u3, expected->_d1_32u3);
	printResult(base, 22, "DMA2 16", values->_d2_16, expected->_d2_16);
	printResult(base, 23, "DMA2 16 (unaligned 1)", values->_d2_16u1, expected->_d2_16u1);
	printResult(base, 24, "DMA2 32", values->_d2_32, expected->_d2_32);
	printResult(base, 25, "DMA2 32 (unaligned 1)", values->_d2_32u1, expected->_d2_32u1);
	printResult(base, 26, "DMA2 32 (unaligned 2)", values->_d2_32u2, expected->_d2_32u2);
	printResult(base, 27, "DMA2 32 (unaligned 3)", values->_d2_32u3, expected->_d2_32u3);
	printResult(base, 28, "DMA3 16", values->_d3_16, expected->_d3_16);
	printResult(base, 29, "DMA3 16 (unaligned 1)", values->_d3_16u1, expected->_d3_16u1);
	printResult(base, 30, "DMA3 32", values->_d3_32, expected->_d3_32);
	printResult(base, 31, "DMA3 32 (unaligned 1)", values->_d3_32u1, expected->_d3_32u1);
	printResult(base, 32, "DMA3 32 (unaligned 2)", values->_d3_32u2, expected->_d3_32u2);
	printResult(base, 33, "DMA3 32 (unaligned 3)", values->_d3_32u3, expected->_d3_32u3);
	printResult(base, 34, "swi B 16", values->_c16[0], expected->_c16[0]);
	printResult(base, 35, "swi B 16 (unaligned 1)", values->_c16u1[0], expected->_c16u1[0]);
	printResult(base, 36, "swi B 32", values->_c16[0], expected->_c32[0]);
	printResult(base, 37, "swi B 32 (unaligned 1)", values->_c32u1[0], expected->_c32u1[0]);
	printResult(base, 38, "swi B 32 (unaligned 2)", values->_c32u2[0], expected->_c32u2[0]);
	printResult(base, 39, "swi B 32 (unaligned 3)", values->_c32u3[0], expected->_c32u3[0]);
	printResult(base, 40, "swi C 32", values->_cf32[0], expected->_cf32[0]);
	printResult(base, 41, "swi C 32 (unaligned 1)", values->_cf32u1[0], expected->_cf32u1[0]);
	printResult(base, 42, "swi C 32 (unaligned 2)", values->_cf32u2[0], expected->_cf32u2[0]);
	printResult(base, 43, "swi C 32 (unaligned 3)", values->_cf32u3[0], expected->_cf32u3[0]);
}

static size_t listMemorySuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = memoryTests[i + offset].testName;
	}
	return i;
}

static void runMemorySuite(void) {
	passes = 0;
	totalResults = 0;
	const struct MemoryTest* activeTest = 0;
	int i;
	for (i = 0; i < nTests; ++i) {
		struct TestConfigurations currentTest = {0};
		VBlankIntrWait();
		REG_IME = 0;
		activeTest = &memoryTests[i];
		activeTest->test(&currentTest);
		REG_IME = 1;
		if (activeTest->expected._u8 == currentTest._u8) {
			++passes;
		}
		if (activeTest->expected._s8 == currentTest._s8) {
			++passes;
		}
		if (activeTest->expected._u16 == currentTest._u16) {
			++passes;
		}
		if (activeTest->expected._u16u1 == currentTest._u16u1) {
			++passes;
		}
		if (activeTest->expected._s16 == currentTest._s16) {
			++passes;
		}
		if (activeTest->expected._s16u1 == currentTest._s16u1) {
			++passes;
		}
		if (activeTest->expected._u32 == currentTest._u32) {
			++passes;
		}
		if (activeTest->expected._u32u1 == currentTest._u32u1) {
			++passes;
		}
		if (activeTest->expected._u32u2 == currentTest._u32u2) {
			++passes;
		}
		if (activeTest->expected._u32u3 == currentTest._u32u3) {
			++passes;
		}
		if (activeTest->expected._d0_16 == currentTest._d0_16) {
			++passes;
		}
		if (activeTest->expected._d0_16u1 == currentTest._d0_16u1) {
			++passes;
		}
		if (activeTest->expected._d0_32 == currentTest._d0_32) {
			++passes;
		}
		if (activeTest->expected._d0_32u1 == currentTest._d0_32u1) {
			++passes;
		}
		if (activeTest->expected._d0_32u2 == currentTest._d0_32u2) {
			++passes;
		}
		if (activeTest->expected._d0_32u3 == currentTest._d0_32u3) {
			++passes;
		}
		if (activeTest->expected._d1_16 == currentTest._d1_16) {
			++passes;
		}
		if (activeTest->expected._d1_16u1 == currentTest._d1_16u1) {
			++passes;
		}
		if (activeTest->expected._d1_32 == currentTest._d1_32) {
			++passes;
		}
		if (activeTest->expected._d1_32u1 == currentTest._d1_32u1) {
			++passes;
		}
		if (activeTest->expected._d1_32u2 == currentTest._d1_32u2) {
			++passes;
		}
		if (activeTest->expected._d1_32u3 == currentTest._d1_32u3) {
			++passes;
		}
		if (activeTest->expected._d2_16 == currentTest._d2_16) {
			++passes;
		}
		if (activeTest->expected._d2_16u1 == currentTest._d2_16u1) {
			++passes;
		}
		if (activeTest->expected._d2_32 == currentTest._d2_32) {
			++passes;
		}
		if (activeTest->expected._d2_32u1 == currentTest._d2_32u1) {
			++passes;
		}
		if (activeTest->expected._d2_32u2 == currentTest._d2_32u2) {
			++passes;
		}
		if (activeTest->expected._d2_32u3 == currentTest._d2_32u3) {
			++passes;
		}
		if (activeTest->expected._d3_16 == currentTest._d3_16) {
			++passes;
		}
		if (activeTest->expected._d3_16u1 == currentTest._d3_16u1) {
			++passes;
		}
		if (activeTest->expected._d3_32 == currentTest._d3_32) {
			++passes;
		}
		if (activeTest->expected._d3_32u1 == currentTest._d3_32u1) {
			++passes;
		}
		if (activeTest->expected._d3_32u2 == currentTest._d3_32u2) {
			++passes;
		}
		if (activeTest->expected._d3_32u3 == currentTest._d3_32u3) {
			++passes;
		}
		if (activeTest->expected._c16[0] == currentTest._c16[0]) {
			++passes;
		}
		if (activeTest->expected._c16u1[0] == currentTest._c16u1[0]) {
			++passes;
		}
		if (activeTest->expected._c32[0] == currentTest._c32[0]) {
			++passes;
		}
		if (activeTest->expected._c32u1[0] == currentTest._c32u1[0]) {
			++passes;
		}
		if (activeTest->expected._c32u2[0] == currentTest._c32u2[0]) {
			++passes;
		}
		if (activeTest->expected._c32u3[0] == currentTest._c32u3[0]) {
			++passes;
		}
		if (activeTest->expected._cf32[0] == currentTest._cf32[0]) {
			++passes;
		}
		if (activeTest->expected._cf32u1[0] == currentTest._cf32u1[0]) {
			++passes;
		}
		if (activeTest->expected._cf32u2[0] == currentTest._cf32u2[0]) {
			++passes;
		}
		if (activeTest->expected._cf32u3[0] == currentTest._cf32u3[0]) {
			++passes;
		}
		totalResults += 44;
	}
}

static void showMemorySuite(size_t index) {
	const struct MemoryTest* activeTest = &memoryTests[index];
	struct TestConfigurations currentTest = {0};
	size_t resultIndex = 0;
	while (1) {
		memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDown();

		if (keys & KEY_B) {
			return;
		}

		if (keys & KEY_UP) {
			if (resultIndex > 0) {
				--resultIndex;
			}
		}
		if (keys & KEY_DOWN) {
			if (resultIndex < 72) {
				++resultIndex;
			}
		}
		activeTest->test(&currentTest);
		printResults(activeTest->testName, &currentTest, &activeTest->expected, resultIndex);
		updateTextGrid();
	}
}

const struct TestSuite memoryTestSuite = {
	.name = "Memory tests",
	.run = runMemorySuite,
	.list = listMemorySuite,
	.show = showMemorySuite,
	.nTests = sizeof(memoryTests) / sizeof(*memoryTests),
	.passes = &passes,
	.totalResults = &totalResults
};
