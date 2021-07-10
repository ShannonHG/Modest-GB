#pragma once

namespace SHG::Arithmetic
{
	bool IsUnsigned8Bit(int num);
	bool IsUnsigned16Bit(int num);
	bool IsHalfCarryRequired(int operand1, int operand2);
}