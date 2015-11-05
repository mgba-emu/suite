#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "font.h"
#include "timing.h"
#include "memory.h"
#include "suite.h"

u16* textBase = (u16*) VRAM;
char textGrid[32 * 32];

#define REG_WAITCNT (*(vu16*) 0x4000204)

EWRAM_DATA const int _anchor = 0xABAD1DEA; // There seems to be a bug in the ld script that this fixes

void updateTextGrid(void) {
	int i;
	for (i = 0; i < 20 * 32; ++i) {
		textBase[i] = textGrid[i] ? textGrid[i] - ' ' : 0;
	}
}

const struct TestSuite* const suites[] = {
	&memoryTestSuite,
	&timingTestSuite
};

const size_t nSuites = sizeof(suites) / sizeof(*suites);

static void runSuite(const struct TestSuite* activeSuite) {
	const char* testNameBuffer[128];
	int testIndex = 0;
	int viewIndex = 0;
	strcpy(&textGrid[GRID_STRIDE], activeSuite->name);
	strcpy(&textGrid[GRID_STRIDE * 4 + 11], "Testing...");
	updateTextGrid();
	activeSuite->list(testNameBuffer, sizeof(testNameBuffer) / sizeof(*testNameBuffer), 0);
	activeSuite->run();
	while (1) {
		memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDown();

		if (keys & KEY_A) {
			activeSuite->show(testIndex);
			continue;
		}

		if (keys & KEY_B) {
			testIndex = 0;
			viewIndex = 0;
			break;
		}

		if (keys & KEY_UP) {
			--testIndex;
			if (testIndex < 0) {
				testIndex = activeSuite->nTests - 1;
			}
		}
		if (keys & KEY_DOWN) {
			++testIndex;
			testIndex %= activeSuite->nTests;
		}
		if (testIndex < viewIndex) {
			viewIndex = testIndex;
		} else if (testIndex >= viewIndex + VIEW_SIZE) {
			viewIndex = testIndex - VIEW_SIZE + 1;
		}
		strcpy(&textGrid[GRID_STRIDE], activeSuite->name);
		sprintf(&textGrid[GRID_STRIDE + 21], "%4u/%-4u", *activeSuite->passes, *activeSuite->totalResults);
		size_t i;
		for (i = 0; i < activeSuite->nTests && i < VIEW_SIZE; ++i) {
			snprintf(&textGrid[(3 + i) * GRID_STRIDE], 31, "%c%s", (i + viewIndex == testIndex) ? '>' : ' ', testNameBuffer[i + viewIndex]);
		}

		updateTextGrid();
		VBlankIntrWait();
	}
}

int main(void) {
	irqInit();

	BG_PALETTE[0] = 0x7FFF;
	DMA3COPY(fontTiles, TILE_BASE_ADR(1), DMA16 | DMA_IMMEDIATE | (fontTilesLen >> 1));
	REG_BG1CNT = CHAR_BASE(1) | SCREEN_BASE(0);
	REG_BG1VOFS = -4;
	memset(textGrid, 0, sizeof(textGrid));
	strcpy(&textGrid[2], "Game Boy Advance Test Suite");
	updateTextGrid();
	REG_DISPCNT = MODE_0 | BG1_ON;

	irqEnable(IRQ_VBLANK);

	int suiteIndex = 0;
	int viewIndex = 0;
	while (1) {
		memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDown();

		if (keys & KEY_A) {
			runSuite(suites[suiteIndex]);
			continue;
		}

		if (keys & KEY_UP) {
			--suiteIndex;
			if (suiteIndex < 0) {
				suiteIndex = nSuites - 1;
			}
		}
		if (keys & KEY_DOWN) {
			++suiteIndex;
			suiteIndex %= nSuites;
		}
		if (suiteIndex < viewIndex) {
			viewIndex = suiteIndex;
		} else if (suiteIndex >= viewIndex + VIEW_SIZE) {
			viewIndex = suiteIndex - VIEW_SIZE + 1;
		}
		strcpy(&textGrid[GRID_STRIDE], "Select suite");
		size_t i;
		for (i = 0; i < nSuites && i < VIEW_SIZE; ++i) {
			snprintf(&textGrid[(3 + i) * GRID_STRIDE], 31, "%c%s", (i + viewIndex == suiteIndex) ? '>' : ' ', suites[i]->name);
		}

		updateTextGrid();
		VBlankIntrWait();
	}

	return 0;
}
