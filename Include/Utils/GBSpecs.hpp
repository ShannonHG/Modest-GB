#pragma once
#include <cstdint>

namespace ModestGB
{
	const uint32_t GB_CLOCK_SPEED = 4194304;
	const double GB_FRAMES_PER_SECOND = 59.7;
	const double GB_SECONDS_PER_FRAME = 1 / GB_FRAMES_PER_SECOND;
	const double GB_CYCLES_PER_FRAME = GB_CLOCK_SPEED / GB_FRAMES_PER_SECOND;
	const uint8_t GB_SCREEN_WIDTH = 160;
	const uint8_t GB_SCREEN_HEIGHT = 144;
	const uint16_t GB_HSYNC_TIME = 9198;
	const uint16_t GB_VSYNC_TIME = 59730;
}