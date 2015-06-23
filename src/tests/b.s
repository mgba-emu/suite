#include "macros.s"

TEST_ALL(testB, b 1f ; nop ; 1: nop, ,)
TEST_ALL(testNopB, nop ; b 1f ; nop ; 1: nop, ,)

TEST_ALL(testTrivialLoop, _(
	mov r2, #0 ;
	mov r3, #16 ;
	1: add r2, #1 ;
	cmp r2, r3 ;
	bne 1b ;
), , )

TEST_THUMB(testBxThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r2, =longjump ;
	mov r3, pc ;
	add r3, r3, #5 ;
	mov lr, r3 ;
	bx r2 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(mov r3, lr ; push {r3-r5}), _(pop {r3-r5} ; mov lr, r3))

TEST_ARM(testBxArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r2, =longjump ;
	mov lr, pc ;
	bx r2 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5, lr}), _(pop {r4, r5, lr}))

TEST_VARIANT(testBx, testBxArm, testBxThumb)

TEST_THUMB(testCLoopThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r2, =CLoop ;
	mov r3, pc ;
	add r3, r3, #5 ;
	mov lr, r3 ;
	bx r2 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(mov r3, lr ; push {r3-r5}), _(pop {r3-r5} ; mov lr, r3))

TEST_ARM(testCLoopArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r2, =CLoop ;
	mov lr, pc ;
	bx r2 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5, lr}), _(pop {r4, r5, lr}))

TEST_VARIANT(testCLoop, testCLoopArm, testCLoopThumb)
