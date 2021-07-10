#include <limits>
#include <cstdint>
#include "Common/Arithmetic.hpp"

namespace SHG
{
	bool SHG::IsUnsigned8Bit(int num)
	{
		return num >= std::numeric_limits<uint8_t>::min() && num <= std::numeric_limits<uint8_t>::max();
	}

	bool SHG::IsUnsigned16Bit(int num)
	{
		return num >= std::numeric_limits<uint16_t>::min() && num <= std::numeric_limits<uint16_t>::max();
	}

	bool SHG::IsHalfCarryRequired(int operand1, int operand2)
	{
		// Isolate and add low nibbles to determine if a carry occurs.
		return (((operand1 & 0x0F) + (operand2 & 0x0F)) & 0x10) == 0x10;
	}

	void ChangeBit(uint8_t& data, uint8_t bitIndex, bool enable)
	{
		if (enable) SetBit(data, bitIndex);
		else ResetBit(data, bitIndex);
	}

	void SHG::SetBit(uint8_t& data, uint8_t bitIndex)
	{
		data |= (1 << bitIndex);
	}

	void SHG::ResetBit(uint8_t& data, uint8_t bitIndex)
	{
		data &= ~(1 << bitIndex);
	}
}