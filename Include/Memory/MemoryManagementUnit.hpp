#pragma once
#include <cstdint>

namespace SHG
{
	class MemoryManagementUnit
	{
	public: 
		virtual uint8_t GetByte(uint16_t address) = 0;
		virtual void SetByte(uint16_t address, uint8_t value) = 0;
		virtual bool IsAddressAvailable(uint16_t address) = 0;
	};
}