#include "macros.s"

TEST_ALL(testLdrh, _(ldrh r2, [sp]), ,)
TEST_ALL(testStrh, _(strh r3, [sp]), _(ldrh r3, [sp]),)
