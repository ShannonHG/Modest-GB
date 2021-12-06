#include "Memory/Memory.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	bool Memory::GetBit(uint16_t address, uint8_t bitNum) const
	{
		uint8_t byte = Read(address);
		return Arithmetic::GetBit(byte, bitNum);
	}

	void Memory::ChangeBit(uint16_t address, uint8_t bitNum, bool bitValue)
	{
		uint8_t byte = Read(address);
		Arithmetic::ChangeBit(byte, bitNum, bitValue);
		Write(address, byte);
	}

	void Memory::SetBit(uint16_t address, uint8_t bitNum)
	{
		ChangeBit(address, bitNum, true);
	}

	void Memory::ClearBit(uint16_t address, uint8_t bitNum)
	{
		ChangeBit(address, bitNum, false);
	}
}