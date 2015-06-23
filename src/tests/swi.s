#include "macros.s"

TEST_THUMB(testDivThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r0, =0x12345678 ;
	mov r1, #0xFF ;
	swi $6 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testDivArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r0, =0x12345678 ;
	mov r1, #0xFF ;
	swi $0x60000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testDiv, testDivArm, testDivThumb)

TEST_THUMB(testCpuSetThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r0, =0x02000000 ;
	ldr r1, =0x02000000 ;
	ldr r2, =0x100 ;
	swi $0xC ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4-r5}), _(pop {r4-r5}))

TEST_ARM(testCpuSetArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x02000000 ;
	mov r1, #0x02000000 ;
	mov r2, #0x100 ;
	swi $0xC0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testCpuSet, testCpuSetArm, testCpuSetThumb)
