#include "Memory/Register8.hpp"
#include "Memory/Register16.hpp"

namespace ModestGB
{
	Register16::Register16()
	{
		lowRegister = Register8();
		highRegister = Register8();
	}

	uint8_t Register16::GetHighByte() const
	{
		return highRegister.Read();
	}

	uint8_t Register16::GetLowByte() const
	{
		return lowRegister.Read();
	}

	void Register16::Write(uint16_t data)
	{
		uint8_t highByte = data >> 8;
		uint8_t lowByte = data & 0x00FF;

		highRegister.Write(highByte);
		lowRegister.Write(lowByte);
	}

	uint16_t Register16::Read() const
	{
		return (highRegister.Read() << 8) | lowRegister.Read();
	}

	void Register16::SetHighByte(uint8_t data)
	{
		highRegister.Write(data);
	}

	void Register16::SetLowByte(uint8_t data)
	{
		lowRegister.Write(data);
	}

	void Register16::SetBit(uint8_t bitNumber, bool enabled)
	{
		if (bitNumber < 8)
			lowRegister.ChangeBit(bitNumber, enabled);
		else
			highRegister.ChangeBit(bitNumber - 8, enabled);
	}

	uint8_t Register16::Read(uint8_t bitNumber) const
	{
		return bitNumber < 8 ? lowRegister.Read(bitNumber) : highRegister.Read(bitNumber);
	}

	void Register16::Increment()
	{
		Write(Read() + 1);
	}

	void Register16::Decrement()
	{
		Write(Read() - 1);
	}

	void Register16::Increase(uint16_t amount)
	{
		Write(Read() + amount);
	}

	void Register16::Fill(bool value)
	{
		lowRegister.Fill(value);
		highRegister.Fill(value);
	}

	void Register16::Decrease(uint16_t amount)
	{
		Write(Read() - amount);
	}

	Register8& Register16::GetHighRegister()
	{
		return highRegister;
	}

	Register8& Register16::GetLowRegister()
	{
		return lowRegister;
	}
}