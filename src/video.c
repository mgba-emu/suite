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

#include "actual.h"
#include "degenerateObjTransform.h"
#include "expected.h"

struct VideoTest {
	const char* testName;
	bool (*expected)(bool refresh);
	bool (*actual)(bool refresh);
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

static bool basicExpected(bool) {
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
	return false;
}

static bool basic3Actual(bool) {
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
	return false;
}

static bool basic4Actual(bool) {
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
	return false;
}

static bool degenerateObjTransform(bool) {
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
	return false;
}

static bool degenerateObjTransformExpected(bool) {
	REG_BG2CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	LZ77UnCompVram((void*) degenerateObjTransformTiles, (void*) 0x06008000);
	LZ77UnCompVram((void*) degenerateObjTransformMap, (void*) 0x06000800);
	CpuFastSet(degenerateObjTransformPal, BG_PALETTE, 8);
	uint32_t zero = 0;
	CpuFastSet(&zero, (void*) 0x06000c00, 0x01000100);
	REG_DISPCNT = MODE_0 | BG2_ON;
	return false;
}

static void toggleBg0(void) {
	while (!(REG_DISPSTAT & 2));
	REG_DISPSTAT ^= 0x2000;
	REG_DISPCNT ^= BG0_ON;
}

static bool layerToggle(bool) {
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
	return false;
}

static bool layerToggleExpected(bool) {
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
	return false;
}

static char oamDelayInside = 0;

static void oamDelayVcount(void) {
	while (!(REG_DISPSTAT & LCDC_HBL_FLAG));

	if (!oamDelayInside) {
		BG_PALETTE[1] = RGB5(31, 0, 0);
		BG_PALETTE[2] = RGB5(0, 0, 31);
		OAM[1].attr1 = ATTR1_SIZE_64 | OBJ_X(88 + 16);
		REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | VCOUNT(96-1);
		oamDelayInside = 1;
	} else {
		BG_PALETTE[1] = RGB5(31, 31, 31);
		BG_PALETTE[2] = RGB5(0, 0, 0);
		OAM[1].attr1 = ATTR1_SIZE_64 | OBJ_X(88 - 16);
		REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | VCOUNT(64-1);
		oamDelayInside = 0;
	}
}

static bool oamDelayActual(bool) {
	REG_DISPCNT = MODE_0 | BG0_ON;
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	*(u32*) 0x06008000 = 0x22112211;
	*(u32*) 0x06008004 = 0x22112211;
	*(u32*) 0x06008008 = 0x11221122;
	*(u32*) 0x0600800C = 0x11221122;
	*(u32*) 0x06008010 = 0x22112211;
	*(u32*) 0x06008014 = 0x22112211;
	*(u32*) 0x06008018 = 0x11221122;
	*(u32*) 0x0600801C = 0x11221122;

	uint32_t zero = 0;
	CpuFastSet(&zero, (void*) 0x06000800, 0x01000400);
	BG_PALETTE[0] = RGB5(15, 15, 15);
	BG_PALETTE[1] = RGB5(31, 31, 31);
	BG_PALETTE[2] = RGB5(0, 0, 0);

	SPRITE_PALETTE[16 + 1] = RGB5(15, 31, 15);
	memset(SPR_VRAM(0x0), 0x11, 64*64 / 2);
	OAM[1].attr0 = ATTR0_COLOR_16 | OBJ_Y(64 - 16);
	OAM[1].attr1 = ATTR1_SIZE_64 | OBJ_X(88 - 8);
	OAM[1].attr2 = OBJ_CHAR(0x0) | OBJ_PALETTE(1);
	OAM[2].attr0 = OBJ_DISABLE;
	OAM[2].attr1 = 0;
	OAM[2].attr2 = 0;
	OAM[3].attr0 = OBJ_DISABLE;
	OAM[3].attr1 = 0;
	OAM[3].attr2 = 0;

	REG_DISPSTAT = LCDC_VBL | LCDC_VCNT | VCOUNT(64 - 1);
	oamDelayInside = 0;
	irqSet(IRQ_VCOUNT, oamDelayVcount);
	irqEnable(IRQ_VCOUNT);
	VBlankIntrWait();
	REG_DISPCNT = MODE_0 | BG0_ON;
	return false;
}

static void setTiles(int y, int x, int w, u16 val) {
	for (int i = 0; i < w; ++i) {
		MAP[1][y][x + i] = val;
	}
}

static void oamDelayVcountBgOnly(void) {
	while (!(REG_DISPSTAT & LCDC_HBL_FLAG));

	if (!oamDelayInside) {
		BG_PALETTE[1] = RGB5(31, 0, 0);
		BG_PALETTE[2] = RGB5(0, 0, 31);
		REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | VCOUNT(96-1);
		oamDelayInside = 1;
	} else {
		BG_PALETTE[1] = RGB5(31, 31, 31);
		BG_PALETTE[2] = RGB5(0, 0, 0);
		OAM[1].attr1 = ATTR1_SIZE_64 | OBJ_X(32);
		REG_DISPSTAT = (REG_DISPSTAT & 0x00FF) | VCOUNT(64-1);
		oamDelayInside = 0;
	}
}

static bool oamDelayExpected(bool) {
	REG_DISPCNT = MODE_0 | BG0_ON;
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	*(u32*) 0x06008000 = 0x22112211;
	*(u32*) 0x06008004 = 0x22112211;
	*(u32*) 0x06008008 = 0x11221122;
	*(u32*) 0x0600800C = 0x11221122;
	*(u32*) 0x06008010 = 0x22112211;
	*(u32*) 0x06008014 = 0x22112211;
	*(u32*) 0x06008018 = 0x11221122;
	*(u32*) 0x0600801C = 0x11221122;
	memcpy((void*) 0x06008020, (void*) 0x06008000, 8 * 4);
	*(u32*) 0x06008020 = 0x33333333;
	memset((void*) 0x06008040, 0x33, 8*4);
	*(u32*) 0x06008040 = 0x22112211;
	memset((void*) 0x06008060, 0x33, 8*4);

	uint32_t zero = 0;
	CpuFastSet(&zero, (void*) 0x06000800, 0x01000400);
	//        y,  x, w, val
	setTiles( 6,  9, 8,   3);
	setTiles( 7,  9, 8,   3);
	setTiles( 8,  9, 4,   1);
	setTiles( 8, 13, 4,   3);
	setTiles( 8, 17, 4,   2);
	setTiles( 9, 13, 8,   3);
	setTiles(10, 13, 8,   3);
	setTiles(11, 13, 8,   3);
	setTiles(12,  9, 4,   2);
	setTiles(12, 13, 4,   3);
	setTiles(12, 17, 4,   1);
	setTiles(13,  9, 8,   3);

	BG_PALETTE[0] = RGB5(15, 15, 15);
	BG_PALETTE[1] = RGB5(31, 31, 31);
	BG_PALETTE[2] = RGB5(0, 0, 0);
	BG_PALETTE[3] = RGB5(15, 31, 15);

	REG_DISPSTAT = LCDC_VBL | LCDC_VCNT | VCOUNT(64 - 1);
	oamDelayInside = 0;
	irqSet(IRQ_VCOUNT, oamDelayVcountBgOnly);
	irqEnable(IRQ_VCOUNT);
	VBlankIntrWait();
	REG_DISPCNT = MODE_0 | BG0_ON;
	return false;
}

static bool windowOffscreenActual(bool) {
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	REG_WINOUT = 0x0010;
	REG_WININ = 0xFFFF;
	REG_WIN0H = 0x0078;
	REG_WIN0V = 0x50E3;
	REG_WIN1H = 0x78F0;
	REG_WIN1V = 0x50E4;
	fillSquare((u32*) 0x06008000, 0x11111111);
	BG_PALETTE[0] = RGB5(31, 31, 31);
	BG_PALETTE[1] = RGB5(15, 15, 15);
	REG_DISPCNT = MODE_0 | BG0_ON | WIN0_ON | WIN1_ON;
	return false;
}

static bool windowOffscreenExpected(bool) {
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	REG_WINOUT = 0x0010;
	REG_WINOUT = 0x0010;
	REG_WININ = 0xFFFF;
	REG_WIN0H = 0x0078;
	REG_WIN0V = 0x50A0;
	REG_WIN1H = 0x78F0;
	REG_WIN1V = 0x00A0;
	fillSquare((u32*) 0x06008000, 0x11111111);
	BG_PALETTE[0] = RGB5(31, 31, 31);
	BG_PALETTE[1] = RGB5(15, 15, 15);
	REG_DISPCNT = MODE_0 | BG0_ON | WIN0_ON | WIN1_ON;
	return false;
}

static void toggleLines(void) {
	switch (REG_VCOUNT) {
	case 0x00:
	case 0x09:
	case 0x12:
	case 0x1B:
	case 0x24:
	case 0x2D:
	case 0x37:
	case 0x81:
	case 0x8A:
		REG_DISPCNT |= BG0_ON;
		break;
	case 0x40:
	case 0x49:
	case 0x52:
	case 0x5B:
	case 0x64:
	case 0x6D:
	case 0x77:
	case 0x91:
	case 0x9A:
		while (REG_DISPSTAT & LCDC_HBL_FLAG);
		REG_DISPCNT |= BG0_ON;
		break;
	case 0x07:
	case 0x0F:
	case 0x17:
	case 0x1F:
	case 0x27:
	case 0x2F:
	case 0x3F:
	case 0x47:
	case 0x4F:
	case 0x57:
	case 0x5F:
	case 0x67:
	case 0x6F:
	case 0x7F:
	case 0x88:
	case 0x90:
	case 0x98:
		REG_DISPCNT &= ~BG0_ON;
	}
}

static bool bgEnableExpected(bool) {
	REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
	BG_PALETTE[0] = RGB5(31, 31, 31);
	BG_PALETTE[1] = RGB5(0, 0, 0);
	BG_PALETTE[2] = RGB5(15, 15, 15);

	u32* base = (u32*) 0x06008000;

	base[0x08] = 0x00000000;
	base[0x09] = 0x00000000;
	base[0x0A] = 0x00000000;
	base[0x0B] = 0x11221122;
	base[0x0C] = 0x22112211;
	base[0x0D] = 0x22112211;
	base[0x0E] = 0x11221122;
	base[0x0F] = 0x11221122;

	base[0x10] = 0x00000000;
	base[0x11] = 0x00000000;
	base[0x12] = 0x00000000;
	base[0x13] = 0x00000000;
	base[0x14] = 0x22112211;
	base[0x15] = 0x22112211;
	base[0x16] = 0x11221122;
	base[0x17] = 0x11221122;

	base[0x18] = 0x00000000;
	base[0x19] = 0x00000000;
	base[0x1A] = 0x00000000;
	base[0x1B] = 0x00000000;
	base[0x1C] = 0x00000000;
	base[0x1D] = 0x22112211;
	base[0x1E] = 0x11221122;
	base[0x1F] = 0x11221122;

	base[0x20] = 0x00000000;
	base[0x21] = 0x00000000;
	base[0x22] = 0x00000000;
	base[0x23] = 0x00000000;
	base[0x24] = 0x00000000;
	base[0x25] = 0x00000000;
	base[0x26] = 0x11221122;
	base[0x27] = 0x11221122;

	base[0x28] = 0x00000000;
	base[0x29] = 0x00000000;
	base[0x2A] = 0x00000000;
	base[0x2B] = 0x00000000;
	base[0x2C] = 0x00000000;
	base[0x2D] = 0x00000000;
	base[0x2E] = 0x00000000;
	base[0x2F] = 0x11221122;

	base[0x30] = 0x00000000;
	base[0x31] = 0x00000000;
	base[0x32] = 0x11221122;
	base[0x33] = 0x11221122;
	base[0x34] = 0x22112211;
	base[0x35] = 0x22112211;
	base[0x36] = 0x11221122;
	base[0x37] = 0x11221122;

	base[0x38] = 0x00000000;
	base[0x39] = 0x22112211;
	base[0x3A] = 0x00000000;
	base[0x3B] = 0x00000000;
	base[0x3C] = 0x22112211;
	base[0x3D] = 0x22112211;
	base[0x3E] = 0x11221122;
	base[0x3F] = 0x11221122;

	base[0x40] = 0x00000000;
	base[0x41] = 0x11221100;
	base[0x42] = 0x00000000;
	base[0x43] = 0x00000000;
	base[0x44] = 0x22112211;
	base[0x45] = 0x22112211;
	base[0x46] = 0x11221122;
	base[0x47] = 0x11221122;

	base[0x48] = 0x00000000;
	base[0x49] = 0x00000000;
	base[0x4A] = 0x00000000;
	base[0x4B] = 0x11221122;
	base[0x4C] = 0x22112211;
	base[0x4D] = 0x22112211;
	base[0x4E] = 0x11221122;
	base[0x4F] = 0x11221122;

	base[0x50] = 0x00000000;
	base[0x51] = 0x00000000;
	base[0x52] = 0x00000000;
	base[0x53] = 0x00000000;
	base[0x54] = 0x22112211;
	base[0x55] = 0x22112211;
	base[0x56] = 0x11221122;
	base[0x57] = 0x11221122;

	base[0x58] = 0x22112211;
	base[0x59] = 0x00000000;
	base[0x5A] = 0x00000000;
	base[0x5B] = 0x00000000;
	base[0x5C] = 0x00000000;
	base[0x5D] = 0x22112211;
	base[0x5E] = 0x11221122;
	base[0x5F] = 0x11221122;

	base[0x60] = 0x22112211;
	base[0x61] = 0x00000000;
	base[0x62] = 0x11221122;
	base[0x63] = 0x00000000;
	base[0x64] = 0x00000000;
	base[0x65] = 0x22112211;
	base[0x66] = 0x11221122;
	base[0x67] = 0x11221122;

	base[0x68] = 0x22112211;
	base[0x69] = 0x00000000;
	base[0x6A] = 0x22112211;
	base[0x6B] = 0x00000000;
	base[0x6C] = 0x00000000;
	base[0x6D] = 0x22112211;
	base[0x6E] = 0x11221122;
	base[0x6F] = 0x11221122;

	base[0x70] = 0x22112211;
	base[0x71] = 0x00000000;
	base[0x72] = 0x00000000;
	base[0x73] = 0x00000000;
	base[0x74] = 0x00000000;
	base[0x75] = 0x00000000;
	base[0x76] = 0x11221122;
	base[0x77] = 0x11221122;

	const u32 tileset[20] = {
		0x00010001,
		0x00020002,
		0x00030003,
		0x00040004,
		0x00050005,
		0,
		0,
		0x00060006,
		0x00070007,
		0x00030003,
		0x00040004,
		0x00050005,
		0,
		0,
		0,
		0x00090009,
		0x000A000A,
		0x000B000B,
		0x000C000C,
		0x000E000E,
	};

	int i;
	for (i = 0; i < 20; ++i) {
		CpuFastSet(&tileset[i], MAP[1][i], FILL | COPY32 | 0x10);
	}
	MAP[1][8][0] = 0x0008;
	MAP[1][18][0] = 0x000D;

	REG_DISPCNT = BG0_ON;
	return false;
}

static bool bgEnableActual(bool refresh) {
	if (!refresh) {
		REG_BG0CNT = CHAR_BASE(2) | SCREEN_BASE(1);
		BG_PALETTE[0] = RGB5(31, 31, 31);
		BG_PALETTE[1] = RGB5(0, 0, 0);
		BG_PALETTE[2] = RGB5(15, 15, 15);

		u32* base = (u32*) 0x06008000;
		base[0 + 8] = 0x22112211;
		base[1 + 8] = 0x22112211;
		base[2 + 8] = 0x11221122;
		base[3 + 8] = 0x11221122;
		base[4 + 8] = 0x22112211;
		base[5 + 8] = 0x22112211;
		base[6 + 8] = 0x11221122;
		base[7 + 8] = 0x11221122;

		int i;
		for (i = 0; i < 1024; ++i) {
			MAP[1][i >> 5][i & 0x1F] = 1;
		}
	}

	REG_DISPSTAT = LCDC_VBL | LCDC_HBL;
	irqSet(IRQ_HBLANK, toggleLines);
	irqEnable(IRQ_HBLANK);

	REG_DISPCNT = 0;
	return true;
}

static const struct VideoTest videoTests[] = {
	{ "Basic Mode 3", basicExpected, basic3Actual },
	{ "Basic Mode 4", basicExpected, basic4Actual },
	{ "Degenerate OBJ transforms", degenerateObjTransformExpected, degenerateObjTransform },
	{ "Layer toggle", layerToggleExpected, layerToggle },
	{ "Layer toggle 2", bgEnableExpected, bgEnableActual },
	{ "OAM Update Delay", oamDelayExpected, oamDelayActual },
	{ "Window offscreen reset", windowOffscreenExpected, windowOffscreenActual },
};

static const u32 nTests = sizeof(videoTests) / sizeof(*videoTests);

const static unsigned constZero = 0;

static size_t listVideoSuite(const char** names, bool* passed, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTests) {
			break;
		}
		names[i] = videoTests[i + offset].testName;
		passed[i] = true;
	}
	return i;
}

