#include "MBC3.hpp"

namespace SHG
{
	std::string MBC3::GetName()
	{
		return "MBC3";
	}

	bool MBC3::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return false;
	}

	bool MBC3::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}