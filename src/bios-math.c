#include "bios-math.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TestMath {
	int syscall;
	s32 in0;
	s32 in1;
	s32 outR0;
	s32 outR1;
	s32 outR2;
	s32 outR3;
	s32 outPsr;
};

struct MathTest {
	const char* testName;
	struct TestMath expected;
};

static const struct MathTest mathTests[] = {
	{ "ArcTan 00000000",           { 0x9,          0,          0,          0,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan 00000001",           { 0x9,          1,          0,          0,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan 00003FFF",           { 0x9,     0x3FFF,          0,     0x1FFF, 0xFFFFC002, 0xEF090000,  0x8001, 0x0000001F } },
	{ "ArcTan 00004000",           { 0x9,     0x4000,          0,     0x2000, 0xFFFFC000, 0xEF090000,  0x8000, 0x0000001F } },
	{ "ArcTan 00004001",           { 0x9,     0x4001,          0,     0x1FFF, 0xFFFFBFFE, 0xEF090000,  0x7FFE, 0x0000001F } },
	{ "ArcTan 00007FFF",           { 0x9,     0x7FFF,          0,     0x16D8, 0xFFFF0004, 0xEF090000, 0x22DB6, 0x0000001F } },
	{ "ArcTan 00008000",           { 0x9,     0x8000,          0,     0x16A2, 0xFFFF0000, 0xEF090000, 0x22D45, 0x0000001F } },
	{ "ArcTan 00008001",           { 0x9,     0x8001,          0,     0x1005, 0xFFFEFFFC, 0xEF090000, 0x22006, 0x0000001F } },
	{ "ArcTan 0000BFFF",           { 0x9,     0xBFFF,          0,     0x1F64, 0x0001C006, 0xEF090000, 0x17F33, 0x0000001F } },
	{ "ArcTan 0000C000",           { 0x9,     0xC000,          0, 0xFFFFC360, 0x0001C000, 0xEF090000, 0x10480, 0x0000001F } },
	{ "ArcTan 0000C001",           { 0x9,     0xC001,          0, 0xFFFFD550, 0x0001BFFA, 0xEF090000, 0x271BD, 0x0000001F } },
	{ "ArcTan 0000FFFF",           { 0x9,     0xFFFF,          0, 0xFFFFA2FE,          8, 0xEF090000,  0xA2FF, 0x0000001F } },
	{ "ArcTan FFFF0000",           { 0x9, 0xFFFF0000,          0,     0x5D07,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan FFFF0001",           { 0x9, 0xFFFF0001,          0,     0x5D01,          8, 0xEF090000,  0xA2FF, 0x0000001F } },
	{ "ArcTan FFFF3FFF",           { 0x9, 0xFFFF3FFF,          0,     0x2AAF, 0x0001BFFA, 0xEF090000, 0x271BD, 0x0000001F } },
	{ "ArcTan FFFF4000",           { 0x9, 0xFFFF4000,          0,     0x3CA0, 0x0001C000, 0xEF090000, 0x10480, 0x0000001F } },
	{ "ArcTan FFFF4001",           { 0x9, 0xFFFF4001,          0, 0xFFFFE09B, 0x0001C006, 0xEF090000, 0x17F33, 0x0000001F } },
	{ "ArcTan FFFF7FFF",           { 0x9, 0xFFFF7FFF,          0, 0xFFFFEFFA, 0xFFFEFFFC, 0xEF090000, 0x22006, 0x0000001F } },
	{ "ArcTan FFFF8000",           { 0x9, 0xFFFF8000,          0, 0xFFFFE95D, 0xFFFF0000, 0xEF090000, 0x22D45, 0x0000001F } },
	{ "ArcTan FFFF8001",           { 0x9, 0xFFFF8001,          0, 0xFFFFE927, 0xFFFF0004, 0xEF090000, 0x22DB6, 0x0000001F } },
	{ "ArcTan FFFFBFFF",           { 0x9, 0xFFFFBFFF,          0, 0xFFFFE000, 0xFFFFBFFE, 0xEF090000,  0x7FFE, 0x0000001F } },
	{ "ArcTan FFFFC000",           { 0x9, 0xFFFFC000,          0, 0xFFFFE000, 0xFFFFC000, 0xEF090000,  0x8000, 0x0000001F } },
	{ "ArcTan FFFFC001",           { 0x9, 0xFFFFC001,          0, 0xFFFFE000, 0xFFFFC002, 0xEF090000,  0x8001, 0x0000001F } },
	{ "ArcTan FFFFFFFF",           { 0x9, 0xFFFFFFFF,          0, 0xFFFFFFFF,          0, 0xEF090000,  0xA2F9, 0x0000001F } },
	{ "ArcTan2 00000000,00000000", { 0xA,          0,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,00000000", { 0xA,          1,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00003FFF,00000000", { 0xA,     0x3FFF,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,00000000", { 0xA,     0x4000,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004001,00000000", { 0xA,     0x4001,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00007FFF,00000000", { 0xA,     0x7FFF,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,00000000", { 0xA,     0x8000,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008001,00000000", { 0xA,     0x8001,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000BFFF,00000000", { 0xA,     0xBFFF,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,00000000", { 0xA,     0xC000,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C001,00000000", { 0xA,     0xC001,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,00000000", { 0xA,     0xFFFF,          0,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,00000000", { 0xA, 0xFFFF0000,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,00000000", { 0xA, 0xFFFF0001,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF3FFF,00000000", { 0xA, 0xFFFF3FFF,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,00000000", { 0xA, 0xFFFF4000,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4001,00000000", { 0xA, 0xFFFF4001,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF7FFF,00000000", { 0xA, 0xFFFF7FFF,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,00000000", { 0xA, 0xFFFF8000,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8001,00000000", { 0xA, 0xFFFF8001,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFBFFF,00000000", { 0xA, 0xFFFFBFFF,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,00000000", { 0xA, 0xFFFFC000,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC001,00000000", { 0xA, 0xFFFFC001,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,00000000", { 0xA, 0xFFFFFFFF,          0,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,00000001", { 0xA,          0,          1,     0x4000,          1, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,00000001", { 0xA,          1,          1,     0x2000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,00000001", { 0xA,     0x4000,          1,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,00000001", { 0xA,     0x8000,          1,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,00000001", { 0xA,     0xC000,          1,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,00000001", { 0xA,     0xFFFF,          1,          0,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,00000001", { 0xA, 0xFFFF0000,          1,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,00000001", { 0xA, 0xFFFF4000,          1,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,00000001", { 0xA, 0xFFFF8000,          1,     0x8000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,00000001", { 0xA, 0xFFFFC000,          1,     0x7FFF,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,00000001", { 0xA, 0xFFFFFFFF,          1,     0x6000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,00004000", { 0xA,          0,     0x4000,     0x4000,     0x4000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,00004000", { 0xA,          1,     0x4000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,00004000", { 0xA,     0x4000,     0x4000,     0x2000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,00004000", { 0xA,     0x8000,     0x4000,     0x12E4, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,00004000", { 0xA,     0xC000,     0x4000,      0xD1B, 0xFFFFF8E4, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,00004000", { 0xA,     0xFFFF,     0x4000,      0x9FB, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,00004000", { 0xA, 0xFFFF0000,     0x4000,     0x7604, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,00004000", { 0xA, 0xFFFF0001,     0x4000,     0x7604, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,00004000", { 0xA, 0xFFFF4000,     0x4000,     0x72E4, 0xFFFFF8E4, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,00004000", { 0xA, 0xFFFF8000,     0x4000,     0x6D1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,00004000", { 0xA, 0xFFFFC000,     0x4000,     0x6000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,00004000", { 0xA, 0xFFFFFFFF,     0x4000,     0x4001,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,00008000", { 0xA,          0,     0x8000,     0x4000,     0x8000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,00008000", { 0xA,          1,     0x8000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,00008000", { 0xA,     0x4000,     0x8000,     0x2D1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,00008000", { 0xA,     0x8000,     0x8000,     0x2000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,00008000", { 0xA,     0xC000,     0x8000,     0x17F4, 0xFFFFE390, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,00008000", { 0xA,     0xFFFF,     0x8000,     0x12E4, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,00008000", { 0xA, 0xFFFF0000,     0x8000,     0x6D1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,00008000", { 0xA, 0xFFFF0001,     0x8000,     0x6D1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,00008000", { 0xA, 0xFFFF4000,     0x8000,     0x680B, 0xFFFFE390, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,00008000", { 0xA, 0xFFFF8000,     0x8000,     0x6000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,00008000", { 0xA, 0xFFFFC000,     0x8000,     0x52E4, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,00008000", { 0xA, 0xFFFFFFFF,     0x8000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,0000C000", { 0xA,          0,     0xC000,     0x4000,     0xC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,0000C000", { 0xA,          1,     0xC000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,0000C000", { 0xA,     0x4000,     0xC000,     0x32E5, 0xFFFFF8E4, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,0000C000", { 0xA,     0x8000,     0xC000,     0x280C, 0xFFFFE390, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,0000C000", { 0xA,     0xC000,     0xC000,     0x2000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,0000C000", { 0xA,     0xFFFF,     0xC000,     0x1A37, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,0000C000", { 0xA, 0xFFFF0000,     0xC000,     0x65C8, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,0000C000", { 0xA, 0xFFFF0001,     0xC000,     0x65C8, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,0000C000", { 0xA, 0xFFFF4000,     0xC000,     0x6000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,0000C000", { 0xA, 0xFFFF8000,     0xC000,     0x57F5, 0xFFFFE390, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,0000C000", { 0xA, 0xFFFFC000,     0xC000,     0x4D1C, 0xFFFFF8E4, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,0000C000", { 0xA, 0xFFFFFFFF,     0xC000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,00010000", { 0xA,          0,    0x10000,     0x4000,    0x10000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,00010000", { 0xA,          1,    0x10000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,00010000", { 0xA,     0x4000,    0x10000,     0x3605, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,00010000", { 0xA,     0x8000,    0x10000,     0x2D1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,00010000", { 0xA,     0xC000,    0x10000,     0x25C9, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,00010000", { 0xA,     0xFFFF,    0x10000,     0x2001, 0xFFFFC002, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,00010000", { 0xA, 0xFFFF0000,    0x10000,     0x6000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,00010000", { 0xA, 0xFFFF0001,    0x10000,     0x6000, 0xFFFFC002, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,00010000", { 0xA, 0xFFFF4000,    0x10000,     0x5A38, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,00010000", { 0xA, 0xFFFF8000,    0x10000,     0x52E4, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,00010000", { 0xA, 0xFFFFC000,    0x10000,     0x49FC, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,00010000", { 0xA, 0xFFFFFFFF,    0x10000,     0x4000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000000,FFFF0000", { 0xA,          0, 0xFFFF0000,     0xC000, 0xFFFF0000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00000001,FFFF0000", { 0xA,          1, 0xFFFF0000,     0xC000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00004000,FFFF0000", { 0xA,     0x4000, 0xFFFF0000,     0xC9FC, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 00008000,FFFF0000", { 0xA,     0x8000, 0xFFFF0000,     0xD2E4, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000C000,FFFF0000", { 0xA,     0xC000, 0xFFFF0000,     0xDA38, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 0000FFFF,FFFF0000", { 0xA,     0xFFFF, 0xFFFF0000,     0xE000, 0xFFFFC002, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0000,FFFF0000", { 0xA, 0xFFFF0000, 0xFFFF0000,     0xA000, 0xFFFFC000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF0001,FFFF0000", { 0xA, 0xFFFF0001, 0xFFFF0000,     0xA001, 0xFFFFC002, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF4000,FFFF0000", { 0xA, 0xFFFF4000, 0xFFFF0000,     0xA5C9, 0xFFFFDC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFF8000,FFFF0000", { 0xA, 0xFFFF8000, 0xFFFF0000,     0xAD1C, 0xFFFFF000, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFC000,FFFF0000", { 0xA, 0xFFFFC000, 0xFFFF0000,     0xB605, 0xFFFFFC00, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "ArcTan2 FFFFFFFF,FFFF0000", { 0xA, 0xFFFFFFFF, 0xFFFF0000,     0xC000,          0, 0xEF0A0000,   0x170, 0x0000001F } },
	{ "Div 00000000/00000000",     { 0x6, 0x00000000, 0x00000000,          1,          0, 0xEF060000,       1, 0x0000001F } },
	{ "Div 00000001/00000000",     { 0x6, 0x00000001, 0x00000000,          1,          1, 0xEF060000,       1, 0x0000001F } },
	{ "Div FFFFFFFF/00000000",     { 0x6, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xEF060000,       1, 0x0000001F } },
	{ "Div 00000000/00000000",     { 0x6, 0x00000000, 0x00000000,          1,          0, 0xEF060000,       1, 0x0000001F } },
	{ "Div 00000001/00000000",     { 0x6, 0x00000001, 0x00000000,          1,          1, 0xEF060000,       1, 0x0000001F } },
	{ "Div 80000000/FFFFFFFF",     { 0x6, 0x80000000, 0xFFFFFFFF, 0x80000000,          0, 0xEF060000, 0x80000000, 0x0001F } },
};

static const u32 nMathTests = sizeof(mathTests) / sizeof(*mathTests);

static unsigned passes;
static unsigned totalResults;

IWRAM_CODE
__attribute__((noinline))
static void runTest(struct TestMath* test) {
	__asm__("ldr r0, =enterM \n"
		"bx r0 \n"
		".arm; enterM:\n"
		"ldr r0, %[in0] \n"
		"ldr r1, %[in1] \n"
		"orr r2, %[syscall], #0xEF00 \n"
		"lsl r2, r2, #16 \n"
		"str r2, [pc, #4] \n"
		"mov r3, #0 \n"
		"msr cpsr_f, #0 \n"
		"nop \n"
		"str r0, %[R0] \n"
		"str r1, %[R1] \n"
		"str r2, %[R2] \n"
		"str r3, %[R3] \n"
		"mrs %[psr], cpsr \n"
		"ldr r0, =exitM \n"
		"add r0, #1 \n"
		"bx r0 \n"
		".ltorg \n"
		".thumb; exitM:"
		: [R0]"=m"(test->outR0), [R1]"=m"(test->outR1), [R2]"=m"(test->outR2), [R3]"=m"(test->outR3), [psr]"=r"(test->outPsr)
		: [in0]"m"(test->in0), [in1]"m"(test->in1), [syscall]"r"(test->syscall)
		: "r0", "r1", "r2", "r3");
}

static void printResult(int offset, int line, const char* preface, s32 value, s32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-4s: %08lX", preface, value);
	if (value == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 15], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 15], 16, "!= %08lX", expected);
	}
}

static void doResult(const char* preface, const char* testName, s32 value, s32 expected) {
	if (value != expected) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("%s: Got %08lX vs %08lX: FAIL", preface, value, expected);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct TestMath* values, const struct TestMath* expected, int base) {
	snprintf(&textGrid[32], 31, "BIOS math test: %s", preface);
	printResult(base, 0, "r0", values->outR0, expected->outR0);
	printResult(base, 1, "r1", values->outR1, expected->outR1);
	printResult(base, 2, "r2", values->outR2, expected->outR2);
	printResult(base, 3, "r3", values->outR3, expected->outR3);
	printResult(base, 4, "cpsr", values->outPsr, expected->outPsr);
}

static void runMathSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct MathTest* activeTest = 0;
	int i;
	for (i = 0; i < nMathTests; ++i) {
		struct TestMath currentTest = {0};
		VBlankIntrWait();
		activeTest = &mathTests[i];
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		runTest(&currentTest);

		savprintf("Math test: %s", activeTest->testName);
		doResult("r0", activeTest->testName, currentTest.outR0, activeTest->expected.outR0);
		doResult("r1", activeTest->testName, currentTest.outR1, activeTest->expected.outR1);
		doResult("r2", activeTest->testName, currentTest.outR2, activeTest->expected.outR2);
		doResult("r3", activeTest->testName, currentTest.outR3, activeTest->expected.outR3);
		doResult("cpsr", activeTest->testName, currentTest.outPsr, activeTest->expected.outPsr);
	}
}

static size_t listMathSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nMathTests) {
			break;
		}
		names[i] = mathTests[i + offset].testName;
	}
	return i;
}

static void showMathSuite(size_t index) {
	const struct MathTest* activeTest = &mathTests[index];
	struct TestMath currentTest = {0};
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
			if (resultIndex < 4) {
				++resultIndex;
			}
		}
		memcpy(&currentTest, &activeTest->expected, sizeof(currentTest));
		runTest(&currentTest);
		printResults(activeTest->testName, &currentTest, &activeTest->expected, resultIndex);
		updateTextGrid();
	}
}

const struct TestSuite biosMathTestSuite = {
	.name = "BIOS math tests",
	.run = runMathSuite,
	.list = listMathSuite,
	.show = showMathSuite,
	.nTests = sizeof(mathTests) / sizeof(*mathTests),
	.passes = &passes,
	.totalResults = &totalResults
};
