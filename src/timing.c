#include "timing.h"

#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>

#include <stdio.h>
#include <string.h>

#include "suite.h"

struct TestTimings {
	s32 arm_text_0000;
	s32 arm_text_4000;
	s32 arm_text_0004;
	s32 arm_text_4004;
	s32 arm_text_0010;
	s32 arm_text_4010;
	s32 arm_text_0014;
	s32 arm_text_4014;
	s32 arm_ewram;
	s32 arm_iwram;
	s32 thumb_text_0000;
	s32 thumb_text_4000;
	s32 thumb_text_0004;
	s32 thumb_text_4004;
	s32 thumb_text_0010;
	s32 thumb_text_4010;
	s32 thumb_text_0014;
	s32 thumb_text_4014;
	s32 thumb_ewram;
	s32 thumb_iwram;
};

struct TimingTest {
	const char* testName;
	void (*test)(struct TestTimings*);
	int modes;
	struct TestTimings expected;
};

void calibrate(struct TestTimings*);
void testNop(struct TestTimings*);
void testNop2(struct TestTimings*);
void testLdrh(struct TestTimings*);
void testLdrhNop(struct TestTimings*);
void testNopLdrh(struct TestTimings*);
void testNopLdrhNop(struct TestTimings*);
void testLdrhRom(struct TestTimings*);
void testLdrhRomNop(struct TestTimings*);
void testNopLdrhRom(struct TestTimings*);
void testLdrRom(struct TestTimings*);
void testLdrRomNop(struct TestTimings*);
void testNopLdrRom(struct TestTimings*);
void testLdrh(struct TestTimings*);
void testLdrhNop(struct TestTimings*);
void testNopLdrh(struct TestTimings*);
void testStrh(struct TestTimings*);
void testStrhNop(struct TestTimings*);
void testNopStrhNop(struct TestTimings*);
void testNopStrh(struct TestTimings*);
void testLdrStr(struct TestTimings*);
void testLdrLdr(struct TestTimings*);
void testLdrRomLdr(struct TestTimings*);
void testLdrLdrRom(struct TestTimings*);
void testLdrRomLdrRom(struct TestTimings*);
void testStrLdr(struct TestTimings*);
void testStrStr(struct TestTimings*);
void testLdmia1(struct TestTimings*);
void testLdmia2(struct TestTimings*);
void testLdmia6(struct TestTimings*);
void testLdmia1x2(struct TestTimings*);
void testLdmia2x2(struct TestTimings*);
void testLdmia6x2(struct TestTimings*);
void testStmia1(struct TestTimings*);
void testStmia2(struct TestTimings*);
void testStmia6(struct TestTimings*);
void testStmia1x2(struct TestTimings*);
void testStmia2x2(struct TestTimings*);
void testStmia6x2(struct TestTimings*);
void testMul0(struct TestTimings*);
void testMul1(struct TestTimings*);
void testMul2(struct TestTimings*);
void testMul3(struct TestTimings*);
void testMul4(struct TestTimings*);
void testMul5(struct TestTimings*);
void testMul6(struct TestTimings*);
void testMul7(struct TestTimings*);
void testMul8(struct TestTimings*);
void testMul9(struct TestTimings*);
void testMla0(struct TestTimings*);
void testMla1(struct TestTimings*);
void testMla2(struct TestTimings*);
void testMla3(struct TestTimings*);
void testMla4(struct TestTimings*);
void testMla5(struct TestTimings*);
void testMla6(struct TestTimings*);
void testMla7(struct TestTimings*);
void testMla8(struct TestTimings*);
void testMla9(struct TestTimings*);
void testSmull0(struct TestTimings*);
void testSmull1(struct TestTimings*);
void testSmull2(struct TestTimings*);
void testSmull3(struct TestTimings*);
void testSmull4(struct TestTimings*);
void testSmull5(struct TestTimings*);
void testSmull6(struct TestTimings*);
void testSmull7(struct TestTimings*);
void testSmull8(struct TestTimings*);
void testSmull9(struct TestTimings*);
void testSmlal0(struct TestTimings*);
void testSmlal1(struct TestTimings*);
void testSmlal2(struct TestTimings*);
void testSmlal3(struct TestTimings*);
void testSmlal4(struct TestTimings*);
void testSmlal5(struct TestTimings*);
void testSmlal6(struct TestTimings*);
void testSmlal7(struct TestTimings*);
void testSmlal8(struct TestTimings*);
void testSmlal9(struct TestTimings*);
void testB(struct TestTimings*);
void testNopB(struct TestTimings*);
void testBx(struct TestTimings*);
void testTrivialLoop(struct TestTimings*);
void testCLoop(struct TestTimings*);
void testDiv(struct TestTimings*);
void testDiv2(struct TestTimings*);
void testSqrt(struct TestTimings*);
void testSqrt2(struct TestTimings*);
void testSqrt3(struct TestTimings*);
void testAtan(struct TestTimings*);
void testCpuSet(struct TestTimings*);
void testTrivialDma(struct TestTimings*);
void testTrivialDmaRom(struct TestTimings*);
void testTrivialDmaToRom(struct TestTimings*);
void testTrivialDmaRomRom(struct TestTimings*);
void testTrivial32Dma(struct TestTimings*);
void testTrivial32DmaRom(struct TestTimings*);
void testTrivial32DmaToRom(struct TestTimings*);
void testTrivial32DmaRomRom(struct TestTimings*);
void testShortDma(struct TestTimings*);
void testShortDmaRom(struct TestTimings*);
void testShortDmaToRom(struct TestTimings*);
void testShortDmaRomRom(struct TestTimings*);
void testShort32Dma(struct TestTimings*);
void testShort32DmaRom(struct TestTimings*);
void testShort32DmaToRom(struct TestTimings*);
void testShort32DmaRomRom(struct TestTimings*);

