#include "CPU/Interrupts.hpp"
#include "Common/Arithmetic.hpp"

namespace SHG
{
	const uint16_t INTERRUPT_FLAG_ADDRESS = 0xFF0F;
	const uint16_t INTERRUPT_ENABLE_ADDRESS = 0xFFFF;

	const std::map<InterruptType, uint8_t> SHG::INTERRUPT_OPERATIONS =
	{
		{ InterruptType::VBlank, 0x40 },
		{ InterruptType::LCDStat, 0x48 },
		{ InterruptType::Timer, 0x50 },
		{ InterruptType::Serial, 0x58 },
		{ InterruptType::Joypad, 0x60 }
	};

	void SHG::RequestInterrupt(MemoryMap& memoryMap, InterruptType interruptType)
	{
		uint8_t interruptEnable = memoryMap.GetByte(INTERRUPT_ENABLE_ADDRESS);
		Arithmetic::SetBit(interruptEnable, static_cast<uint8_t>(interruptType));
		memoryMap.SetByte(INTERRUPT_ENABLE_ADDRESS, interruptEnable);

		uint8_t interruptFlag = memoryMap.GetByte(INTERRUPT_FLAG_ADDRESS);
		Arithmetic::SetBit(interruptFlag, static_cast<uint8_t>(interruptType));
		memoryMap.SetByte(INTERRUPT_FLAG_ADDRESS, interruptFlag);
	}
}