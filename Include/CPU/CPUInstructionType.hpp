#pragma once

namespace SHG
{
	enum class CPUInstructionType
	{
		Invalid,
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
		Compare,
		FlipCarryFlag,
		SetCarryFlag,
		OnesComplement,
		RotateLeft,
		RotateRight,
		RotateLeftThroughCarry,
		RotateRightThroughCarry,
		ArithmeticRightShift,
		ArithmeticLeftShift,
		LogicalRightShift,
		Swap,
		BitTest,
		BitReset,
		BitSet
	};
}