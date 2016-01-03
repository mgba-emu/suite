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

struct VideoTest {
	const char* testName;
	void (*expected)(void);
	void (*actual)(void);
};

static void basicExpected(void) {
	REG_BG2CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	*(u32*) 0x06008000 = 0;
	*(u32*) 0x06008004 = 0;
	*(u32*) 0x06008008 = 0;
	*(u32*) 0x0600800C = 0;
	*(u32*) 0x06008010 = 0;
	*(u32*) 0x06008014 = 0;
	*(u32*) 0x06008018 = 0;
	*(u32*) 0x0600801C = 0;
	*(u32*) 0x06008020 = 0x11111111;
	*(u32*) 0x06008024 = 0x11111111;
	*(u32*) 0x06008028 = 0x11111111;
	*(u32*) 0x0600802C = 0x11111111;
	*(u32*) 0x06008030 = 0x11111111;
	*(u32*) 0x06008034 = 0x11111111;
	*(u32*) 0x06008038 = 0x11111111;
	*(u32*) 0x0600803C = 0x11111111;
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

static const struct VideoTest videoTests[] = {
	{ "Basic Mode 3", basicExpected, basic3Actual },
	{ "Basic Mode 4", basicExpected, basic4Actual },
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
	// Nice job libgba...TODO: file a PR
	OBJ_COLORS[0] = 0x7F1C;
	OBJ_COLORS[1] = 0x7FFF;
	OBJ_COLORS[2] = 0x0000;
	LZ77UnCompVram((void*) expectedTiles, (void*) 0x06014000);
	LZ77UnCompVram((void*) actualTiles, (void*) 0x06014400);
	const struct VideoTest* activeTest = &videoTests[index];
	bool showExpected = false;
	bool performShow = true;
	bool showNav = true;
	u16 dispcnt = REG_DISPCNT;
	OAM[0].attr0 = ATTR0_COLOR_16 | ATTR0_WIDE | OBJ_Y(148);
	int i;
	for (i = 1; i < 128; ++i) {
		OAM[i].attr0 = OBJ_DISABLE;
	}
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
