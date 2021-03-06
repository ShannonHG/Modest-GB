#include <limits>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include "Utils/Arithmetic.hpp"

namespace ModestGB::Arithmetic
{
	bool ModestGB::Arithmetic::GetBit(uint8_t data, uint8_t bitIndex)
	{
		return (data >> bitIndex) & 1;
	}

	uint32_t ModestGB::Arithmetic::GetBits(uint32_t data, uint8_t start, uint8_t end)
	{
		return (data >> start) & static_cast<uint32_t>(std::pow(2, (end - start) + 1) - 1);
	}

	bool ModestGB::Arithmetic::Is8BitOverflow(int num)
	{
		return num < std::numeric_limits<uint8_t>::min() || num > std::numeric_limits<uint8_t>::max();
	}

	bool ModestGB::Arithmetic::Is16BitOverflow(int num)
	{
		return num < std::numeric_limits<uint16_t>::min() || num > std::numeric_limits<uint16_t>::max();
	}

	bool ModestGB::Arithmetic::Is8BitHalfCarryRequired(const std::vector<uint8_t>& operands, bool isSubtraction)
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

	bool ModestGB::Arithmetic::Is16BitHalfCarryRequired(const std::vector<uint16_t>& operands, bool isSubtraction)
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

	void ModestGB::Arithmetic::ChangeBit(uint8_t& data, uint8_t bitIndex, bool isSet)
	{
		if (isSet) 
			SetBit(data, bitIndex);
		else 
			ClearBit(data, bitIndex);
	}

	void ModestGB::Arithmetic::SetBit(uint8_t& data, uint8_t bitIndex)
	{
		data |= (1 << bitIndex);
	}

	void ModestGB::Arithmetic::ClearBit(uint8_t& data, uint8_t bitIndex)
	{
		data &= ~(1 << bitIndex);
	}

	uint32_t ModestGB::Arithmetic::EvenCeil(uint32_t operand)
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

	uint16_t Arithmetic::NormalizeAddress(uint16_t address, uint16_t lowerBound, uint16_t upperBound)
	{
		int32_t result = address - lowerBound;

		// std::clamp could be used here, but doing this calculation manually appears to be a bit faster.
		return result < 0 ? 0 : result > upperBound ? upperBound : result;
	}
}