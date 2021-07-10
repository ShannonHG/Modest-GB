#pragma once
#include <cstdint>
#include <cmath>
#include <string>

namespace SHG
{
	const uint16_t KiB = 1024;
	const uint32_t MiB = pow(1024, 2);
	const uint64_t GiB = pow(1024, 3);
	const uint8_t GB_SCREEN_WIDTH = 160;
	const uint8_t GB_SCREEN_HEIGHT = 144;
	const uint32_t GB_CLOCK_SPEED = 4194304;
	const uint16_t GB_HSYNC_TIME = 9198;
	const uint16_t GB_VSYNC_TIME = 59730;

	std::string ConvertToHexString(uint16_t data, int width);
	void SetBitValue(uint8_t& data, uint8_t bitIndex, bool bitValue);
}
