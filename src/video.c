#include "io-read.h"

#include <gba_compression.h>
#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sprites.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "font.h"
#include "suite.h"

#include "videotest.h"
#include "degenerateObjTransform.h"

struct VideoTest {
	const char* testName;
	void (*expected)(void);
	void (*actual)(void);
};

static void fillSquare(u32* base, u32 fill) {
	base[0] = fill;
	base[1] = fill;
	base[2] = fill;
	base[3] = fill;
	base[4] = fill;
	base[5] = fill;
	base[6] = fill;
	base[7] = fill;
}

static void basicExpected(void) {
	REG_BG2CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	fillSquare((u32*) 0x06008000, 0);
	fillSquare((u32*) 0x06008020, 0x11111111);
	int x = 0x00010000;
	int i;
	for (i = 0; i < 32; ++i) {
		*(u32*) (0x06000800 + i * 0x40) = x;
		*(u32*) (0x06000804 + i * 0x40) = x;
		*(u32*) (0x06000808 + i * 0x40) = x;
		*(u32*) (0x0600080C + i * 0x40) = x;
		*(u32*) (0x06000810 + i * 0x40) = x;
		*(u32*) (0x06000814 + i * 0x40) = x;
		*(u32*) (0x06000818 + i * 0x40) = x;
		*(u32*) (0x0600081C + i * 0x40) = x;
		*(u32*) (0x06000820 + i * 0x40) = x;
		*(u32*) (0x06000824 + i * 0x40) = x;
		*(u32*) (0x06000828 + i * 0x40) = x;
		*(u32*) (0x0600082C + i * 0x40) = x;
		*(u32*) (0x06000830 + i * 0x40) = x;
		*(u32*) (0x06000834 + i * 0x40) = x;
		*(u32*) (0x06000838 + i * 0x40) = x;
		*(u32*) (0x0600083C + i * 0x40) = x;
		x ^= 0x00010001;
	}
	VBlankIntrWait();
	REG_DISPCNT = MODE_0 | BG2_ON;
}

static void basic3Actual(void) {
	u32 x = 0xFFFFFFFF;
	int i, j;
	for (i = 0; i < 160; ++i) {
		for (j = 0; j < 15; ++j) {
			*(u32*) (0x06000000 + i * (32 * 15) + j * 32) = x;
			*(u32*) (0x06000004 + i * (32 * 15) + j * 32) = x;
			*(u32*) (0x06000008 + i * (32 * 15) + j * 32) = x;
			*(u32*) (0x0600000C + i * (32 * 15) + j * 32) = x;
			*(u32*) (0x06000010 + i * (32 * 15) + j * 32) = ~x;
			*(u32*) (0x06000014 + i * (32 * 15) + j * 32) = ~x;
			*(u32*) (0x06000018 + i * (32 * 15) + j * 32) = ~x;
			*(u32*) (0x0600001C + i * (32 * 15) + j * 32) = ~x;
		}
		if ((i & 7) == 7) {
			x = ~x;
		}
	}
	VBlankIntrWait();
	REG_DISPCNT = MODE_3 | BG2_ON;
}

static void basic4Actual(void) {
	u32 x = 0x00000000;
	u32 y = 0x01010101;
	int i, j;
	for (i = 0; i < 160; ++i) {
		for (j = 0; j < 15; ++j) {
			*(u32*) (0x06000000 + i * (16 * 15) + j * 16) = x;
			*(u32*) (0x06000004 + i * (16 * 15) + j * 16) = x;
			*(u32*) (0x06000008 + i * (16 * 15) + j * 16) = y;
			*(u32*) (0x0600000C + i * (16 * 15) + j * 16) = y;
		}
		if ((i & 7) == 7) {
			x ^= 0x01010101;
			y ^= 0x01010101;
		}
	}
	VBlankIntrWait();
	REG_DISPCNT = MODE_4 | BG2_ON;
}

