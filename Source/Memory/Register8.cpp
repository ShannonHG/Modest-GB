#include "Memory/Register8.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	void Register8::Write(uint8_t data)
	{
		this->data = data;
	}

	void Register8::ChangeBit(uint8_t bitIndex, bool enable)
	{
		Arithmetic::ChangeBit(data, bitIndex, enable);
	}

	uint8_t Register8::Read(uint8_t bitIndex) const
	{
		return Arithmetic::GetBit(data, bitIndex);
	}

	uint8_t Register8::Read(uint8_t start, uint8_t end) const
	{
		return Arithmetic::GetBits(data, start, end);
	}

	uint8_t Register8::Read() const
	{
		return data;
	}

	void Register8::Fill(bool value)
	{
		data = value * 255;
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