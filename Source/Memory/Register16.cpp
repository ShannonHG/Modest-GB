#include "Memory/Register8.hpp"
#include "Memory/Register16.hpp"

namespace SHG
{
	uint8_t Register16::GetHighByte()
	{
		return highRegister.GetData();
	}

	uint8_t Register16::GetLowByte()
	{
		return lowRegister.GetData();
	}

	void Register16::SetData(uint16_t data)
	{
		uint8_t highByte = data >> 8;
		uint8_t lowByte = data & 0x00FF;

		highRegister.SetData(highByte);
		lowRegister.SetData(lowByte);
	}

	uint16_t Register16::GetData()
	{
		return (highRegister.GetData() << 8) | lowRegister.GetData();
	}

	void Register16::SetHighByte(uint8_t data)
	{
		highRegister.SetData(data);
	}

	void Register16::SetLowByte(uint8_t data)
	{
		lowRegister.SetData(data);
	}

	void Register16::SetBit(uint8_t bitNumber, bool enabled)
	{
		if (bitNumber < 8) lowRegister.SetBit(bitNumber, enabled);
		else highRegister.SetBit(bitNumber - 8, enabled);
	}

	uint8_t Register16::GetBit(uint8_t bitNumber)
	{
		return bitNumber < 8 ? lowRegister.GetBit(bitNumber) : highRegister.GetBit(bitNumber);
	}

	void Register16::Increment()
	{
		SetData(GetData() + 1);
	}

	void Register16::Decrement()
	{
		SetData(GetData() - 1);
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