static void degenerateObjTransform(void) {
	OBJ_COLORS[0x10] = 0x7F1C;
	OBJ_COLORS[0x11] = 0x0000;
	OBJ_COLORS[0x12] = 0x1084;
	OBJ_COLORS[0x13] = 0x1084 * 2;
	OBJ_COLORS[0x14] = 0x1084 * 3;
	OBJ_COLORS[0x15] = 0x1084 * 4;
	OBJ_COLORS[0x16] = 0x1084 * 5;
	OBJ_COLORS[0x17] = 0x1084 * 6;
	OBJ_COLORS[0x18] = 0x1084 * 7;
	int i;
	for (i = 0; i < 64; ++i) {
		*(u32*) (0x06014800 + i * 0x20) = 0x12345678;
		*(u32*) (0x06014804 + i * 0x20) = 0x45678123;
		*(u32*) (0x06014808 + i * 0x20) = 0x78123456;
		*(u32*) (0x0601480C + i * 0x20) = 0x23456781;
		*(u32*) (0x06014810 + i * 0x20) = 0x56781234;
		*(u32*) (0x06014814 + i * 0x20) = 0x81234567;
		*(u32*) (0x06014818 + i * 0x20) = 0x34567812;
		*(u32*) (0x0601481C + i * 0x20) = 0x67812345;
	}

	OAM[1].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(0);
	OAM[1].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(0) | OBJ_X(0);
	OAM[1].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[2].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(0);
	OAM[2].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(1) | OBJ_X(64);
	OAM[2].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[3].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(64);
	OAM[3].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(2) | OBJ_X(0);
	OAM[3].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[4].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(64);
	OAM[4].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(3) | OBJ_X(64);
	OAM[4].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[5].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(0);
	OAM[5].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(4) | OBJ_X(128);
	OAM[5].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[6].attr0 = ATTR0_COLOR_16 | ATTR0_ROTSCALE | OBJ_Y(64);
	OAM[6].attr1 = ATTR1_SIZE_64 | OBJ_ROT_SCALE(5) | OBJ_X(128);
	OAM[6].attr2 = OBJ_CHAR(0x240) | OBJ_PALETTE(1);

	OAM[0].dummy = 256;
	OAM[1].dummy = 0;
	OAM[2].dummy = 0;
	OAM[3].dummy = 0;

	OAM[4].dummy = 0;
	OAM[5].dummy = 256;
	OAM[6].dummy = 0;
	OAM[7].dummy = 0;

	OAM[8].dummy = 0;
	OAM[9].dummy = 0;
	OAM[10].dummy = 256;
	OAM[11].dummy = 0;

	OAM[12].dummy = 0;
	OAM[13].dummy = 0;
	OAM[14].dummy = 0;
	OAM[15].dummy = 256;

	OAM[16].dummy = 0;
	OAM[17].dummy = 0;
	OAM[18].dummy = 0;
	OAM[19].dummy = 0;

	OAM[20].dummy = 256;
	OAM[21].dummy = 256;
	OAM[22].dummy = 256;
	OAM[23].dummy = 256;
	REG_DISPCNT = OBJ_ON | OBJ_1D_MAP;
}

static void degenerateObjTransformExpected(void) {
	REG_BG2CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	LZ77UnCompVram((void*) degenerateObjTransformTiles, (void*) 0x06008000);
	LZ77UnCompVram((void*) degenerateObjTransformMap, (void*) 0x06000800);
	CpuFastSet(degenerateObjTransformPal, BG_PALETTE, 8);
	REG_DISPCNT = MODE_0 | BG2_ON;
}

static void toggleBg0(void) {
	while (!(REG_DISPSTAT & 2));
	REG_DISPSTAT ^= 0x2000;
	REG_DISPCNT ^= BG0_ON;
}

static void layerToggle(void) {
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	*(u32*) 0x06008000 = 0x22221111;
	*(u32*) 0x06008004 = 0x22221111;
	*(u32*) 0x06008008 = 0x22221111;
	*(u32*) 0x0600800C = 0x22221111;
	*(u32*) 0x06008010 = 0x11112222;
	*(u32*) 0x06008014 = 0x11112222;
	*(u32*) 0x06008018 = 0x11112222;
	*(u32*) 0x0600801C = 0x11112222;
	uint32_t zero = 0;
	CpuFastSet(&zero, (void*) 0x06000800, 0x01000400);
	BG_PALETTE[0] = 0x3DEF;
	BG_PALETTE[1] = 0x7FFF;
	BG_PALETTE[2] = 0;
	REG_DISPSTAT = 0x4528;
	irqSet(IRQ_VCOUNT, toggleBg0);
	irqEnable(IRQ_VCOUNT);
	VBlankIntrWait();
	REG_DISPCNT = MODE_0 | BG0_ON;
}

