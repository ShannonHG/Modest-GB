#include <sstream>
#include "Globals.hpp"
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

	void SetBitValue(uint8_t& data, uint8_t bitNumber, bool bitValue)
	{
		if (bitValue) data |= ((uint8_t)bitValue) << bitNumber;
		else data &= ~(((uint8_t)!bitValue) << bitNumber);
	}
}