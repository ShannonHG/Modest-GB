#include "Memory/HuC1.hpp"

namespace SHG
{
	std::string HuC1::GetName()
	{
		return "HuC1";
	}

	bool HuC1::TryGetByte(uint16_t address, uint8_t& outValue)
	{
		return 0;
	}

	bool HuC1::TrySetByte(uint16_t address, uint8_t byte)
	{
		return false;
	}
}