static void showVideoSuite(size_t index) {
	OBJ_COLORS[0] = 0x7F1C;
	OBJ_COLORS[1] = 0x7FFF;
	OBJ_COLORS[2] = 0x0000;
	LZ77UnCompVram(expectedTiles, SPR_VRAM(0x200));
	LZ77UnCompVram(actualTiles, SPR_VRAM(0x220));
	const struct VideoTest* activeTest = &videoTests[index];
	bool showExpected = false;
	bool performShow = true;
	bool showNav = true;
	bool refresh = false;
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
		if (performShow || refresh) {
			if (performShow) {
				refresh = false;
				irqDisable(IRQ_VCOUNT | IRQ_HBLANK);
			}
			REG_DISPCNT = LCDC_OFF;
			int i;
			for (i = 1; i < 128; ++i) {
				OAM[i].attr0 = OBJ_DISABLE;
			}
			if (showExpected) {
				OAM[0].attr1 = ATTR1_SIZE_64 | OBJ_X(84);
				OAM[0].attr2 = OBJ_CHAR(0x200);
				refresh = activeTest->expected(refresh);
			} else {
				OAM[0].attr1 = ATTR1_SIZE_64 | OBJ_X(91);
				OAM[0].attr2 = OBJ_CHAR(0x220);
				refresh = activeTest->actual(refresh);
			}
			performShow = false;
			REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
		}
	}
	// Clean up
	uint32_t zero = 0;
	CpuFastSet(&zero, (u32*) VRAM, FILL | COPY32 | 0x6000);
	BG_PALETTE[0] = 0x7FFF;
	BG_PALETTE[1] = 0;
	DMA3COPY(fontTiles, TILE_BASE_ADR(1), DMA16 | DMA_IMMEDIATE | (fontTilesLen >> 1));
	memset((void*) 0x06000500, 0, 0x300);
	REG_DISPCNT = dispcnt;
	REG_DISPSTAT &= ~0x0030;
	REG_BG1CNT = CHAR_BASE(1) | SCREEN_BASE(0);
	REG_BG1VOFS = -4;
	irqDisable(IRQ_VCOUNT | IRQ_HBLANK);
	irqInit();
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
