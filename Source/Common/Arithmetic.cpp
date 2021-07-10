#include <limits>
#include <cstdint>
#include "Common/Arithmetic.hpp"

namespace SHG::Arithmetic
{
	bool Arithmetic::IsUnsigned8Bit(int num)
	{
		return num >= std::numeric_limits<uint8_t>::min() && num <= std::numeric_limits<uint8_t>::max();
	}

	bool Arithmetic::IsUnsigned16Bit(int num)
	{
		return num >= std::numeric_limits<uint16_t>::min() && num <= std::numeric_limits<uint16_t>::max();
	}

	bool Arithmetic::IsHalfCarryRequired(int operand1, int operand2)
	{
		// Isolate and add low nibbles to determine if a carry occurs.
		return (((operand1 & 0x0F) + (operand2 & 0x0F)) & 0x10) == 0x10;
	}
}