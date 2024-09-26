#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_sio.h>
#include <gba_systemcalls.h>
#include <gba_video.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <mgba.h>

#include "common.h"

#include "bios-math.h"
#include "carry.h"
#include "dma.h"
#include "font.h"
#include "io-read.h"
#include "memory.h"
#include "misc-edge.h"
#include "multiply-long.h"
#include "shifter.h"
#include "sio-read.h"
#include "sio-timing.h"
#include "suite.h"
#include "timing.h"
#include "timer-irq.h"
#include "timers.h"
#include "video.h"

u16* textBase = (u16*) VRAM;
char textGrid[32 * 32];
bool isMgba = false;

const char savetype[] = "SRAM_V123";

EWRAM_DATA const int _anchor = 0xABAD1DEA; // There seems to be a bug in the ld script that this fixes
IWRAM_DATA struct ActiveInfo activeTestInfo = { {'I', 'n', 'f', 'o'}, -1, -1, -1 };

void updateTextGrid(void) {
	int i;
	for (i = 0; i < 20 * 32; ++i) {
		u16 text = textBase[i] & ~0x3FF;
		if (textGrid[i]) {
			text |= textGrid[i] - ' ';
		}
		textBase[i] = text;
	}
}

const struct TestSuite* const suites[] = {
	&memoryTestSuite,
	&ioReadTestSuite,
	&timingTestSuite,
	&timersTestSuite,
	&timerIRQTestSuite,
	&shifterTestSuite,
	&carryTestSuite,
	&multiplyLongTestSuite,
	&biosMathTestSuite,
	&dmaTestSuite,
	&sioReadTestSuite,
	&sioTimingTestSuite,
	&miscEdgeTestSuite,
	&videoTestSuite
};

const size_t nSuites = sizeof(suites) / sizeof(*suites);

static void runSuite(const struct TestSuite* activeSuite) {
	const char* testNameBuffer[160];
	bool results[160];
	int testIndex = 0;
	int viewIndex = 0;
	strcpy(&textGrid[GRID_STRIDE], activeSuite->name);
	strcpy(&textGrid[GRID_STRIDE * 4 + 11], "Testing...");
	updateTextGrid();
	if (activeSuite->run) {
		debugprintf("BEGIN: %s", activeSuite->name);
		activeSuite->run();
		debugprintf("END: %i/%i", *activeSuite->passes, *activeSuite->totalResults);
	}
	while (1) {
		memset(&textGrid[GRID_STRIDE], 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDownRepeat();

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
		if (keys & KEY_LEFT) {
			if (testIndex != viewIndex) {
				testIndex = viewIndex;
			} else {
				testIndex -= VIEW_SIZE;
			}
			if (testIndex < 0) {
				testIndex = activeSuite->nTests - 1;
			}
		}
		if (keys & KEY_RIGHT) {
			if (testIndex == activeSuite->nTests - 1) {
				testIndex = 0;
			} else if (testIndex + VIEW_SIZE >= activeSuite->nTests) {
				testIndex = activeSuite->nTests - 1;
			} else if (viewIndex + VIEW_SIZE - 1 == testIndex) {
				testIndex += VIEW_SIZE;
			} else {
				testIndex = viewIndex + VIEW_SIZE - 1;
			}
		}
		if (testIndex < viewIndex) {
			viewIndex = testIndex;
		} else if (testIndex >= viewIndex + VIEW_SIZE) {
			viewIndex = testIndex - VIEW_SIZE + 1;
		}
		strcpy(&textGrid[GRID_STRIDE], activeSuite->name);
		if (*activeSuite->totalResults) {
			sprintf(TEXT_LOC(1, 21), "%4u/%-4u", *activeSuite->passes, *activeSuite->totalResults);
		}
		size_t i;
		activeSuite->list(testNameBuffer, results, sizeof(testNameBuffer) / sizeof(*testNameBuffer), viewIndex);
		for (i = 0; i < activeSuite->nTests && i < VIEW_SIZE; ++i) {
			if (results[i]) {
				markLinePass(i + 3);
			} else {
				markLineFail(i + 3);
			}
			snprintf(TEXT_LOC(i + 3, 0), 31, "%c%s", (i + viewIndex == testIndex) ? '>' : ' ', testNameBuffer[i]);
		}

		updateTextGrid();
		VBlankIntrWait();
	}
}

void nocash_printf(const char* str) {
	static const char* ID = (const char*) 0x4FFFA00;
	static volatile const char** OUT = (volatile const char**) 0x4FFFA10;
	static volatile char* OUTC = (volatile char*) 0x4FFFA1C;
	if (!strncmp(ID, "no$gba ", 7)) {
		*OUT = str;
		*OUTC = '\n';
	}
}

__attribute__((format(printf, 1, 2)))
void savprintf(const char* fmt, ...) {
	static u32 location = 0;
	char tmp[128];

	va_list args;
	va_start(args, fmt);
	int s = vsnprintf(tmp, sizeof(tmp), fmt, args);
	va_end(args);

	mgba_printf(MGBA_LOG_INFO, "%s", tmp);
	nocash_printf(tmp);

	vs8* sbase = (vs8*) SRAM + location;
	size_t i;
	for (i = 0; i < s; ++i) {
		if (location + i >= 0x8000) {
			return;
		}
		sbase[i] = tmp[i];
	}
	sbase[s] = '\n';
	++s;
	sbase[s] = '\0';
	location += s;
	return;
}

__attribute__((format(printf, 1, 2)))
int debugprintf(const char* fmt, ...) {
	if (!isMgba) {
		return 0;
	}

	char tmp[128];

	va_list args;
	va_start(args, fmt);
	int s = vsnprintf(tmp, sizeof(tmp), fmt, args);
	va_end(args);

	mgba_printf(MGBA_LOG_DEBUG, "%s", tmp);
	return s;
}

int main(void) {
	irqInit();

	BG_PALETTE[0] = 0x7FFF;
	BG_PALETTE[16] = 0x7FFF;
	BG_PALETTE[17] = 0x1F;
	DMA3COPY(fontTiles, TILE_BASE_ADR(1), DMA16 | DMA_IMMEDIATE | (fontTilesLen >> 1));
	REG_BG1CNT = CHAR_BASE(1) | SCREEN_BASE(0);
	REG_BG1VOFS = -4;
	memset(textGrid, 0, sizeof(textGrid));
	strcpy(&textGrid[2], "Game Boy Advance Test Suite");
	updateTextGrid();
	REG_DISPCNT = MODE_0 | BG1_ON;
	REG_RCNT = 0;

	setRepeat(20, 6);

	irqEnable(IRQ_VBLANK);

	bzero((u8*) SRAM, 0x10000);
	isMgba = mgba_open();
	savprintf("Game Boy Advance Test Suite\n===");

	int suiteIndex = 0;
	int viewIndex = 0;
	while (1) {
		memset(TEXT_LOC(1, 0), 0, sizeof(textGrid) - GRID_STRIDE);
		scanKeys();
		u16 keys = keysDownRepeat();

		if (keys & KEY_A) {
			activeTestInfo.suiteId = suiteIndex;
			runSuite(suites[suiteIndex]);
			activeTestInfo.suiteId = -1;
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
		clearAll();
		strcpy(TEXT_LOC(1, 0), "Select suite");
		size_t i;
		for (i = 0; i < nSuites && i < VIEW_SIZE; ++i) {
			snprintf(TEXT_LOC(i + 3, 0), 31, "%c%s", (i + viewIndex == suiteIndex) ? '>' : ' ', suites[i]->name);
		}

		updateTextGrid();
		VBlankIntrWait();
	}

	return 0;
}
