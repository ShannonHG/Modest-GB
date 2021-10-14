#pragma once
#include <cstdint>
#include <string>

namespace SHG
{
	std::string ConvertToHexString(uint16_t data, int width);
	std::string GetHexString8(uint8_t data);
	std::string GetHexString16(uint8_t data);
}
