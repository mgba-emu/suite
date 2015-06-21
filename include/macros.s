.set _arm_text, 0
.set _arm_ewram, 4
.set _arm_iwram, 8
.set _thumb_text, 12
.set _thumb_ewram, 16
.set _thumb_iwram, 20

#define _(...) __VA_ARGS__

#define TEST_SINGLE(NAME, MODE, SECTION, CODE, SETUP, TEARDOWN) \
	.MODE ;\
	.section .SECTION ;\
	.global NAME ## _ ## MODE ## _ ## SECTION ;\
	.type NAME ## _ ## MODE ## _ ## SECTION, %function ;\
	.align 4 ;\
	NAME ## _ ## MODE ## _ ## SECTION: ;\
	SETUP ;\
	START ;\
	CODE ;\
	TEARDOWN ;\
	END ;\
	bx lr

#define RUN_TEST(NAME, MODE, SECTION) \
	ldr r0, =NAME ## _ ## MODE ## _ ## SECTION ;\
	mov r1, pc ;\
	add r1, r1, #5 ;\
	mov lr, r1 ;\
	bx r0 ;\
	str r0, [r4, # _ ## MODE ## _ ## SECTION] ;\

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
	push {r4, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, arm, text) ;\
	RUN_TEST(NAME, thumb, text) ;\
	RUN_TEST(NAME, arm, iwram) ;\
	RUN_TEST(NAME, thumb, iwram) ;\
	RUN_TEST(NAME, arm, ewram) ;\
	RUN_TEST(NAME, thumb, ewram) ;\
	pop {r4} ;\
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
	push {r4, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, arm, text) ;\
	RUN_TEST(NAME, arm, iwram) ;\
	RUN_TEST(NAME, arm, ewram) ;\
	pop {r4} ;\
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
	push {r4, lr} ;\
	mov r4, r0 ;\
	RUN_TEST(NAME, thumb, text) ;\
	RUN_TEST(NAME, thumb, iwram) ;\
	RUN_TEST(NAME, thumb, ewram) ;\
	pop {r4} ;\
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

.set TM0CNT_LO, 0x4000100

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
