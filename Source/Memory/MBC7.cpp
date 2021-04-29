#include "Memory/MBC7.hpp"

namespace SHG
{
	std::string MBC7::GetName()
	{
		return "MBC7";
	}

	bool MBC7::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MBC7::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}