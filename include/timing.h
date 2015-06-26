#pragma once

#include "common.h"

struct TestTimings {
	s32 arm_text_0000;
	s32 arm_text_4000;
	s32 arm_text_0004;
	s32 arm_text_4004;
	s32 arm_text_0010;
	s32 arm_text_4010;
	s32 arm_text_0014;
	s32 arm_text_4014;
	s32 arm_ewram;
	s32 arm_iwram;
	s32 thumb_text_0000;
	s32 thumb_text_4000;
	s32 thumb_text_0004;
	s32 thumb_text_4004;
	s32 thumb_text_0010;
	s32 thumb_text_4010;
	s32 thumb_text_0014;
	s32 thumb_text_4014;
	s32 thumb_ewram;
	s32 thumb_iwram;
};

struct TimingTest {
	const char* testName;
	void (*test)(struct TestTimings*);
	int modes;
	struct TestTimings expected;
};

extern const struct TimingTest timingTests[];
extern const u32 nTimingTests;

void calibrate(struct TestTimings*);
