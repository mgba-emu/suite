#pragma once

#include <gba_types.h>

#define TEST_ARM 1
#define TEST_THUMB 2
#define VIEW_SIZE 16

#define REG_WAITCNT (*(vu16*) 0x4000204)

#define GRID_STRIDE 32
extern char textGrid[GRID_STRIDE * 32];

void updateTextGrid(void);
__attribute__((format(printf, 1, 2))) int savprintf(const char* fmt, ...);
__attribute__((format(printf, 1, 2))) int debugprintf(const char* fmt, ...);
