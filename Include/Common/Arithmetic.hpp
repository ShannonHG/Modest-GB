#pragma once

namespace SHG
{
	bool IsUnsigned8Bit(int num);
	bool IsUnsigned16Bit(int num);
	bool IsHalfCarryRequired(int operand1, int operand2);
	void ChangeBit(uint8_t& data, uint8_t bitIndex, bool isSet);
	void SetBit(uint8_t& data, uint8_t bitIndex);
	void ResetBit(uint8_t& data, uint8_t bitIndex);
}