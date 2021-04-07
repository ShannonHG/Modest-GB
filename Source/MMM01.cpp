#include "MMM01.hpp"

namespace SHG
{
	std::string MMM01::GetName()
	{
		return "MMM01";
	}

	bool MMM01::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MMM01::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}