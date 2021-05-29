#include "CPU/CPURegister.hpp"

namespace SHG
{
	void CPURegister::SetUpperByte(uint8_t value)
	{
		data |= value << 8;
	}

	void CPURegister::SetLowerByte(uint8_t value)
	{
		data |= 0x00FF & value;
	}

	uint8_t CPURegister::GetUpperByte()
	{
		return data >> 8;
	}

	uint8_t CPURegister::GetLowerByte()
	{
		return data & 0x00FF;
	}

	uint16_t CPURegister::GetData()
	{
		return data;
	}

	void CPURegister::SetData(uint16_t value)
	{
		data = value;
	}

	void CPURegister::Increment()
	{
		data++;
	}

	void CPURegister::Decrement()
	{
		data--;
	}
}