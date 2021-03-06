#pragma once
#include <vector>
#include <string>

namespace ModestGB::Arithmetic
{
	bool Is8BitOverflow(int num);
	bool Is16BitOverflow(int num);
	bool Is8BitHalfCarryRequired(const std::vector<uint8_t>& operands, bool isSubtraction = false);
	bool Is16BitHalfCarryRequired(const std::vector<uint16_t>& operands, bool isSubtraction = false);
	bool GetBit(uint8_t data, uint8_t bitIndex);
	uint32_t GetBits(uint32_t data, uint8_t start, uint8_t end);
	void ChangeBit(uint8_t& data, uint8_t bitIndex, bool isSet);
	void SetBit(uint8_t& data, uint8_t bitIndex);
	void ClearBit(uint8_t& data, uint8_t bitIndex);
	uint32_t EvenCeil(uint32_t operand);
	bool IsInRange(int operand, int lowerLimit, int upperLimit);

	// Remaps the address to a value between 0 and [upperBound - lowerBound].
	uint16_t NormalizeAddress(uint16_t operand, uint16_t lowerBound, uint16_t upperBound);
}