#include <sstream>
#include "Globals.hpp"
#include <iomanip>

namespace SHG
{
	extern const uint16_t KiB = 1024;
	extern const uint32_t MiB = pow(1024, 2);
	extern const uint64_t GiB = pow(1024, 3);

	std::string ConvertToHexString(uint8_t byte, int width)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(width) << std::hex << (int)byte;

		std::string result;
		for (auto c : stream.str()) result.push_back(std::toupper(c));

		return "0x" + result;
	}
}