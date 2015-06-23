#define DMA3SAD 0x40000D4
#define TM0CNT_LO 0x4000100
#define WAITCNT 0x4000204
#define IME 0x4000208
#define _arm_text_0000 0
#define _arm_text_4000 4
#define _arm_text_0004 8
#define _arm_text_4004 12
#define _arm_text_0010 16
#define _arm_text_4010 20
#define _arm_text_0014 24
#define _arm_text_4014 28
#define _arm_ewram_0000 32
#define _arm_iwram_0000 36
#define _thumb_text_0000 40
#define _thumb_text_4000 44
#define _thumb_text_0004 48
#define _thumb_text_4004 52
#define _thumb_text_0010 56
#define _thumb_text_4010 60
#define _thumb_text_0014 64
#define _thumb_text_4014 68
#define _thumb_ewram_0000 72
#define _thumb_iwram_0000 76

#define _(...) __VA_ARGS__

#define TEST_SINGLE(NAME, MODE, SECTION, CODE, SETUP, TEARDOWN) \
	.text ;\
	.MODE ;\
	.section .SECTION ;\
	.func NAME, NAME ;\
	.global NAME ## _ ## MODE ## _ ## SECTION ;\
	.type NAME ## _ ## MODE ## _ ## SECTION, %function ;\
	.balign 4 ;\
	NAME ## _ ## MODE ## _ ## SECTION: ;\
	SETUP ;\
	START ;\
	CODE ;\
	END ;\
	TEARDOWN ;\
	bx lr ;\
	.endfunc ;\
	.ltorg

#define RUN_TEST(NAME, MODE, SECTION, WAIT) \
	ldr r0, =0x ## WAIT ; \
	ldr r1, =WAITCNT ;\
	str r0, [r1] ;\
	ldr r0, =IME ;\
	mov r1, #0 ;\
	ldr r5, [r0] ;\
	str r1, [r0] ;\
	ldr r0, =NAME ## _ ## MODE ## _ ## SECTION ;\
	mov r1, pc ;\
	add r1, r1, #5 ;\
	mov lr, r1 ;\
	bx r0 ;\
	str r0, [r4, # _ ## MODE ## _ ## SECTION ## _ ## WAIT] ;\
	ldr r0, =IME ;\
	str r5, [r0]

#define TEST_ALL(NAME, CODE, SETUP, TEARDOWN) \
	TEST_SINGLE(NAME, arm, text, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, thumb, text, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, arm, iwram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, thumb, iwram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, arm, ewram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, thumb, ewram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	.thumb ;\
	.section .text ;\
	.global NAME ;\
	.type NAME, %function ;\
	NAME: ;\
	push {r4-r5, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, arm, text, 0000) ;\
	RUN_TEST(NAME, arm, text, 4000) ;\
	RUN_TEST(NAME, arm, text, 0004) ;\
	RUN_TEST(NAME, arm, text, 4004) ;\
	RUN_TEST(NAME, arm, text, 0010) ;\
	RUN_TEST(NAME, arm, text, 4010) ;\
	RUN_TEST(NAME, arm, text, 0014) ;\
	RUN_TEST(NAME, arm, text, 4014) ;\
	RUN_TEST(NAME, arm, iwram, 0000) ;\
	RUN_TEST(NAME, arm, ewram, 0000) ;\
	RUN_TEST(NAME, thumb, text, 0000) ;\
	RUN_TEST(NAME, thumb, text, 4000) ;\
	RUN_TEST(NAME, thumb, text, 0004) ;\
	RUN_TEST(NAME, thumb, text, 4004) ;\
	RUN_TEST(NAME, thumb, text, 0010) ;\
	RUN_TEST(NAME, thumb, text, 4010) ;\
	RUN_TEST(NAME, thumb, text, 0014) ;\
	RUN_TEST(NAME, thumb, text, 4014) ;\
	RUN_TEST(NAME, thumb, iwram, 0000) ;\
	RUN_TEST(NAME, thumb, ewram, 0000) ;\
	pop {r4-r5} ;\
	pop {r0} ;\
	bx r0

#define TEST_ARM(NAME, CODE, SETUP, TEARDOWN) \
	TEST_SINGLE(NAME, arm, text, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, arm, iwram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, arm, ewram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	.thumb ;\
	.section .text ;\
	.global NAME ;\
	.type NAME, %function ;\
	NAME: ;\
	push {r4-r5, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, arm, text, 0000) ;\
	RUN_TEST(NAME, arm, text, 4000) ;\
	RUN_TEST(NAME, arm, text, 0004) ;\
	RUN_TEST(NAME, arm, text, 4004) ;\
	RUN_TEST(NAME, arm, text, 0010) ;\
	RUN_TEST(NAME, arm, text, 4010) ;\
	RUN_TEST(NAME, arm, text, 0014) ;\
	RUN_TEST(NAME, arm, text, 4014) ;\
	RUN_TEST(NAME, arm, iwram, 0000) ;\
	RUN_TEST(NAME, arm, ewram, 0000) ;\
	pop {r4-r5} ;\
	pop {r0} ;\
	bx r0

#define TEST_THUMB(NAME, CODE, SETUP, TEARDOWN) \
	TEST_SINGLE(NAME, thumb, text, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, thumb, iwram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	TEST_SINGLE(NAME, thumb, ewram, _(CODE), _(SETUP), _(TEARDOWN)) ;\
	.thumb ;\
	.section .text ;\
	.global NAME ;\
	.type NAME, %function ;\
	NAME: ;\
	push {r4-r5, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, thumb, text, 0000) ;\
	RUN_TEST(NAME, thumb, text, 4000) ;\
	RUN_TEST(NAME, thumb, text, 0004) ;\
	RUN_TEST(NAME, thumb, text, 4004) ;\
	RUN_TEST(NAME, thumb, text, 0010) ;\
	RUN_TEST(NAME, thumb, text, 4010) ;\
	RUN_TEST(NAME, thumb, text, 0014) ;\
	RUN_TEST(NAME, thumb, text, 4014) ;\
	RUN_TEST(NAME, thumb, iwram, 0000) ;\
	RUN_TEST(NAME, thumb, ewram, 0000) ;\
	pop {r4-r5} ;\
	pop {r0} ;\
	bx r0

#define TEST_VARIANT(NAME, ARM, THUMB) \
	.thumb ;\
	.section .text ;\
	.global NAME ;\
	.type NAME, %function ;\
	NAME: ;\
	push {r4, lr} ;\
	mov r4, r0 ;\
	bl ARM ;\
	mov r0, r4 ;\
	bl THUMB ;\
	pop {r4} ;\
	pop {r0} ;\
	bx r0

.macro START
ldr r0, =TM0CNT_LO
ldr r1, =0x800000
str r1, [r0]
.endm

.macro END
ldrh r2, [r0]
strh r1, [r0, #2]
mov r0, r2
.endm
