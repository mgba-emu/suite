#include "macros.s"

TEST_ARM(testStmia1, _(stmia sp, {r2}), _(ldmia sp, {r2}),)
TEST_ARM(testStmia2, _(stmia sp, {r2, r3}), _(ldmia sp, {r2, r3}),)
TEST_ARM(testStmia6, _(stmia sp, {r2-r7}), _(push {r4-r7} ; ldmia sp, {r2-r7}), _(pop {r4-r7}))

TEST_ARM(testStmia1x2, _(stmia sp, {r2} ; stmia sp, {r2}), _(ldmia sp, {r2}),)
TEST_ARM(testStmia2x2, _(stmia sp, {r2, r3} ; stmia sp, {r2, r3}), _(ldmia sp, {r2, r3}),)
TEST_ARM(testStmia6x2, _(stmia sp, {r2-r7} ; stmia sp, {r2-r7}), _(push {r4-r7} ; ldmia sp, {r2-r7}), _(pop {r4-r7}))
