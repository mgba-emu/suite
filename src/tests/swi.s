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

TEST_THUMB(testDiv2Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	ldr r1, =0x12345678 ;
	swi $6 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testDiv2Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	ldr r1, =0x12345678 ;
	swi $0x60000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testDiv2, testDiv2Arm, testDiv2Thumb)

TEST_THUMB(testAtanThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	swi $9 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testAtanArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	swi $0x90000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testAtan, testAtanArm, testAtanThumb)

TEST_THUMB(testSqrtThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	swi $8 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testSqrtArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	swi $0x80000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testSqrt, testSqrtArm, testSqrtThumb)

TEST_THUMB(testSqrt2Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	swi $8 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testSqrt2Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0xFF ;
	swi $0x80000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testSqrt2, testSqrt2Arm, testSqrt2Thumb)

TEST_THUMB(testSqrt3Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r0, =0x12345678 ;
	swi $8 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testSqrt3Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	ldr r0, =0x12345678 ;
	swi $0x80000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testSqrt3, testSqrt3Arm, testSqrt3Thumb)

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

TEST_THUMB(testBiosChecksumThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	swi $0xD ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testBiosChecksumArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	swi $0xD0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testBiosChecksum, testBiosChecksumArm, testBiosChecksumThumb)

TEST_THUMB(testBgAffineSetThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x0 ;
	swi $0xE ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testBgAffineSetArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x0 ;
	swi $0xE0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testBgAffineSet, testBgAffineSetArm, testBgAffineSetThumb)

TEST_THUMB(testBgAffineSet2Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x1 ;
	swi $0xE ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testBgAffineSet2Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x1 ;
	swi $0xE0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testBgAffineSet2, testBgAffineSet2Arm, testBgAffineSet2Thumb)

TEST_THUMB(testBgAffineSet3Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x2 ;
	swi $0xE ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testBgAffineSet3Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x2 ;
	swi $0xE0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testBgAffineSet3, testBgAffineSet3Arm, testBgAffineSet3Thumb)

TEST_THUMB(testObjAffineSetThumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x0 ;
	mov r3, #0x0 ;
	swi $0xF ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testObjAffineSetArm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x0 ;
	mov r3, #0x0 ;
	swi $0xF0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testObjAffineSet, testObjAffineSetArm, testObjAffineSetThumb)

TEST_THUMB(testObjAffineSet2Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x1 ;
	mov r3, #0x0 ;
	swi $0xF ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testObjAffineSet2Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x1 ;
	mov r3, #0x0 ;
	swi $0xF0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testObjAffineSet2, testObjAffineSet2Arm, testObjAffineSet2Thumb)

TEST_THUMB(testObjAffineSet3Thumb, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x2 ;
	mov r3, #0x0 ;
	swi $0xF ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_ARM(testObjAffineSet3Arm, _(
	mov r4, r0 ;
	mov r5, r1 ;
	mov r0, #0x0 ;
	mov r1, #0x0 ;
	mov r2, #0x2 ;
	mov r3, #0x0 ;
	swi $0xF0000 ;
	mov r0, r4 ;
	mov r1, r5 ;
), _(push {r4, r5}), _(pop {r4, r5}))

TEST_VARIANT(testObjAffineSet3, testObjAffineSet3Arm, testObjAffineSet3Thumb)