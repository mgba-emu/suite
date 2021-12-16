#include "macros.s"

TEST_ARM(testLdmia1, _(ldmia sp, {r2}), ,)
TEST_ARM(testLdmia2, _(ldmia sp, {r2, r3}), ,)
TEST_ARM(testLdmia6, _(ldmia sp, {r2-r7}), _(push {r4-r7}), _(pop {r4-r7}))

TEST_ARM(testLdmia1x2, _(ldmia sp, {r2} ; ldmia sp, {r2}), ,)
TEST_ARM(testLdmia2x2, _(ldmia sp, {r2, r3} ; ldmia sp, {r2, r3}), ,)
TEST_ARM(testLdmia6x2, _(ldmia sp, {r2-r7} ; ldmia sp, {r2-r7}), _(push {r4-r7}), _(pop {r4-r7}))

TEST_ALL(testLdmiaOverflow1OamToRom, _(ldmia r2!, {r3-r7}), _(push {r4-r7} ; ldr r2, =0x07FFFFFC), _(pop {r4-r7}))
TEST_ALL(testLdmiaOverflow2OamToRom, _(ldmia r2!, {r3-r7}), _(push {r4-r7} ; ldr r2, =0x07FFFFF8), _(pop {r4-r7}))
TEST_ALL(testLdmiaOverflow3OamToRom, _(ldmia r2!, {r3-r7}), _(push {r4-r7} ; ldr r2, =0x07FFFFF4), _(pop {r4-r7}))
TEST_ALL(testLdmiaOverflow4OamToRom, _(ldmia r2!, {r3-r7}), _(push {r4-r7} ; ldr r2, =0x07FFFFF0), _(pop {r4-r7}))
TEST_ALL(testLdmiaOverflow5OamToRom, _(ldmia r2!, {r3-r7}), _(push {r4-r7} ; ldr r2, =0x07FFFFEC), _(pop {r4-r7}))