void longjump() {
}

void CLoop() {
	vs32 x = 0;
	int i;
	for (i = 0; i < 16; ++i) {
		x += i;
	}
}

static const struct TimingTest timingTests[] = {
	{ "Calibration", 0, TEST_ARM | TEST_THUMB, {
		7, 4, 6, 4, 6, 2, 5, 2,
		5, 0,
		4, 1, 3, 1, 4, 0, 3, 0,
		2, 0
	} },
	{ "nop", testNop, TEST_ARM | TEST_THUMB, {
		6, 6, 6, 6, 4, 4, 4, 4,
		6, 1,
		3, 3, 3, 3, 2, 2, 2, 2,
		3, 1
	} },
	{ "nop / nop", testNop2, TEST_ARM | TEST_THUMB, {
		12, 12, 12, 12, 8, 8, 8, 8,
		12, 2,
		6, 6, 6, 6, 4, 4, 4, 4,
		6, 2
	} },
	{ "ldrh r2, [sp]", testLdrh, TEST_ARM | TEST_THUMB, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
		7, 3, 6, 3, 7, 3, 6, 3,
		5, 3
	} },
	{ "ldrh r2, [sp] / nop", testLdrhNop, TEST_ARM | TEST_THUMB, {
		16, 12, 15, 12, 13, 8, 12, 8,
		14, 4,
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 4
	} },
	{ "nop / ldrh r2, [sp]", testNopLdrh, TEST_ARM | TEST_THUMB, {
		16, 12, 15, 12, 13, 8, 12, 8,
		14, 4,
		10, 6, 9, 6, 9, 5, 8, 5,
		8, 4
	} },
	{ "nop / ldrh r2, [sp] / nop", testNopLdrhNop, TEST_ARM | TEST_THUMB, {
		22, 18, 21, 18, 17, 12, 16, 12,
		20, 5,
		13, 9, 12, 9, 11, 6, 10, 6,
		11, 5
	} },
	{ "ldrh r2, [#0x08000000]", testLdrhRom, TEST_ARM | TEST_THUMB, {
		14, 14, 12, 12, 13, 13, 11, 11,
		12, 7,
		11, 11, 9, 9, 11, 11, 9, 9,
		9, 7
	} },
	{ "ldrh r2, [#0x08000000] / nop", testLdrhRomNop, TEST_ARM | TEST_THUMB, {
		20, 20, 18, 18, 17, 17, 15, 15,
		18, 8,
		14, 14, 12, 12, 13, 13, 11, 11,
		12, 8
	} },
	{ "nop / ldrh r2, [#0x08000000]", testNopLdrhRom, TEST_ARM | TEST_THUMB, {
		20, 20, 18, 18, 17, 17, 15, 15,
		18, 8,
		14, 14, 12, 12, 13, 13, 11, 11,
		12, 8
	} },
	{ "ldr r2, [#0x08000000]", testLdrRom, TEST_ARM | TEST_THUMB, {
		17, 17, 15, 15, 15, 15, 13, 13,
		15, 10,
		14, 14, 12, 12, 13, 13, 11, 11,
		12, 10
	} },
	{ "ldr r2, [#0x08000000] / nop", testLdrRomNop, TEST_ARM | TEST_THUMB, {
		23, 23, 21, 21, 19, 19, 17, 17,
		21, 11,
		17, 17, 15, 15, 15, 15, 13, 13,
		15, 11
	} },
	{ "nop / ldr r2, [#0x08000000]", testNopLdrRom, TEST_ARM | TEST_THUMB, {
		23, 23, 21, 21, 19, 19, 17, 17,
		21, 11,
		17, 17, 15, 15, 15, 15, 13, 13,
		15, 11
	} },
	{ "strh r3, [sp]", testStrh, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "strh r3, [sp] / nop", testStrhNop, TEST_ARM | TEST_THUMB, {
		15, 12, 14, 12, 12, 8, 11, 8,
		13, 3,
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 3
	} },
	{ "nop / strh r3, [sp]", testNopStrh, TEST_ARM | TEST_THUMB, {
		15, 12, 14, 12, 12, 8, 11, 8,
		13, 3,
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 3
	} },
	{ "nop / strh r3, [sp] / nop", testNopStrhNop, TEST_ARM | TEST_THUMB, {
		21, 18, 20, 18, 16, 12, 15, 12,
		19, 4,
		12, 9, 11, 9, 10, 6, 9, 6,
		10, 4
	} },
	{ "ldr r2, [sp] x2", testLdrLdr, TEST_ARM | TEST_THUMB, {
		20, 12, 18, 12, 18, 8, 16, 8,
		16, 6,
		14, 6, 12, 6, 14, 6, 12, 6,
		10, 6
	} },
	{ "ldr r2, [#0x08000000] / ldr r2, [sp]", testLdrRomLdr, TEST_ARM | TEST_THUMB, {
		27, 23, 24, 21, 24, 19, 21, 17,
		23, 13,
		21, 17, 18, 15, 20, 16, 17, 14,
		17, 13
	} },
	{ "ldr r2, [sp] / ldr r2, [#0x08000000]", testLdrLdrRom, TEST_ARM | TEST_THUMB, {
		27, 23, 24, 21, 24, 19, 21, 17,
		23, 13,
		21, 17, 18, 15, 20, 17, 17, 15,
		17, 13
	} },
	{ "ldr r2, [#0x08000000] x2", testLdrRomLdrRom, TEST_ARM | TEST_THUMB, {
		34, 34, 30, 30, 30, 30, 26, 26,
		30, 20,
		28, 28, 24, 24, 26, 26, 22, 22,
		24, 20
	} },
	{ "str r3, [sp] x2", testStrStr, TEST_ARM | TEST_THUMB, {
		18, 12, 16, 12, 16, 8, 14, 8,
		14, 4,
		12, 6, 10, 6, 12, 4, 10, 4,
		8, 4
	} },
	{ "ldr r2, [sp] / str r2, [sp]", testLdrStr, TEST_ARM | TEST_THUMB, {
		19, 12, 17, 12, 17, 8, 15, 8,
		15, 5,
		13, 6, 11, 6, 13, 5, 11, 5,
		9, 5
	} },
	{ "str r3, [sp] / ldr r3, [sp]", testStrLdr, TEST_ARM | TEST_THUMB, {
		19, 12, 17, 12, 17, 8, 15, 8,
		15, 5,
		13, 6, 11, 6, 13, 5, 11, 5,
		9, 5
	} },
	{ "ldmia sp, {r2}", testLdmia1, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3
	} },
	{ "ldmia sp, {r2, r3}", testLdmia2, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4
	} },
	{ "ldmia sp, {r2-r7}", testLdmia6, TEST_ARM, {
		15, 8, 14, 8, 14, 8, 13, 8,
		13, 8
	} },
	{ "ldmia sp, {r2} x2", testLdmia1x2, TEST_ARM, {
		20, 12, 18, 12, 18, 8, 16, 8,
		16, 6
	} },
	{ "ldmia sp, {r2, r3} x2", testLdmia2x2, TEST_ARM, {
		22, 12, 20, 12, 20, 8, 18, 8,
		18, 8
	} },
	{ "ldmia sp, {r2-r7} x2", testLdmia6x2, TEST_ARM, {
		30, 16, 28, 16, 28, 16, 26, 16,
		26, 16
	} },
	{ "stmia sp, {r2}", testStmia1, TEST_ARM, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2
	} },
	{ "stmia sp, {r2, r3}", testStmia2, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3
	} },
	{ "stmia sp, {r2-r7}", testStmia6, TEST_ARM, {
		14, 7, 13, 7, 13, 7, 12, 7,
		12, 7
	} },
	{ "stmia sp, {r2} x2", testStmia1x2, TEST_ARM, {
		18, 12, 16, 12, 16, 8, 14, 8,
		14, 4
	} },
	{ "stmia sp, {r2, r3} x2", testStmia2x2, TEST_ARM, {
		20, 12, 18, 12, 18, 8, 16, 8,
		16, 6
	} },
	{ "stmia sp, {r2-r7} x2", testStmia6x2, TEST_ARM, {
		28, 14, 26, 14, 26, 14, 24, 14,
		24, 14
	} },
	{ "mul #0x00000000, #0xFF", testMul0, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "mul #0x00000078, #0xFF", testMul1, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "mul #0x00005678, #0xFF", testMul2, TEST_ARM | TEST_THUMB, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
		7, 3, 6, 3, 7, 3, 6, 3,
		5, 3
	} },
	{ "mul #0x00345678, #0xFF", testMul3, TEST_ARM | TEST_THUMB, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
		8, 4, 7, 4, 8, 4, 7, 4,
		6, 4
	} },
	{ "mul #0x12345678, #0xFF", testMul4, TEST_ARM | TEST_THUMB, {
		12, 6, 11, 6, 11, 5, 10, 5,
		10, 5,
		9, 5, 8, 5, 9, 5, 8, 5,
		7, 5
	} },
	{ "mul #0xFF000000, #0xFF", testMul5, TEST_ARM | TEST_THUMB, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
		8, 4, 7, 4, 8, 4, 7, 4,
		6, 4
	} },
	{ "mul #0xFFFF0000, #0xFF", testMul6, TEST_ARM | TEST_THUMB, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
		7, 3, 6, 3, 7, 3, 6, 3,
		5, 3
	} },
	{ "mul #0xFFFFFF00, #0xFF", testMul7, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "mul #0xFFFFFFFF, #0xFF", testMul8, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "mul #0xFFFFFFFF, #0x00", testMul9, TEST_ARM | TEST_THUMB, {
		9, 6, 8, 6, 8, 4, 7, 4,
		7, 2,
		6, 3, 5, 3, 6, 2, 5, 2,
		4, 2
	} },
	{ "mla #0x00000000, #0xFF", testMla0, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0x00000078, #0xFF", testMla1, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0x00005678, #0xFF", testMla2, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0x00345678, #0xFF", testMla3, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0x12345678, #0xFF", testMla4, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0xFF000000, #0xFF", testMla5, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0xFFFF0000, #0xFF", testMla6, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0xFFFFFF00, #0xFF", testMla7, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0xFFFFFFFF, #0xFF", testMla8, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "mla #0xFFFFFFFF, #0x00", testMla9, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0x00000000, #0xFF", testSmull0, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0x00000078, #0xFF", testSmull1, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0x00005678, #0xFF", testSmull2, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0x00345678, #0xFF", testSmull3, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0x12345678, #0xFF", testSmull4, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0xFF000000, #0xFF", testSmull5, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0xFFFF0000, #0xFF", testSmull6, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0xFFFFFF00, #0xFF", testSmull7, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0xFFFFFFFF, #0xFF", testSmull8, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smull #0xFFFFFFFF, #0x00", testSmull9, TEST_ARM, {
		10, 6, 9, 6, 9, 4, 8, 4,
		8, 3,
	} },
	{ "smlal #0x00000000, #0xFF", testSmlal0, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0x00000078, #0xFF", testSmlal1, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0x00005678, #0xFF", testSmlal2, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0x00345678, #0xFF", testSmlal3, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0x12345678, #0xFF", testSmlal4, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0xFF000000, #0xFF", testSmlal5, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0xFFFF0000, #0xFF", testSmlal6, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0xFFFFFF00, #0xFF", testSmlal7, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0xFFFFFFFF, #0xFF", testSmlal8, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "smlal #0xFFFFFFFF, #0x00", testSmlal9, TEST_ARM, {
		11, 6, 10, 6, 10, 4, 9, 4,
		9, 4,
	} },
	{ "b pc", testB, TEST_ARM | TEST_THUMB, {
		26, 26, 25, 25, 19, 19, 18, 18,
		24, 4,
		14, 14, 13, 13, 11, 11, 10, 10,
		12, 4
	} },
	{ "nop ; b pc", testB, TEST_ARM | TEST_THUMB, {
		26, 26, 25, 25, 19, 19, 18, 18,
		24, 4,
		14, 14, 13, 13, 11, 11, 10, 10,
		12, 4
	} },
	{ "bx", testBx, TEST_ARM | TEST_THUMB, {
		78, 78, 74, 74, 59, 59, 55, 55,
		72, 22,
		57, 57, 53, 53, 45, 45, 41, 41,
		51, 24
	} },
	{ "Trivial loop", testTrivialLoop, TEST_ARM | TEST_THUMB, {
		510, 510, 495, 495, 365, 365, 350, 350,
		480, 80,
		270, 270, 255, 255, 205, 205, 190, 190,
		240, 80
	} },
	{ "C loop", testCLoop, TEST_ARM | TEST_THUMB, {
		346, 231, 309, 227, 309, 161, 272, 157,
		340, 290,
		325, 210, 288, 206, 295, 147, 258, 143,
		319, 292
	} },
	{ "BIOS Division", testDiv, TEST_ARM | TEST_THUMB, {
		398, 398, 394, 394, 381, 381, 377, 377,
		390, 338,
		371, 371, 367, 367, 363, 363, 359, 359,
		363, 338
	} },
	{ "BIOS Division 2", testDiv2, TEST_ARM | TEST_THUMB, {
		138, 138, 134, 134, 121, 121, 117, 117,
		130, 78,
		111, 111, 107, 107, 103, 103, 99, 99,
		103, 78
	} },
	{ "BIOS Sqrt", testSqrt, TEST_ARM | TEST_THUMB, {
		150, 150, 148, 148, 135, 135, 133, 133,
		146, 104,
		126, 126, 124, 124, 119, 119, 117, 117,
		122, 104
	} },
	{ "BIOS Sqrt 2", testSqrt2, TEST_ARM | TEST_THUMB, {
		265, 265, 263, 263, 250, 250, 248, 248,
		261, 219,
		241, 241, 239, 239, 234, 234, 232, 232,
		237, 219
	} },
	{ "BIOS Sqrt 3", testSqrt3, TEST_ARM | TEST_THUMB, {
		1192, 1192, 1188, 1188, 1177, 1177, 1173, 1173,
		1184, 1137,
		1168, 1168, 1164, 1164, 1161, 1161, 1157, 1157,
		1160, 1137
	} },
	{ "BIOS ArcTan", testAtan, TEST_ARM | TEST_THUMB, {
		150, 150, 148, 148, 135, 135, 133, 133,
		146, 104,
		126, 126, 124, 124, 119, 119, 117, 117,
		122, 104
	} },
	{ "CpuSet", testCpuSet, TEST_ARM | TEST_THUMB, {
		3453, 3453, 3451, 3451, 3434, 3434, 3432, 3432,
		3449, 3397,
		3456, 3456, 3448, 3448, 3447, 3447, 3439, 3439,
		3440, 3403
	} },
	{ "Trivial DMA (16)", testTrivialDma, TEST_ARM | TEST_THUMB, {
		13, 10, 12, 10, 12, 8, 11, 8,
		11, 2,
		10, 7, 9, 7, 10, 2, 9, 2,
		8, 2
	} },
	{ "Trivial DMA (16/ROM)", testTrivialDmaRom, TEST_ARM | TEST_THUMB, {
		17, 14, 15, 13, 16, 13, 14, 12,
		15, 2,
		14, 11, 12, 10, 14, 2, 12, 2,
		12, 2
	} },
	{ "Trivial DMA (16/to ROM)", testTrivialDmaToRom, TEST_ARM | TEST_THUMB, {
		17, 15, 15, 14, 16, 12, 14, 11,
		15, 2,
		14, 12, 12, 11, 14, 2, 12, 2,
		12, 2
	} },
	{ "Trivial DMA (16/ROM to ROM)", testTrivialDmaRomRom, TEST_ARM | TEST_THUMB, {
		19, 16, 17, 15, 17, 14, 15, 13,
		17, 2,
		16, 13, 14, 12, 15, 2, 13, 2,
		14, 2
	} },
	{ "Trivial DMA (32)", testTrivial32Dma, TEST_ARM | TEST_THUMB, {
		13, 10, 12, 10, 12, 8, 11, 8,
		11, 2,
		10, 7, 9, 7, 10, 2, 9, 2,
		8, 2
	} },
	{ "Trivial DMA (32/from ROM)", testTrivial32DmaRom, TEST_ARM | TEST_THUMB, {
		20, 17, 18, 16, 18, 15, 16, 14,
		18, 2,
		17, 14, 15, 13, 16, 2, 14, 2,
		15, 2
	} },
	{ "Trivial DMA (32/to ROM)", testTrivial32DmaToRom, TEST_ARM | TEST_THUMB, {
		20, 18, 18, 17, 18, 14, 16, 13,
		18, 2,
		17, 15, 15, 14, 16, 2, 14, 2,
		15, 2
	} },
	{ "Trivial DMA (32/ROM to ROM)", testTrivial32DmaRomRom, TEST_ARM | TEST_THUMB, {
		25, 22, 23, 21, 21, 18, 19, 17,
		23, 2,
		22, 19, 20, 18, 19, 2, 17, 2,
		20, 2
	} },
	{ "Short DMA (16)", testShortDma, TEST_ARM | TEST_THUMB, {
		43, 40, 42, 40, 42, 38, 41, 38,
		41, 2,
		40, 37, 39, 37, 40, 2, 39, 2,
		38, 2
	} },
	{ "Short DMA (16/from ROM)", testShortDmaRom, TEST_ARM | TEST_THUMB, {
		77, 74, 75, 73, 61, 58, 59, 57,
		75, 2,
		74, 71, 72, 70, 59, 2, 57, 2,
		72, 2
	} },
	{ "Short DMA (16/to ROM)", testShortDmaToRom, TEST_ARM | TEST_THUMB, {
		77, 75, 75, 74, 61, 57, 59, 56,
		75, 2,
		74, 72, 72, 71, 59, 2, 57, 2,
		72, 2
	} },
	{ "Short DMA (16/ROM to ROM)", testShortDmaRomRom, TEST_ARM | TEST_THUMB, {
		109, 106, 107, 105, 77, 74, 75, 73,
		107, 2,
		106, 103, 104, 102, 75, 2, 73, 2,
		104, 2
	} },
	{ "Short DMA (32)", testShort32Dma, TEST_ARM | TEST_THUMB, {
		43, 40, 42, 40, 42, 38, 41, 38,
		41, 2,
		40, 37, 39, 37, 40, 2, 39, 2,
		38, 2
	} },
	{ "Short DMA (32/from ROM)", testShort32DmaRom, TEST_ARM | TEST_THUMB, {
		125, 122, 123, 121, 93, 90, 91, 89,
		123, 2,
		122, 119, 120, 118, 91, 2, 89, 2,
		120, 2
	} },
	{ "Short DMA (32/to ROM)", testShort32DmaToRom, TEST_ARM | TEST_THUMB, {
		125, 123, 123, 122, 93, 89, 91, 88,
		123, 2,
		122, 120, 120, 119, 91, 2, 89, 2,
		120, 2
	} },
	{ "Short DMA (32/ROM to ROM)", testShort32DmaRomRom, TEST_ARM | TEST_THUMB, {
		205, 202, 203, 201, 141, 138, 139, 137,
		203, 2,
		202, 199, 200, 198, 139, 2, 137, 2,
		200, 2
	} },
};

