#include "Memory/Register8.hpp"
#include "Memory/Register16.hpp"

namespace SHG
{
	Register16::Register16() : Memory(2)
	{
		lowRegister = Register8();
		highRegister = Register8();
	}

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
		if (bitNumber < 8) lowRegister.ChangeBit(bitNumber, enabled);
		else highRegister.ChangeBit(bitNumber - 8, enabled);
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

	void Register16::Increase(uint16_t amount)
	{
		SetData(GetData() + amount);
	}

	void Register16::Decrease(uint16_t amount)
	{
		SetData(GetData() - amount);
	}

	Register8& Register16::GetHighRegister()
	{
		return highRegister;
	}

	Register8& Register16::GetLowRegister()
	{
		return lowRegister;
	}

	uint8_t Register16::Read(uint16_t address)
	{
		if (IsAddressAvailable(address))
			return GetData();

		return 0;
	}
	
	void Register16::Write(uint16_t address, uint8_t value)
	{
		if (IsAddressAvailable(address))
			SetData(value);
	}

	bool Register16::IsAddressAvailable(uint16_t address)
	{
		return address < 2;
	}

	void Register16::Reset()
	{
		lowRegister.Reset();
		highRegister.Reset();
	}
}