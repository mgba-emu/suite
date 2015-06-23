#include "macros.s"

TEST_ARM(testLdmia1, _(ldmia sp, {r2}), ,)
TEST_ARM(testLdmia2, _(ldmia sp, {r2, r3}), ,)
TEST_ARM(testLdmia6, _(ldmia sp, {r2-r7}), _(push {r4-r7}), _(pop {r4-r7}))
