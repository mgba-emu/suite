#include "memory.h"

#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

static const u32 r8[8] = {
	0xDEADBEEF, 0x33333333
};

static EWRAM_DATA u32 r2[8] = {
	0xFEEDFACE, 0x33333333
};

static IWRAM_DATA u32 r3[8] = {
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
	bool store;
	struct TestConfigurations expected;
};

__attribute__((noinline))
static void testLoad(struct TestConfigurations* config, u8* base) {
	__asm__ __volatile__(
			"ldr r0, =enterL \n"
			"bx r0 \n"
			".arm; enterL:\n"
			"mov r0, %[i] \n"
			"ldrb   %[u8],    [r0] \n"
			"ldrsb  %[s8],    [r0] \n"
			"ldrh   %[u16],   [r0] \n"
			"ldrh   %[u16u1], [r0, #1] \n"
			"ldrsh  %[s16],   [r0] \n"
			"ldrsh  %[s16u1], [r0, #1] \n"
			"ldr    %[u32],   [r0] \n"
			"ldr    %[u32u1], [r0, #1] \n"
			"ldr    %[u32u2], [r0, #2] \n"
			"ldr    %[u32u3], [r0, #3] \n"
			"ldr r0, =exitL \n"
			"add r0, #1 \n"
			"bx r0 \n"
			".ltorg \n"
			".thumb; exitL:"
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
		: "r0");
	// Clear stored DMA state
	u32 temp;
	DMA0COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);

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
	CpuSet(base, config->_c16, 8);
	CpuSet(base + 1, config->_c16u1, 8);
	CpuSet(base, config->_c32, 8 | BIT(26));
	CpuSet(base + 1, config->_c32u1, 8 | BIT(26));
	CpuSet(base + 2, config->_c32u2, 8 | BIT(26));
	CpuSet(base + 3, config->_c32u3, 8 | BIT(26));
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

static void testLoadSRAM(struct TestConfigurations* config) {
	testLoad(config, (u8*) SRAM);
}

static void testLoadSRAMMirror(struct TestConfigurations* config) {
	testLoad(config, (u8*) (SRAM | 0x01000000));
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

static void testLoadVRAM(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x0600FFE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testLoad(config, (u8*) base);
}

static void testLoadVRAMMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x0600FFE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x0602FFE0;
	testLoad(config, (u8*) base);
}

static void testLoadVRAM2(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testLoad(config, (u8*) base);
}

static void testLoadVRAM2Mirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x0601FFE0;
	testLoad(config, (u8*) base);
}

static void testLoadVRAM2Mirror2(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x06037FE0;
	testLoad(config, (u8*) base);
}

static void testLoadPalette(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x050003E0;
	base[0] = 0x1DEA;
	base[1] = 0xABAD;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testLoad(config, (u8*) base);
}

static void testLoadPaletteMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x050003E0;
	base[0] = 0x1DEA;
	base[1] = 0xABAD;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x050007E0;
	testLoad(config, (u8*) base);
}

static void testLoadOAM(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x070003E0;
	base[0] = 0xA5ED;
	base[1] = 0xDECE;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testLoad(config, (u8*) base);
	base[0] = 0x0200;
	base[1] = 0x0000;
	base[2] = 0x0000;
	base[3] = 0x0000;
}

static void testLoadOAMMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x070003E0;
	base[0] = 0xA5ED;
	base[1] = 0xDECE;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x070007E0;
	testLoad(config, (u8*) base);
	base[0] = 0x0200;
	base[1] = 0x0000;
	base[2] = 0x0000;
	base[3] = 0x0000;
}

static void testLoadBIOSBad(struct TestConfigurations* config) {
	testLoad(config, (u8*) 0x10000);
}

static void testLoadBad(struct TestConfigurations* config) {
	testLoad(config, (u8*) 0x01000000);
}