static const u32 nTimingTests = sizeof(timingTests) / sizeof(*timingTests);

static struct TestTimings calibration;
static unsigned passes;
static unsigned totalResults;

static void printResult(int offset, int line, const char* preface, s32 value, s32 calibration, s32 expected) {
	static const int base = 96;
	if (offset > line || base + 32 * (line - offset) > 576) {
		return;
	}

	snprintf(&textGrid[base + 32 * (line - offset)], 31, "%-13s: %5li", preface, value - calibration);
	if (value - calibration == expected) {
		strncpy(&textGrid[base + 32 * (line - offset) + 21], "PASS", 10);
	} else {
		snprintf(&textGrid[base + 32 * (line - offset) + 21], 10, "!= %5li", expected);
	}
}

static void doResult(const char* preface, const char* testName, s32 value, s32 calibration, s32 expected) {
	if (value - calibration != expected) {
		debugprintf("FAIL: %s %s", testName, preface);
		savprintf("%s: Got %5li vs %5li: FAIL", preface, value - calibration, expected);
	} else {
		debugprintf("PASS: %s %s", testName, preface);
		++passes;
	}
	++totalResults;
}

static void printResults(const char* preface, const struct TestTimings* values, const struct TestTimings* calibration, const struct TestTimings* expected, int mode, int base) {
	snprintf(&textGrid[32], 31, "Timing test: %s", preface);

	printResult(base, 0, "ARM/ROM ...", values->arm_text_0000, calibration->arm_text_0000, expected->arm_text_0000);
	printResult(base, 1, "ARM/ROM P..", values->arm_text_4000, calibration->arm_text_4000, expected->arm_text_4000);
	printResult(base, 2, "ARM/ROM .N.", values->arm_text_0004, calibration->arm_text_0004, expected->arm_text_0004);
	printResult(base, 3, "ARM/ROM PN.", values->arm_text_4004, calibration->arm_text_4004, expected->arm_text_4004);
	printResult(base, 4, "ARM/ROM ..S", values->arm_text_0010, calibration->arm_text_0010, expected->arm_text_0010);
	printResult(base, 5, "ARM/ROM P.S", values->arm_text_4010, calibration->arm_text_4010, expected->arm_text_4010);
	printResult(base, 6, "ARM/ROM .NS", values->arm_text_0014, calibration->arm_text_0014, expected->arm_text_0014);
	printResult(base, 7, "ARM/ROM PNS", values->arm_text_4014, calibration->arm_text_4014, expected->arm_text_4014);
	printResult(base, 8, "ARM/WRAM", values->arm_ewram, calibration->arm_ewram, expected->arm_ewram);
	printResult(base, 9, "ARM/IWRAM", values->arm_iwram, calibration->arm_iwram, expected->arm_iwram);

	if (mode & TEST_THUMB) {
		printResult(base, 10, "Thumb/ROM ...", values->thumb_text_0000, calibration->thumb_text_0000, expected->thumb_text_0000);
		printResult(base, 11, "Thumb/ROM P..", values->thumb_text_4000, calibration->thumb_text_4000, expected->thumb_text_4000);
		printResult(base, 12, "Thumb/ROM .N.", values->thumb_text_0004, calibration->thumb_text_0004, expected->thumb_text_0004);
		printResult(base, 13, "Thumb/ROM PN.", values->thumb_text_4004, calibration->thumb_text_4004, expected->thumb_text_4004);
		printResult(base, 14, "Thumb/ROM ..S", values->thumb_text_0010, calibration->thumb_text_0010, expected->thumb_text_0010);
		printResult(base, 15, "Thumb/ROM P.S", values->thumb_text_4010, calibration->thumb_text_4010, expected->thumb_text_4010);
		printResult(base, 16, "Thumb/ROM .NS", values->thumb_text_0014, calibration->thumb_text_0014, expected->thumb_text_0014);
		printResult(base, 17, "Thumb/ROM PNS", values->thumb_text_4014, calibration->thumb_text_4014, expected->thumb_text_4014);
		printResult(base, 18, "Thumb/WRAM", values->thumb_ewram, calibration->thumb_ewram, expected->thumb_ewram);
		printResult(base, 19, "Thumb/IWRAM", values->thumb_iwram, calibration->thumb_iwram, expected->thumb_iwram);
	}
}

