#include <cassert>
#include <sstream>
#include "CPU/CPUInstruction.hpp"
#include "Globals.hpp"

namespace SHG
{
	std::string CPUInstruction::ToString()
	{
		std::stringstream stream;

		stream << "Instruction: " << GetInstructionTypeString() << " | ";
		stream << "Opcode: " << ConvertToHexString(opcode, 4) << " | ";
		stream << "Data: ";
		for (auto d : data) stream << d << " ";

		return stream.str();
	}

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
		case CPUInstructionType::RotateLeft:
			return "Rotate Left";
		case CPUInstructionType::RotateRight:
			return "Rotate Right";
		case CPUInstructionType::RotateLeftThroughCarry:
			return "Rotate Left Through Carry";
		case CPUInstructionType::RotateRightThroughCarry:
			return "Rotate Right Through Carry";
		default:
			assert(false);
		}
	}
}