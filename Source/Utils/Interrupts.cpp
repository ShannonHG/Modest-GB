#include "Utils/Interrupts.hpp"
#include "Utils/Arithmetic.hpp"
#include "Logger.hpp"

namespace ModestGB::Interrupts
{
	const uint16_t INTERRUPT_FLAG_ADDRESS = 0xFF0F;
	const uint16_t INTERRUPT_ENABLE_ADDRESS = 0xFFFF;

	const std::map<InterruptType, uint8_t> INTERRUPT_OPERATIONS =
	{
		{ InterruptType::VBlank, 0x40 },
		{ InterruptType::LCDStat, 0x48 },
		{ InterruptType::Timer, 0x50 },
		{ InterruptType::Serial, 0x58 },
		{ InterruptType::Joypad, 0x60 }
	};

	void RequestInterrupt(Memory& memoryMap, InterruptType interruptType)
	{
		memoryMap.SetBit(INTERRUPT_FLAG_ADDRESS, static_cast<uint8_t>(interruptType));
	}
}