#include "macros.s"

#define TEST(NAME, CODE) \
	.text ;\
	.arm ;\
	.func NAME, NAME ;\
	.global NAME ;\
	.type NAME, %function ;\
	.balign 4 ;\
	NAME: \
	CODE; \
	bx lr; \
	.endfunc;

#define IMM_TEST(IMM) TEST(movImm ## IMM, _(movs r0, # ## 0x ## IMM))
#define SHIFTER_TEST(NAME, SHIFT) \
	TEST(movR1 ## NAME, _(movs r0, r1, SHIFT)) \
	TEST(movPC ## NAME, _(movs r0, pc, SHIFT)) \
	TEST(addR0PCR1 ## NAME, _(adds r0, pc, r1, SHIFT))

IMM_TEST(0)
IMM_TEST(FF)
IMM_TEST(FF000000)
IMM_TEST(F000000F)
TEST(movR1, _(movs r0, r1))
TEST(movPC, _(movs r0, pc))
TEST(addR0PCR1, _(adds r0, pc, r1))
SHIFTER_TEST(Lsl0, lsl #0)
SHIFTER_TEST(Lsl1, lsl #1)
SHIFTER_TEST(Lsl31, lsl #31)
SHIFTER_TEST(LslR0, lsl r0)
SHIFTER_TEST(Lsr1, lsr #1)
SHIFTER_TEST(Lsr31, lsr #31)
SHIFTER_TEST(Lsr32, lsr #32)
SHIFTER_TEST(LsrR0, lsr r0)
SHIFTER_TEST(Asr1, asr #1)
SHIFTER_TEST(Asr31, asr #31)
SHIFTER_TEST(Asr32, asr #32)
SHIFTER_TEST(AsrR0, asr r0)
SHIFTER_TEST(Ror1, ror #1)
SHIFTER_TEST(Ror31, ror #31)
SHIFTER_TEST(RorR0, ror r0)
SHIFTER_TEST(Rrx, rrx)
