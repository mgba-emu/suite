#pragma once

#include <gba_types.h>

#define TEST_ARM 1
#define TEST_THUMB 2
#define VIEW_SIZE 16

#define GRID_STRIDE 32
extern char textGrid[GRID_STRIDE * 32];

void updateTextGrid(void);
