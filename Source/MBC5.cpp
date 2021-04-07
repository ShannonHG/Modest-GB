#include "MBC5.hpp"

namespace SHG
{
	std::string MBC5::GetName()
	{
		return "MBC5";
	}

	bool MBC5::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MBC5::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}