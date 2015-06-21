#include "macros.s"

TEST_ALL(testLdrh, _(ldrh r2, [sp]), ,)
TEST_ALL(testStrh, _(strh r3, [sp]), _(ldrh r3, [sp]),)
TEST_ARM(testLdmia1, _(ldmia sp, {r2}), ,)
TEST_ARM(testLdmia2, _(ldmia sp, {r2, r3}), ,)
TEST_ARM(testLdmia6, _(ldmia sp, {r2, r3}), _(push {r4-r7}), _(pop {r4-r7}))
TEST_ARM(testStmia1, _(stmia sp, {r2}), _(ldmia sp, {r2}),)
TEST_ARM(testStmia2, _(stmia sp, {r2, r3}), _(ldmia sp, {r2, r3}),)
TEST_ARM(testStmia6, _(stmia sp, {r2-r7}), _(push {r4-r7} ; ldmia sp, {r2-r7}), _(pop {r4-r7}))
