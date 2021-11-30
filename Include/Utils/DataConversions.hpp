#pragma once
#include <cstdint>
#include <string>

namespace SHG
{
	std::string ConvertToHexString(uint32_t data, uint8_t width);
	std::string GetHexString8(uint8_t data);
	std::string GetHexString16(uint16_t data);
	std::string GetHexString32(uint32_t data);
}