static void layerToggleExpected(void) {
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	*(u32*) 0x06008000 = 0x22221111;
	*(u32*) 0x06008004 = 0x22221111;
	*(u32*) 0x06008008 = 0x22221111;
	*(u32*) 0x0600800C = 0x22221111;
	*(u32*) 0x06008010 = 0x11112222;
	*(u32*) 0x06008014 = 0x11112222;
	*(u32*) 0x06008018 = 0x11112222;
	*(u32*) 0x0600801C = 0x11112222;
	*(u32*) 0x06008020 = 0;
	*(u32*) 0x06008024 = 0;
	*(u32*) 0x06008028 = 0;
	*(u32*) 0x0600802C = 0;
	*(u32*) 0x06008030 = 0;
	*(u32*) 0x06008034 = 0;
	*(u32*) 0x06008038 = 0;
	*(u32*) 0x0600803C = 0;
	*(u32*) 0x06008040 = 0x22221111;
	*(u32*) 0x06008044 = 0x22221111;
	*(u32*) 0x06008048 = 0x22221111;
	*(u32*) 0x0600804C = 0x22221111;
	*(u32*) 0x06008050 = 0x11112222;
	*(u32*) 0x06008054 = 0x11112222;
	*(u32*) 0x06008058 = 0;
	*(u32*) 0x0600805C = 0;
	uint32_t zero = 0;
	CpuFastSet(&zero, (void*) 0x06000800, 0x01000400);
	zero = 0x00020002;
	CpuFastSet(&zero, (void*) 0x06000A00, 0x01000040);
	zero = 0x00010001;
	CpuFastSet(&zero, (void*) 0x06000A40, 0x01000040);
	BG_PALETTE[0] = 0x3DEF;
	BG_PALETTE[1] = 0x7FFF;
	BG_PALETTE[2] = 0;
	irqDisable(IRQ_VCOUNT);
	VBlankIntrWait();
	REG_DISPCNT = MODE_0 | BG0_ON;
}

static const struct VideoTest videoTests[] = {
	{ "Basic Mode 3", basicExpected, basic3Actual },
	{ "Basic Mode 4", basicExpected, basic4Actual },
	{ "Degenerate OBJ transforms", degenerateObjTransformExpected, degenerateObjTransform },
	{ "Layer toggle", layerToggleExpected, layerToggle },
};

static const u32 nTests = sizeof(videoTests) / sizeof(*videoTests);

const static unsigned constZero = 0;

static size_t listVideoSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = videoTests[i + offset].testName;
	}
	return i;
}

static void showVideoSuite(size_t index) {
	OBJ_COLORS[0] = 0x7F1C;
	OBJ_COLORS[1] = 0x7FFF;
	OBJ_COLORS[2] = 0x0000;
	LZ77UnCompVram(expectedTiles, (void*) 0x06014000);
	LZ77UnCompVram(actualTiles, (void*) 0x06014400);
	const struct VideoTest* activeTest = &videoTests[index];
	bool showExpected = false;
	bool performShow = true;
	bool showNav = true;
	u16 dispcnt = REG_DISPCNT;
	OAM[0].attr0 = ATTR0_COLOR_16 | ATTR0_WIDE | OBJ_Y(148);

	REG_BG2PA = 0x100;
	REG_BG2PB = 0;
	REG_BG2PC = 0;
	REG_BG2PD = 0x100;

	while (1) {
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_B) {
			break;
		}

		if (keys & KEY_LEFT && showExpected) {
			showExpected = false;
			performShow = true;
		} else if (keys & (KEY_RIGHT | KEY_A) && !showExpected) {
			showExpected = true;
			performShow = true;
		}
		if (keys & KEY_START) {
			showNav = !showNav;
			if (!showNav) {
				OAM[0].attr0 |= OBJ_DISABLE;
			} else {
				OAM[0].attr0 &= ~OBJ_DISABLE;
			}
		}
		VBlankIntrWait();
		if (performShow) {
			REG_DISPCNT = LCDC_OFF;
			int i;
			for (i = 1; i < 128; ++i) {
				OAM[i].attr0 = OBJ_DISABLE;
			}
			if (showExpected) {
				OAM[0].attr1 = ATTR1_SIZE_64 | OBJ_X(84);
				OAM[0].attr2 = OBJ_CHAR(0x200);
				activeTest->expected();
			} else {
				OAM[0].attr1 = ATTR1_SIZE_64 | OBJ_X(91);
				OAM[0].attr2 = OBJ_CHAR(0x220);
				activeTest->actual();
			}
			performShow = 0;
			REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
		}
	}
	// Clean up
	BG_PALETTE[0] = 0x7FFF;
	BG_PALETTE[1] = 0;
	DMA3COPY(fontTiles, TILE_BASE_ADR(1), DMA16 | DMA_IMMEDIATE | (fontTilesLen >> 1));
	memset((void*) 0x06000500, 0, 0x300);
	REG_DISPCNT = dispcnt;
	REG_DISPSTAT &= ~0x0030;
	irqDisable(IRQ_VCOUNT);
}

const struct TestSuite videoTestSuite = {
	.name = "Video tests",
	.run = NULL,
	.list = listVideoSuite,
	.show = showVideoSuite,
	.nTests = sizeof(videoTests) / sizeof(*videoTests),
	.passes = &constZero,
	.totalResults = &constZero
};
