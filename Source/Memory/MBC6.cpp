#include "Memory/MBC6.hpp"

namespace SHG
{
	std::string MBC6::GetName()
	{
		return "MBC6";
	}

	bool MBC6::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MBC6::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}