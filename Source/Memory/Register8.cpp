#include "Memory/Register8.hpp"
#include "Globals.hpp"

namespace SHG
{
	void Register8::SetData(uint8_t data)
	{
		this->data = data;
	}

	void Register8::SetBit(uint8_t bitNumber, bool enabled)
	{
		SetBitValue(data, bitNumber, enabled);
	}

	uint8_t Register8::GetBit(uint8_t bitNumber)
	{
		return (data >> bitNumber) & 1;
	}

	uint8_t Register8::GetData()
	{
		return data;
	}

	void Register8::Increment()
	{
		data++;
	}

	void Register8::Decrement()
	{
		data--;
	}
}