__attribute__((noinline))
static void testStore(struct TestConfigurations* config, u32* base) {
	const u32 fodder[2] = { base[0], base[1] };
	base[0] = 0;
	base[1] = 0xBBBBBBBB;

	__asm__ __volatile__(
			"ldr r0, =enterS \n"
			"bx r0 \n"
			".arm; enterS:\n"
			"mov  r1, %[i] \n"
			"mov  r0, %[o] \n"
			"mov  r3, #0 \n"
			"strb r0, [r1] \n"
			"ldrb r2, [r1] \n"
			"str  r2, %[u8] \n"
			"ldrh r2, [r1] \n"
			"str  r2, %[s8] \n"
			"str  r3, [r1] \n"
			"strh r0, [r1] \n"
			"ldrh r2, [r1] \n"
			"str  r2, %[u16] \n"
			"strh r0, [r1, #1] \n"
			"ldrh r2, [r1] \n"
			"str  r3, [r1] \n"
			"str  r2, %[u16u1] \n"
			"str  r0, [r1] \n"
			"ldr  r2, [r1] \n"
			"str  r3, [r1] \n"
			"str  r2, %[u32] \n"
			"str  r0, [r1, #1] \n"
			"ldr  r2, [r1] \n"
			"str  r3, [r1] \n"
			"str  r2, %[u32u1] \n"
			"str  r0, [r1, #2] \n"
			"ldr  r2, [r1] \n"
			"str  r3, [r1] \n"
			"str  r2, %[u32u2] \n"
			"str  r0, [r1, #3] \n"
			"ldr  r2, [r1] \n"
			"str  r3, [r1] \n"
			"str  r2, %[u32u3] \n"
			"ldr  r0, =exitS \n"
			"add  r0, #1 \n"
			"bx r0 \n"
			".ltorg \n"
			".thumb; exitS:"
		:
			[u8]"=m"(config->_u8),
			[s8]"=m"(config->_s8),
			[u16]"=m"(config->_u16),
			[u16u1]"=m"(config->_u16u1),
			[u32]"=m"(config->_u32),
			[u32u1]"=m"(config->_u32u1),
			[u32u2]"=m"(config->_u32u2),
			[u32u3]"=m"(config->_u32u3)
		: [i]"r"(base), [o]"r"(fodder[0])
		: "r0", "r1", "r2", "r3");
	// Clear stored DMA state
	u32 temp;
	DMA0COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA1COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA2COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);
	DMA3COPY(r2, &temp, DMA32 | DMA_IMMEDIATE | 1);

	DMA0COPY(fodder, base, DMA16 | DMA_IMMEDIATE | 1);
	config->_d0_16 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA0COPY(fodder, (u8*) base + 1, DMA16 | DMA_IMMEDIATE | 1);
	config->_d0_16u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA0COPY(fodder, base, DMA32 | DMA_IMMEDIATE | 1);
	config->_d0_32 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA0COPY(fodder, (u8*) base + 1, DMA32 | DMA_IMMEDIATE | 1);
	config->_d0_32u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA0COPY(fodder, (u8*) base + 2, DMA32 | DMA_IMMEDIATE | 1);
	config->_d0_32u2 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA0COPY(fodder, (u8*) base + 3, DMA32 | DMA_IMMEDIATE | 1);
	config->_d0_32u3 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, base, DMA16 | DMA_IMMEDIATE | 1);
	config->_d1_16 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, (u8*) base + 1, DMA16 | DMA_IMMEDIATE | 1);
	config->_d1_16u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, base, DMA32 | DMA_IMMEDIATE | 1);
	config->_d1_32 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, (u8*) base + 1, DMA32 | DMA_IMMEDIATE | 1);
	config->_d1_32u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, (u8*) base + 2, DMA32 | DMA_IMMEDIATE | 1);
	config->_d1_32u2 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA1COPY(fodder, (u8*) base + 3, DMA32 | DMA_IMMEDIATE | 1);
	config->_d1_32u3 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, base, DMA16 | DMA_IMMEDIATE | 1);
	config->_d2_16 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, (u8*) base + 1, DMA16 | DMA_IMMEDIATE | 1);
	config->_d2_16u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, base, DMA32 | DMA_IMMEDIATE | 1);
	config->_d2_32 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, (u8*) base + 1, DMA32 | DMA_IMMEDIATE | 1);
	config->_d2_32u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, (u8*) base + 2, DMA32 | DMA_IMMEDIATE | 1);
	config->_d2_32u2 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA2COPY(fodder, (u8*) base + 3, DMA32 | DMA_IMMEDIATE | 1);
	config->_d2_32u3 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, base, DMA16 | DMA_IMMEDIATE | 1);
	config->_d3_16 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, (u8*) base + 1, DMA16 | DMA_IMMEDIATE | 1);
	config->_d3_16u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, base, DMA32 | DMA_IMMEDIATE | 1);
	config->_d3_32 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, (u8*) base + 1, DMA32 | DMA_IMMEDIATE | 1);
	config->_d3_32u1 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, (u8*) base + 2, DMA32 | DMA_IMMEDIATE | 1);
	config->_d3_32u2 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	DMA3COPY(fodder, (u8*) base + 3, DMA32 | DMA_IMMEDIATE | 1);
	config->_d3_32u3 = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, base, 8);
	config->_c16[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, base + 1, 8);
	config->_c16u1[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, base, 8 | BIT(26));
	config->_c32[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, (u8*) base + 1, 8 | BIT(26));
	config->_c32u1[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, (u8*) base + 2, 8 | BIT(26));
	config->_c32u2[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuSet(fodder, (u8*) base + 3, 8 | BIT(26));
	config->_c32u3[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuFastSet(fodder, base, 8);
	config->_cf32[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuFastSet(fodder, (u8*) base + 1, 8);
	config->_cf32u1[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuFastSet(fodder, (u8*) base + 2, 8);
	config->_cf32u2[0] = base[0];
	base[0] = 0;
	base[1] = 0xBBBBBBBB;
	CpuFastSet(fodder, (u8*) base + 3, 8);
	config->_cf32u3[0] = base[0];

	base[0] = fodder[0];
	base[1] = fodder[1];
}

static void testStoreEWRAM(struct TestConfigurations* config) {
	testStore(config, (u32*) r2);
}

static void testStoreEWRAMMirror(struct TestConfigurations* config) {
	testStore(config, (u32*) ((u32) r2 | 0x00800000));
}

static void testStoreIWRAM(struct TestConfigurations* config) {
	testStore(config, (u32*) r3);
}

static void testStoreIWRAMMirror(struct TestConfigurations* config) {
	testStore(config, (u32*) ((u32) r3 | 0x00800000));
}

static void testStoreROM(struct TestConfigurations* config) {
	testStore(config, (u32*) r8);
}

static void testStoreSRAM(struct TestConfigurations* config) {
	testStore(config, (u32*) SRAM);
	((u8*) SRAM)[ 0] = 'G';
	((u8*) SRAM)[ 1] = 'a';
	((u8*) SRAM)[ 2] = 'm';
	((u8*) SRAM)[ 3] = 'e';
	((u8*) SRAM)[ 4] = ' ';
	((u8*) SRAM)[ 5] = 'B';
	((u8*) SRAM)[ 6] = 'o';
	((u8*) SRAM)[ 7] = 'y';
	((u8*) SRAM)[ 8] = ' ';
	((u8*) SRAM)[ 9] = 'A';
	((u8*) SRAM)[10] = 'd';
	((u8*) SRAM)[11] = 'v';
	((u8*) SRAM)[12] = 'a';
	((u8*) SRAM)[13] = 'n';
	((u8*) SRAM)[14] = 'c';
	((u8*) SRAM)[15] = 'e';
	((u8*) SRAM)[16] = ' ';
	((u8*) SRAM)[17] = 'T';
	((u8*) SRAM)[18] = 'e';
	((u8*) SRAM)[19] = 's';
	((u8*) SRAM)[20] = 't';
	((u8*) SRAM)[21] = ' ';
	((u8*) SRAM)[22] = 'S';
	((u8*) SRAM)[23] = 'u';
	((u8*) SRAM)[24] = 'i';
	((u8*) SRAM)[25] = 't';
	((u8*) SRAM)[26] = 'e';
	((u8*) SRAM)[27] = '\n';
	((u8*) SRAM)[28] = '=';
	((u8*) SRAM)[29] = '=';
	((u8*) SRAM)[30] = '=';
	((u8*) SRAM)[31] = '\n';
}

static void testStoreSRAMMirror(struct TestConfigurations* config) {
	testStore(config, (u32*) (SRAM | 0x01000000));
	((u8*) SRAM)[ 0] = 'G';
	((u8*) SRAM)[ 1] = 'a';
	((u8*) SRAM)[ 2] = 'm';
	((u8*) SRAM)[ 3] = 'e';
	((u8*) SRAM)[ 4] = ' ';
	((u8*) SRAM)[ 5] = 'B';
	((u8*) SRAM)[ 6] = 'o';
	((u8*) SRAM)[ 7] = 'y';
	((u8*) SRAM)[ 8] = ' ';
	((u8*) SRAM)[ 9] = 'A';
	((u8*) SRAM)[10] = 'd';
	((u8*) SRAM)[11] = 'v';
	((u8*) SRAM)[12] = 'a';
	((u8*) SRAM)[13] = 'n';
	((u8*) SRAM)[14] = 'c';
	((u8*) SRAM)[15] = 'e';
	((u8*) SRAM)[16] = ' ';
	((u8*) SRAM)[17] = 'T';
	((u8*) SRAM)[18] = 'e';
	((u8*) SRAM)[19] = 's';
	((u8*) SRAM)[20] = 't';
	((u8*) SRAM)[21] = ' ';
	((u8*) SRAM)[22] = 'S';
	((u8*) SRAM)[23] = 'u';
	((u8*) SRAM)[24] = 'i';
	((u8*) SRAM)[25] = 't';
	((u8*) SRAM)[26] = 'e';
	((u8*) SRAM)[27] = '\n';
	((u8*) SRAM)[28] = '=';
	((u8*) SRAM)[29] = '=';
	((u8*) SRAM)[30] = '=';
	((u8*) SRAM)[31] = '\n';
}

static void testStoreVRAM(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16 dispcnt = REG_DISPCNT;
	REG_DISPCNT = LCDC_OFF;
	u16* base = (u16*) 0x0600FFE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testStore(config, (u32*) base);
	REG_DISPCNT = dispcnt;
}

static void testStoreVRAMMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16 dispcnt = REG_DISPCNT;
	REG_DISPCNT = LCDC_OFF;
	u16* base = (u16*) 0x0600FFE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x0602FFE0;
	testStore(config, (u32*) base);
	REG_DISPCNT = dispcnt;
}

static void testStoreVRAM2(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16 dispcnt = REG_DISPCNT;
	REG_DISPCNT = LCDC_OFF;
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testStore(config, (u32*) base);
	REG_DISPCNT = dispcnt;
}

static void testStoreVRAM2Mirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16 dispcnt = REG_DISPCNT;
	REG_DISPCNT = LCDC_OFF;
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x0601FFE0;
	testStore(config, (u32*) base);
	REG_DISPCNT = dispcnt;
}

static void testStoreVRAM2Mirror2(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16 dispcnt = REG_DISPCNT;
	REG_DISPCNT = LCDC_OFF;
	u16* base = (u16*) 0x06017FE0;
	base[0] = 0xF00D;
	base[1] = 0x900D;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x06037FE0;
	testStore(config, (u32*) base);
	REG_DISPCNT = dispcnt;
}

static void testStorePalette(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x050003E0;
	base[0] = 0x1DEA;
	base[1] = 0xABAD;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testStore(config, (u32*) base);
}

static void testStorePaletteMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x050003E0;
	base[0] = 0x1DEA;
	base[1] = 0xABAD;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x050007E0;
	testStore(config, (u32*) base);
}

