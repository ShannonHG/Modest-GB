#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include "Utils/DataConversions.hpp"

namespace ModestGB::Convert
{
	std::string ConvertToHexString(uint32_t data, uint8_t width)
	{
		// Example format string that allows 2 hexadecimal digits: %02X
		std::string format = "%0";
		format += std::to_string(width) + "X";

		auto buffer = std::vector<char>(width + 1);
		std::snprintf(buffer.data(), sizeof(char) * buffer.size(), format.c_str(), data);
		return "$" + std::string(buffer.data());
	}

	std::string GetHexString8(uint8_t data)
	{
		return ConvertToHexString(data, 2);
	}

	std::string GetHexString16(uint16_t data)
	{
		return ConvertToHexString(data, 4);
	}

	std::string GetHexString32(uint32_t data)
	{
		return ConvertToHexString(data, 8);
	}
}