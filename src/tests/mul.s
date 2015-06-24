#include "macros.s"

TEST_ALL(testMul0, _(mul r3, r2), _(ldr r3, =0x00000000 ; mov r2, #0xFF),)
TEST_ALL(testMul1, _(mul r3, r2), _(ldr r3, =0x00000078 ; mov r2, #0xFF),)
TEST_ALL(testMul2, _(mul r3, r2), _(ldr r3, =0x00005678 ; mov r2, #0xFF),)
TEST_ALL(testMul3, _(mul r3, r2), _(ldr r3, =0x00345678 ; mov r2, #0xFF),)
TEST_ALL(testMul4, _(mul r3, r2), _(ldr r3, =0x12345678 ; mov r2, #0xFF),)
TEST_ALL(testMul5, _(mul r3, r2), _(ldr r3, =0xFF000000 ; mov r2, #0xFF),)
TEST_ALL(testMul6, _(mul r3, r2), _(ldr r3, =0xFFFF0000 ; mov r2, #0xFF),)
TEST_ALL(testMul7, _(mul r3, r2), _(ldr r3, =0xFFFFFF00 ; mov r2, #0xFF),)
TEST_ALL(testMul8, _(mul r3, r2), _(ldr r3, =0xFFFFFFFF ; mov r2, #0xFF),)
TEST_ALL(testMul9, _(mul r3, r2), _(ldr r3, =0xFFFFFFFF ; mov r2, #0x00),)