static void testStoreOAM(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x070003E0;
	base[0] = 0xA5ED;
	base[1] = 0xDECE;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	testStore(config, (u32*) base);
	base[0] = 0x0200;
	base[1] = 0x0000;
	base[2] = 0x0000;
	base[3] = 0x0000;
}

static void testStoreOAMMirror(struct TestConfigurations* config) {
	VBlankIntrWait();
	u16* base = (u16*) 0x070003E0;
	base[0] = 0xA5ED;
	base[1] = 0xDECE;
	base[2] = 0xCCCC;
	base[3] = 0xCCCC;
	base = (u16*) 0x070007E0;
	testStore(config, (u32*) base);
	base[0] = 0x0200;
	base[1] = 0x0000;
	base[2] = 0x0000;
	base[3] = 0x0000;
}

static const struct MemoryTest memoryTests[] = {
	{ "ROM load", testLoadROM, false, {
		0xEF, 0xFFFFFFEF, 0xBEEF, 0xEF0000BE, 0xFFFFBEEF, 0xFFFFFFBE, 0xDEADBEEF, 0xEFDEADBE, 0xBEEFDEAD, 0xADBEEFDE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xBEEF, 0xBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		{ 0xDEADBEEF }, { 0xDE00BE }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF },
		{ 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }
	}},
	{ "ROM store", testStoreROM, true, {
		0xEF, 0xBEEF, 0xBEEF, 0xBEEF, 0, 0, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF,
		{ 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF },
		{ 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }, { 0xDEADBEEF }
	}},
	{ "ROM out-of-bounds load", testLoadROMBad, false, {
		0x56, 0x56, 0x3456, 0x56000034, 0x3456, 0x34, 0x34573456, 0x56345734, 0x34563457, 0x57345634,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0x3456, 0x3456, 0x34573456, 0x34573456, 0x34573456, 0x34573456,
		0x3456, 0x3456, 0x34573456, 0x34573456, 0x34573456, 0x34573456,
		0x3456, 0x3456, 0x34573456, 0x34573456, 0x34573456, 0x34573456,
		{ 0x34573456 }, { 0x340034 }, { 0x34573456 }, { 0x34573456 }, { 0x34573456 }, { 0x34573456 },
		{ 0x34573456 }, { 0x34573456 }, { 0x34573456 }, { 0x34573456 }
	}},
	{ "IWRAM load", testLoadIWRAM, false, {
		0xBE, 0xFFFFFFBE, 0xBABE, 0xBE0000BA, 0xFFFFBABE, 0xFFFFFFBA, 0xCAFEBABE, 0xBECAFEBA, 0xBABECAFE, 0xFEBABECA,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0xCA00BA }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
	}},
	{ "IWRAM mirror load", testLoadIWRAMMirror, false, {
		0xBE, 0xFFFFFFBE, 0xBABE, 0xBE0000BA, 0xFFFFBABE, 0xFFFFFFBA, 0xCAFEBABE, 0xBECAFEBA, 0xBABECAFE, 0xFEBABECA,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0xCA00BA }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
	}},
	{ "IWRAM store", testStoreIWRAM, true, {
		0xBE, 0xBE, 0xBABE, 0xBABE, 0, 0, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0 }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
	}},
	{ "IWRAM mirror store", testStoreIWRAMMirror, true, {
		0xBE, 0xBE, 0xBABE, 0xBABE, 0, 0, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		0xBABE, 0xBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE, 0xCAFEBABE,
		{ 0xCAFEBABE }, { 0 }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE },
		{ 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }, { 0xCAFEBABE }
	}},
	{ "EWRAM load", testLoadEWRAM, false, {
		0xCE, 0xFFFFFFCE, 0xFACE, 0xCE0000FA, 0xFFFFFACE, 0xFFFFFFFA, 0xFEEDFACE, 0xCEFEEDFA, 0xFACEFEED, 0xEDFACEFE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0xFE00FA }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "EWRAM mirror load", testLoadEWRAMMirror, false, {
		0xCE, 0xFFFFFFCE, 0xFACE, 0xCE0000FA, 0xFFFFFACE, 0xFFFFFFFA, 0xFEEDFACE, 0xCEFEEDFA, 0xFACEFEED, 0xEDFACEFE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0xFE00FA }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "EWRAM store", testStoreEWRAM, true, {
		0xCE, 0xCE, 0xFACE, 0xFACE, 0, 0, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0 }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "EWRAM mirror store", testStoreEWRAMMirror, true, {
		0xCE, 0xCE, 0xFACE, 0xFACE, 0, 0, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0xFEEDFACE }, { 0 }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE },
		{ 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }, { 0xFEEDFACE }
	}},
	{ "Palette load", testLoadPalette, false, {
		0xEA, 0xFFFFFFEA, 0x1DEA, 0xEA00001D, 0x1DEA, 0x1D, 0xABAD1DEA, 0xEAABAD1D, 0x1DEAABAD, 0xAD1DEAAB,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		{ 0xABAD1DEA }, { 0xAB001D }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA },
		{ 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }
	}},
	{ "Palette mirror load", testLoadPaletteMirror, false, {
		0xEA, 0xFFFFFFEA, 0x1DEA, 0xEA00001D, 0x1DEA, 0x1D, 0xABAD1DEA, 0xEAABAD1D, 0x1DEAABAD, 0xAD1DEAAB,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		{ 0xABAD1DEA }, { 0xAB001D }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA },
		{ 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }
	}},
	{ "Palette store", testStorePalette, true, {
		0xEA, 0xEAEA, 0x1DEA, 0x1DEA, 0, 0, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		{ 0xABAD1DEA }, { 0 }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA },
		{ 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }
	}},
	{ "Palette mirror store", testStorePaletteMirror, true, {
		0xEA, 0xEAEA, 0x1DEA, 0x1DEA, 0, 0, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		0x1DEA, 0x1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA, 0xABAD1DEA,
		{ 0xABAD1DEA }, { 0 }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA },
		{ 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }, { 0xABAD1DEA }
	}},
	{ "VRAM load", testLoadVRAM, false, {
		0xD, 0xD, 0xF00D, 0xD0000F0, 0xFFFFF00D, 0xFFFFFFF0, 0x900DF00D, 0x0D900DF0, 0xF00D900D, 0xDF00D90,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0x009000F0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM mirror load", testLoadVRAMMirror, false, {
		0xD, 0xD, 0xF00D, 0xD0000F0, 0xFFFFF00D, 0xFFFFFFF0, 0x900DF00D, 0x0D900DF0, 0xF00D900D, 0xDF00D90,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0x009000F0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ load", testLoadVRAM2, false, {
		0xD, 0xD, 0xF00D, 0xD0000F0, 0xFFFFF00D, 0xFFFFFFF0, 0x900DF00D, 0x0D900DF0, 0xF00D900D, 0xDF00D90,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0x009000F0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ mirror load", testLoadVRAM2Mirror, false, {
		0xD, 0xD, 0xF00D, 0xD0000F0, 0xFFFFF00D, 0xFFFFFFF0, 0x900DF00D, 0x0D900DF0, 0xF00D900D, 0xDF00D90,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0x009000F0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ mirror 2 load", testLoadVRAM2Mirror2, false, {
		0xD, 0xD, 0xF00D, 0xD0000F0, 0xFFFFF00D, 0xFFFFFFF0, 0x900DF00D, 0x0D900DF0, 0xF00D900D, 0xDF00D90,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0x009000F0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM store", testStoreVRAM, true, {
		0xD, 0xD0D, 0xF00D, 0xF00D, 0, 0, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM mirror store", testStoreVRAMMirror, true, {
		0xD, 0xD0D, 0xF00D, 0xF00D, 0, 0, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ store", testStoreVRAM2, true, {
		0, 0, 0xF00D, 0xF00D, 0, 0, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ mirror store", testStoreVRAM2Mirror, true, {
		0, 0, 0xF00D, 0xF00D, 0, 0, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "VRAM OBJ mirror 2 store", testStoreVRAM2Mirror2, true, {
		0, 0, 0xF00D, 0xF00D, 0, 0, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		0xF00D, 0xF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D, 0x900DF00D,
		{ 0x900DF00D }, { 0 }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D },
		{ 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }, { 0x900DF00D }
	}},
	{ "OAM load", testLoadOAM, false, {
		0xED, 0xFFFFFFED, 0xA5ED, 0xED0000A5, 0xFFFFA5ED, 0xFFFFFFA5, 0xDECEA5ED, 0xEDDECEA5, 0xA5EDDECE, 0xCEA5EDDE,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		{ 0xDECEA5ED }, { 0xDE00A5 }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED },
		{ 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }
	}},
	{ "OAM mirror load", testLoadOAMMirror, false, {
		0xED, 0xFFFFFFED, 0xA5ED, 0xED0000A5, 0xFFFFA5ED, 0xFFFFFFA5, 0xDECEA5ED, 0xEDDECEA5, 0xA5EDDECE, 0xCEA5EDDE,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		{ 0xDECEA5ED }, { 0xDE00A5 }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED },
		{ 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }
	}},
	{ "OAM store", testStoreOAM, true, {
		0, 0, 0xA5ED, 0xA5ED, 0, 0, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		{ 0xDECEA5ED }, { 0 }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED },
		{ 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }
	}},
	{ "OAM mirror store", testStoreOAMMirror, true, {
		0, 0, 0xA5ED, 0xA5ED, 0, 0, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		0xA5ED, 0xA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED, 0xDECEA5ED,
		{ 0xDECEA5ED }, { 0 }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED },
		{ 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }, { 0xDECEA5ED }
	}},
	{ "SRAM load", testLoadSRAM, false, {
		0x47, 0x47, 0x4747, 0x61000061, 0x4747, 0x61, 0x47474747, 0x61616161, 0x6D6D6D6D, 0x65656565,
		0, 0, 0, 0, 0, 0,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		{ 0x6D6D4747 }, { 0x650061 }, { 0x6D6D4747 }, { 0x61616161 }, { 0x6D6D6D6D }, { 0x65656565 },
		{ 0x47474747 }, { 0x61616161 }, { 0x6D6D6D6D }, { 0x65656565 }
	}},
	{ "SRAM mirror load", testLoadSRAMMirror, false, {
		0x47, 0x47, 0x4747, 0x61000061, 0x4747, 0x61, 0x47474747, 0x61616161, 0x6D6D6D6D, 0x65656565,
		0, 0, 0, 0, 0, 0,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		0x4747, 0x4747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		{ 0x6D6D4747 }, { 0x650061 }, { 0x6D6D4747 }, { 0x61616161 }, { 0x6D6D6D6D }, { 0x65656565 },
		{ 0x47474747 }, { 0x61616161 }, { 0x6D6D6D6D }, { 0x65656565 }
	}},
	{ "SRAM store", testStoreSRAM, true, {
		0x47, 0x4747, 0x4747, 0x4747, 0, 0, 0x47474747, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0x47474747, 0x47474747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		{ 0x47474747 }, { 0 }, { 0x47474747 }, { 0 }, { 0 }, { 0 },
		{ 0x47474747 }, { 0 }, { 0 }, { 0 }
	}},
	{ "SRAM mirror store", testStoreSRAMMirror, true, {
		0x47, 0x4747, 0x4747, 0x4747, 0, 0, 0x47474747, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
		0x47474747, 0x47474747, 0x47474747, 0x47474747, 0x47474747, 0x47474747,
		{ 0x47474747 }, { 0 }, { 0x47474747 }, { 0 }, { 0 }, { 0 },
		{ 0x47474747 }, { 0 }, { 0 }, { 0 }
	}},
	{ "BIOS load", testLoadBIOS, false, {
		0x4, 0x4, 0x2004, 0x04000020, 0x2004, 0x20, 0xE3A02004, 0x4E3A020, 0x2004E3A0, 0xA02004E3,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
		{ 0 }, { 0 }, { 0 }, { 0 }
	}},
	{ "BIOS out-of-bounds load", testLoadBIOSBad, false, {
		0xB0, 0xFFFFFFB1, 0xC0F0, 0xF1000070, 0x6000, 0x10, 0xE5902002, 0x3E59030, 0x8E59F, 0x800001E2,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		0xFACE, 0xFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE, 0xFEEDFACE,
		{ 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
		{ 0 }, { 0 }, { 0 }, { 0 }
	}},
	{ "Out-of-bounds load", testLoadBad, false, {
		0xB0, 0xFFFFFFB1, 0xC0F0, 0xF1000070, 0x6000, 0x10, 0xE5902002, 0x3E59030, 0x8E59F, 0x800001E2,
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
		snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], 28, "%08lX PASS", value);
	} else {
		snprintf(&textGrid[base + GRID_STRIDE * (line * 2 - offset + 1) + 4], 28, "%08lX != %08lX", value, expected);
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

static void printResults(const char* preface, const struct TestConfigurations* values, const struct TestConfigurations* expected, int base, bool store) {
	snprintf(&textGrid[GRID_STRIDE], 31, "Memory test: %s", preface);

	printResult(base,  0, "U8", values->_u8, expected->_u8);
	if (store) {
		printResult(base,  1, "U8 (U16 load)", values->_s8, expected->_s8);
	} else {
		printResult(base,  1, "S8", values->_s8, expected->_s8);
	}
	printResult(base,  2, "U16", values->_u16, expected->_u16);
	printResult(base,  3, "U16 (unaligned)", values->_u16u1, expected->_u16u1);
	if (!store) {
		printResult(base,  4, "S16", values->_s16, expected->_s16);
		printResult(base,  5, "S16 (unaligned)", values->_s16u1, expected->_s16u1);
	}
	printResult(base,  6, "32", values->_u32, expected->_u32);
	printResult(base,  7, "32 (unaligned 1)", values->_u32u1, expected->_u32u1);
	printResult(base,  8, "32 (unaligned 2)", values->_u32u2, expected->_u32u2);
	printResult(base,  9, "32 (unaligned 3)", values->_u32u3, expected->_u32u3);
	printResult(base, 10, "DMA0 16", values->_d0_16, expected->_d0_16);
	printResult(base, 11, "DMA0 16 (unaligned)", values->_d0_16u1, expected->_d0_16u1);
	printResult(base, 12, "DMA0 32", values->_d0_32, expected->_d0_32);
	printResult(base, 13, "DMA0 32 (unaligned 1)", values->_d0_32u1, expected->_d0_32u1);
	printResult(base, 14, "DMA0 32 (unaligned 2)", values->_d0_32u2, expected->_d0_32u2);
	printResult(base, 15, "DMA0 32 (unaligned 3)", values->_d0_32u3, expected->_d0_32u3);
	printResult(base, 16, "DMA1 16", values->_d1_16, expected->_d1_16);
	printResult(base, 17, "DMA1 16 (unaligned)", values->_d1_16u1, expected->_d1_16u1);
	printResult(base, 18, "DMA1 32", values->_d1_32, expected->_d1_32);
	printResult(base, 19, "DMA1 32 (unaligned 1)", values->_d1_32u1, expected->_d1_32u1);
	printResult(base, 20, "DMA1 32 (unaligned 2)", values->_d1_32u2, expected->_d1_32u2);
	printResult(base, 21, "DMA1 32 (unaligned 3)", values->_d1_32u3, expected->_d1_32u3);
	printResult(base, 22, "DMA2 16", values->_d2_16, expected->_d2_16);
	printResult(base, 23, "DMA2 16 (unaligned)", values->_d2_16u1, expected->_d2_16u1);
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
	printResult(base, 35, "swi B 16 (unaligned)", values->_c16u1[0], expected->_c16u1[0]);
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
		REG_IME = 0;
		VBlankIntrWait();
		activeTest = &memoryTests[i];
		activeTest->test(&currentTest);
		REG_IME = 1;

		savprintf("Memory test: %s", activeTest->testName);
		doResult("U8", activeTest->testName, currentTest._u8, activeTest->expected._u8);
		if (activeTest->store) {
			doResult("S8 (U16 load)", activeTest->testName, currentTest._s8, activeTest->expected._s8);
		} else {
			doResult("S8", activeTest->testName, currentTest._s8, activeTest->expected._s8);
		}
		doResult("U16", activeTest->testName, currentTest._u16, activeTest->expected._u16);
		doResult("U16 (unaligned)", activeTest->testName, currentTest._u16u1, activeTest->expected._u16u1);
		if (!activeTest->store) {
			doResult("S16", activeTest->testName, currentTest._s16, activeTest->expected._s16);
			doResult("S16 (unaligned)", activeTest->testName, currentTest._s16u1, activeTest->expected._s16u1);
		}
		doResult("32", activeTest->testName, currentTest._u32, activeTest->expected._u32);
		doResult("32 (unaligned 1)", activeTest->testName, currentTest._u32u1, activeTest->expected._u32u1);
		doResult("32 (unaligned 2)", activeTest->testName, currentTest._u32u2, activeTest->expected._u32u2);
		doResult("32 (unaligned 3)", activeTest->testName, currentTest._u32u3, activeTest->expected._u32u3);
		doResult("DMA0 16", activeTest->testName, currentTest._d0_16, activeTest->expected._d0_16);
		doResult("DMA0 16 (unaligned)", activeTest->testName, currentTest._d0_16u1, activeTest->expected._d0_16u1);
		doResult("DMA0 32", activeTest->testName, currentTest._d0_32, activeTest->expected._d0_32);
		doResult("DMA0 32 (unaligned 1)", activeTest->testName, currentTest._d0_32u1, activeTest->expected._d0_32u1);
		doResult("DMA0 32 (unaligned 2)", activeTest->testName, currentTest._d0_32u2, activeTest->expected._d0_32u2);
		doResult("DMA0 32 (unaligned 3)", activeTest->testName, currentTest._d0_32u3, activeTest->expected._d0_32u3);
		doResult("DMA1 16", activeTest->testName, currentTest._d1_16, activeTest->expected._d1_16);
		doResult("DMA1 16 (unaligned)", activeTest->testName, currentTest._d1_16u1, activeTest->expected._d1_16u1);
		doResult("DMA1 32", activeTest->testName, currentTest._d1_32, activeTest->expected._d1_32);
		doResult("DMA1 32 (unaligned 1)", activeTest->testName, currentTest._d1_32u1, activeTest->expected._d1_32u1);
		doResult("DMA1 32 (unaligned 2)", activeTest->testName, currentTest._d1_32u2, activeTest->expected._d1_32u2);
		doResult("DMA1 32 (unaligned 3)", activeTest->testName, currentTest._d1_32u3, activeTest->expected._d1_32u3);
		doResult("DMA2 16", activeTest->testName, currentTest._d2_16, activeTest->expected._d2_16);
		doResult("DMA2 16 (unaligned)", activeTest->testName, currentTest._d2_16u1, activeTest->expected._d2_16u1);
		doResult("DMA2 32", activeTest->testName, currentTest._d2_32, activeTest->expected._d2_32);
		doResult("DMA2 32 (unaligned 1)", activeTest->testName, currentTest._d2_32u1, activeTest->expected._d2_32u1);
		doResult("DMA2 32 (unaligned 2)", activeTest->testName, currentTest._d2_32u2, activeTest->expected._d2_32u2);
		doResult("DMA2 32 (unaligned 3)", activeTest->testName, currentTest._d2_32u3, activeTest->expected._d2_32u3);
		doResult("DMA3 16", activeTest->testName, currentTest._d3_16, activeTest->expected._d3_16);
		doResult("DMA3 16 (unaligned)", activeTest->testName, currentTest._d3_16u1, activeTest->expected._d3_16u1);
		doResult("DMA3 32", activeTest->testName, currentTest._d3_32, activeTest->expected._d3_32);
		doResult("DMA3 32 (unaligned 1)", activeTest->testName, currentTest._d3_32u1, activeTest->expected._d3_32u1);
		doResult("DMA3 32 (unaligned 2)", activeTest->testName, currentTest._d3_32u2, activeTest->expected._d3_32u2);
		doResult("DMA3 32 (unaligned 3)", activeTest->testName, currentTest._d3_32u3, activeTest->expected._d3_32u3);
		doResult("swi B 16", activeTest->testName, currentTest._c16[0], activeTest->expected._c16[0]);
		doResult("swi B 16 (unaligned)", activeTest->testName, currentTest._c16u1[0], activeTest->expected._c16u1[0]);
		doResult("swi B 32", activeTest->testName, currentTest._c16[0], activeTest->expected._c32[0]);
		doResult("swi B 32 (unaligned 1)", activeTest->testName, currentTest._c32u1[0], activeTest->expected._c32u1[0]);
		doResult("swi B 32 (unaligned 2)", activeTest->testName, currentTest._c32u2[0], activeTest->expected._c32u2[0]);
		doResult("swi B 32 (unaligned 3)", activeTest->testName, currentTest._c32u3[0], activeTest->expected._c32u3[0]);
		doResult("swi C 32", activeTest->testName, currentTest._cf32[0], activeTest->expected._cf32[0]);
		doResult("swi C 32 (unaligned 1)", activeTest->testName, currentTest._cf32u1[0], activeTest->expected._cf32u1[0]);
		doResult("swi C 32 (unaligned 2)", activeTest->testName, currentTest._cf32u2[0], activeTest->expected._cf32u2[0]);
		doResult("swi C 32 (unaligned 3)", activeTest->testName, currentTest._cf32u3[0], activeTest->expected._cf32u3[0]);
	}
}

static void showMemorySuite(size_t index) {
	const struct MemoryTest* activeTest = &memoryTests[index];
	struct TestConfigurations currentTest = {0};
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
			if (resultIndex < 72) {
				++resultIndex;
			}
		}
		activeTest->test(&currentTest);
		printResults(activeTest->testName, &currentTest, &activeTest->expected, resultIndex, activeTest->store);
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
