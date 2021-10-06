#include "Memory/Register8.hpp"
#include "Common/Arithmetic.hpp"

namespace SHG
{
	Register8::Register8() : Memory(1)
	{ 
	
	}

	void Register8::SetData(uint8_t data)
	{
		Memory::SetByte(0, data);
	}

	void Register8::ChangeBit(uint8_t bitIndex, bool enable)
	{
		Memory::ChangeBit(0, bitIndex, enable);
	}

	uint8_t Register8::GetBit(uint8_t bitNumber)
	{
		return Memory::GetBit(0, bitNumber);
	}

	uint8_t Register8::GetData()
	{
		return GetByte(0);
	}

	void Register8::Increment()
	{
		SetData(GetData() + 1);
	}

	void Register8::Decrement()
	{
		SetByte(0, GetData() - 1);
	}

	void Register8::Increase(uint8_t amount)
	{
		SetByte(0, GetData() + amount);
	}

	void Register8::Decrease(uint8_t amount)
	{
		SetByte(0, GetData() - amount);
	}
}