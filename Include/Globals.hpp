#pragma once
#include <cstdint>
#include <cmath>
#include <string>

namespace SHG
{
	extern const uint16_t KiB;
	extern const uint32_t MiB;
	extern const uint64_t GiB;

	std::string ConvertToHexString(uint8_t byte, int width);
}