static void runTimingSuite(void) {
	passes = 0;
	totalResults = 0;
	const struct TimingTest* activeTest = 0;
	int i;
	for (i = 0; i < nTimingTests; ++i) {
		struct TestTimings currentTest = {0};
		VBlankIntrWait();
		REG_IME = 0;
		calibrate(&calibration);
		activeTest = &timingTests[i];
		if (activeTest->test) {
			activeTest->test(&currentTest);
		} else {
			currentTest = calibration;
			memset(&calibration, 0, sizeof(calibration));
		}
		REG_IME = 1;

		savprintf("Timing test: %s", activeTest->testName);
		doResult("ARM/ROM ...", activeTest->testName, currentTest.arm_text_0000, calibration.arm_text_0000, activeTest->expected.arm_text_0000);
		doResult("ARM/ROM P..", activeTest->testName, currentTest.arm_text_4000, calibration.arm_text_4000, activeTest->expected.arm_text_4000);
		doResult("ARM/ROM .N.", activeTest->testName, currentTest.arm_text_0004, calibration.arm_text_0004, activeTest->expected.arm_text_0004);
		doResult("ARM/ROM PN.", activeTest->testName, currentTest.arm_text_4004, calibration.arm_text_4004, activeTest->expected.arm_text_4004);
		doResult("ARM/ROM ..S", activeTest->testName, currentTest.arm_text_0010, calibration.arm_text_0010, activeTest->expected.arm_text_0010);
		doResult("ARM/ROM P.S", activeTest->testName, currentTest.arm_text_4010, calibration.arm_text_4010, activeTest->expected.arm_text_4010);
		doResult("ARM/ROM .NS", activeTest->testName, currentTest.arm_text_0014, calibration.arm_text_0014, activeTest->expected.arm_text_0014);
		doResult("ARM/ROM PNS", activeTest->testName, currentTest.arm_text_4014, calibration.arm_text_4014, activeTest->expected.arm_text_4014);
		doResult("ARM/WRAM", activeTest->testName, currentTest.arm_ewram, calibration.arm_ewram, activeTest->expected.arm_ewram);
		doResult("ARM/IWRAM", activeTest->testName, currentTest.arm_iwram, calibration.arm_iwram, activeTest->expected.arm_iwram);

		if (activeTest->modes & TEST_THUMB) {
			doResult("Thumb/ROM ...", activeTest->testName, currentTest.thumb_text_0000, calibration.thumb_text_0000, activeTest->expected.thumb_text_0000);
			doResult("Thumb/ROM P..", activeTest->testName, currentTest.thumb_text_4000, calibration.thumb_text_4000, activeTest->expected.thumb_text_4000);
			doResult("Thumb/ROM .N.", activeTest->testName, currentTest.thumb_text_0004, calibration.thumb_text_0004, activeTest->expected.thumb_text_0004);
			doResult("Thumb/ROM PN.", activeTest->testName, currentTest.thumb_text_4004, calibration.thumb_text_4004, activeTest->expected.thumb_text_4004);
			doResult("Thumb/ROM ..S", activeTest->testName, currentTest.thumb_text_0010, calibration.thumb_text_0010, activeTest->expected.thumb_text_0010);
			doResult("Thumb/ROM P.S", activeTest->testName, currentTest.thumb_text_4010, calibration.thumb_text_4010, activeTest->expected.thumb_text_4010);
			doResult("Thumb/ROM .NS", activeTest->testName, currentTest.thumb_text_0014, calibration.thumb_text_0014, activeTest->expected.thumb_text_0014);
			doResult("Thumb/ROM PNS", activeTest->testName, currentTest.thumb_text_4014, calibration.thumb_text_4014, activeTest->expected.thumb_text_4014);
			doResult("Thumb/WRAM", activeTest->testName, currentTest.thumb_ewram, calibration.thumb_ewram, activeTest->expected.thumb_ewram);
			doResult("Thumb/IWRAM", activeTest->testName, currentTest.thumb_iwram, calibration.thumb_iwram, activeTest->expected.thumb_iwram);
		}
	}
}

static size_t listTimingSuite(const char** names, size_t size, size_t offset) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (i + offset >= nTimingTests) {
			break;
		}
		names[i] = timingTests[i + offset].testName;
	}
	return i;
}

static void showTimingSuite(size_t index) {
	const struct TimingTest* activeTest = &timingTests[index];
	struct TestTimings currentTest = {0};
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
		if (activeTest->test) {
			activeTest->test(&currentTest);
			printResults(activeTest->testName, &currentTest, &calibration, &activeTest->expected, activeTest->modes, resultIndex);
		} else {
			printResults(activeTest->testName, &calibration, &currentTest, &activeTest->expected, activeTest->modes, resultIndex);
		}
		updateTextGrid();
	}
}

const struct TestSuite timingTestSuite = {
	.name = "Timing tests",
	.run = runTimingSuite,
	.list = listTimingSuite,
	.show = showTimingSuite,
	.nTests = sizeof(timingTests) / sizeof(*timingTests),
	.passes = &passes,
	.totalResults = &totalResults
};
