#pragma once

namespace SHG
{
	enum class CPUInstructionType
	{
		Add, 
		AddWithCarry,
		Subtract,
		SubtractWithCarry,
		Increment,
		Decrement,
		DecimalAdjust,
		Load, 
		OR,
		XOR,
		AND,
	};
}