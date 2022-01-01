#include <limits>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include "Utils/Arithmetic.hpp"

namespace SHG::Arithmetic
{
	bool SHG::Arithmetic::GetBit(uint8_t data, uint8_t bitIndex)
	{
		return (data >> bitIndex) & 1;
	}

	uint32_t SHG::Arithmetic::GetBits(uint32_t data, uint8_t start, uint8_t end)
	{
		return (data >> start) & static_cast<uint32_t>(std::pow(2, (end - start) + 1) - 1);
	}

	bool SHG::Arithmetic::Is8BitOverflow(int num)
	{
		return num < std::numeric_limits<uint8_t>::min() || num > std::numeric_limits<uint8_t>::max();
	}

	bool SHG::Arithmetic::Is16BitOverflow(int num)
	{
		return num < std::numeric_limits<uint16_t>::min() || num > std::numeric_limits<uint16_t>::max();
	}

	bool SHG::Arithmetic::Is8BitHalfCarryRequired(std::vector<uint8_t> operands, bool isSubtraction)
	{
		int sum = 0;

		for (int i = 0; i < operands.size(); i++)
		{
			int8_t sign = i == 0 ? 1 : (isSubtraction ? -1 : 1);

			// In 8-bit operations, a half carry happens when there is a carry 
			// from bit 3 to 4
			sum += sign * (operands[i] & 0x0F);
		}

		return (sum & 0xF0) != 0;
	}

	bool SHG::Arithmetic::Is16BitHalfCarryRequired(std::vector<uint16_t> operands, bool isSubtraction)
	{
		int sum = 0;

		for (int i = 0; i < operands.size(); i++)
		{
			int8_t sign = i == 0 ? 1 : (isSubtraction ? -1 : 1);

			// In 16-bit operations, a half carry happens when there is a carry 
			// from bit 11 to 12
			sum += sign * (operands[i] & 0x0FFF);
		}

		return (sum & 0xF000) != 0;
	}

	void SHG::Arithmetic::ChangeBit(uint8_t& data, uint8_t bitIndex, bool isSet)
	{
		if (isSet) 
			SetBit(data, bitIndex);
		else 
			ClearBit(data, bitIndex);
	}

	void SHG::Arithmetic::SetBit(uint8_t& data, uint8_t bitIndex)
	{
		data |= (1 << bitIndex);
	}

	void SHG::Arithmetic::ClearBit(uint8_t& data, uint8_t bitIndex)
	{
		data &= ~(1 << bitIndex);
	}

	uint32_t SHG::Arithmetic::EvenCeil(uint32_t operand)
	{
		uint8_t remainder = (operand % 2);

		if (remainder == 0)
			return operand;

		return operand - remainder + 2;
	}

	bool Arithmetic::IsInRange(int operand, int lowerLimit, int upperLimit)
	{
		return operand >= lowerLimit && operand <= upperLimit;
	}

	int Arithmetic::Normalize(int operand, int lowerBound, int upperBound)
	{
		return std::clamp(operand - lowerBound, 0, upperBound);
	}
}