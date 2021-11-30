#pragma once
#include <map>
#include "Memory/MemoryMap.hpp"

namespace SHG
{
	enum class InterruptType
	{
		VBlank = 0,
		LCDStat = 1,
		Timer = 2,
		Serial = 3,
		Joypad = 4
	};

	extern const uint16_t INTERRUPT_FLAG_ADDRESS;
	extern const uint16_t INTERRUPT_ENABLE_ADDRESS;
	extern const std::map<InterruptType, uint8_t> INTERRUPT_OPERATIONS;

	void RequestInterrupt(MemoryMap& memoryMap, InterruptType interruptType);
}