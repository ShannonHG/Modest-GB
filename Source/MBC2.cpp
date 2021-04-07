#include "MBC2.hpp"

namespace SHG
{
	std::string MBC2::GetName()
	{
		return "MBC2";
	}

	bool MBC2::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MBC2::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}