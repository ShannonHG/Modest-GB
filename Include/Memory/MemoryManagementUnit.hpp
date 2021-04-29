#pragma once
#include <cstdint>

namespace SHG
{
	class MemoryManagementUnit
	{
	public: 
		bool TryGetByte(uint16_t address, uint8_t& outValue);
		bool TrySetByte(uint16_t address, uint8_t value);
	};
}