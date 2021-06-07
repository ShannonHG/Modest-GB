#include "CPU/CPUInstruction.hpp"

namespace SHG
{
	std::string CPUInstruction::GetInstructionTypeString()
	{
		switch (instructionType)
		{
		case CPUInstructionType::Add:
			return "Addition";
		case CPUInstructionType::AddWithCarry:
			return "Addition With Carry";
		case CPUInstructionType::AND:
			return "AND";
		case CPUInstructionType::DecimalAdjust:
			return "Decimal Adjust";
		case CPUInstructionType::Decrement:
			return "Decrement";
		case CPUInstructionType::Increment:
			return "Increment";
		case CPUInstructionType::Load:
			return "Load";
		case CPUInstructionType::OR:
			return "OR";
		case CPUInstructionType::Subtract:
			return "Subtraction";
		case CPUInstructionType::SubtractWithCarry:
			return "Subtraction With Carry";
		case CPUInstructionType::XOR:
			return "XOR";
		case CPUInstructionType::Compare:
			return "Compare";
		case CPUInstructionType::FlipCarryFlag:
			return "Carry Flag Flip";
		case CPUInstructionType::SetCarryFlag:
			return "Set Carry Flag";
		case CPUInstructionType::OnesComplement:
			return "Ones Complement";
		}
	}
}