#include "Memory/HuC3.hpp"

namespace SHG
{
	std::string HuC3::GetName()
	{
		return "HuC3";
	}

	bool HuC3::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return 0;
	}

	bool HuC3::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}