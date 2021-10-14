#include <sstream>
#include "Common/DataConversions.hpp"
#include <iomanip>

namespace SHG
{
	std::string ConvertToHexString(uint16_t data, int width)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(width) << std::hex << (int)data;

		std::string result;
		for (auto c : stream.str()) result.push_back(std::toupper(c));

		return "0x" + result;
	}

	std::string GetHexString8(uint8_t data)
	{
		return ConvertToHexString(data, 2);
	}

	std::string GetHexString16(uint8_t data)
	{
		return ConvertToHexString(data, 4);
	}
}