#include "Memory/Register8.hpp"
#include "Common/Arithmetic.hpp"

namespace SHG
{
	void Register8::SetData(uint8_t data)
	{
		this->data = data;
	}

	void Register8::ChangeBit(uint8_t bitIndex, bool enable)
	{
		Arithmetic::ChangeBit(data, bitIndex, enable);
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

	void Register8::Increase(uint8_t amount)
	{
		data += amount;
	}

	void Register8::Decrease(uint8_t amount)
	{
		data -= amount;
	}
}