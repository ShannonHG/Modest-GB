#pragma once
#include <vector>
#include <string>

namespace SHG::Arithmetic
{
	bool Is8BitOverflow(int num);
	bool Is16BitOverflow(int num);
	bool Is8BitHalfCarryRequired(std::vector<uint8_t> operands, bool isSubtraction = false);
	bool Is16BitHalfCarryRequired(std::vector<uint16_t> operands, bool isSubtraction = false);
	bool GetBit(uint8_t data, uint8_t bitIndex);
	void ChangeBit(uint8_t& data, uint8_t bitIndex, bool isSet);
	void SetBit(uint8_t& data, uint8_t bitIndex);
	void ResetBit(uint8_t& data, uint8_t bitIndex);
}