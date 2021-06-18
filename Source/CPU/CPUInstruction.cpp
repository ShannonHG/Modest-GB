#include <cassert>
#include <sstream>
#include "CPU/CPUInstruction.hpp"
#include "Globals.hpp"
#include "Logger.hpp"

namespace SHG
{
	std::string CPUInstruction::ToString()
	{
		assert(GetInstructionTypeString().c_str() != NULL);

		std::stringstream stream;

		stream << "Instruction: " << GetInstructionTypeString() << " | ";
		stream << "Opcode: " << ConvertToHexString(opcode, 4) << " | ";
		stream << "Data: ";
		
		for (int d : data) stream << d << " ";

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
			return "Ones Complement (CPL)";
		case CPUInstructionType::RotateLeft:
			return "Rotate Left";
		case CPUInstructionType::RotateRight:
			return "Rotate Right";
		case CPUInstructionType::RotateLeftThroughCarry:
			return "Rotate Left Through Carry";
		case CPUInstructionType::RotateRightThroughCarry:
			return "Rotate Right Through Carry";
		case CPUInstructionType::ArithmeticLeftShift:
			return "Arithmetic Left Shift";
		case CPUInstructionType::ArithmeticRightShift:
			return "Arithmetic Right Shift";
		case CPUInstructionType::LogicalRightShift:
			return "Logical Right Shift";
		case CPUInstructionType::Swap:
			return "Swap";
		case CPUInstructionType::BitTest:
			return "Bit Test";
		case CPUInstructionType::BitReset:
			return "Bit Reset";
		case CPUInstructionType::BitSet:
			return "Bit Set";
		case CPUInstructionType::Jump:
			return "Jump";
		case CPUInstructionType::RelativeJump:
			return "Relative Jump (JR)";
		case CPUInstructionType::Restart:
			return "Restart (RST)";
		case CPUInstructionType::Call:
			return "CALL";
		case CPUInstructionType::Return:
			return "Return (RET)";
		case CPUInstructionType::DisableInterrupts:
			return "Disable Interupts (DI)";
		case CPUInstructionType::EnableInterrupts:
			return "Enable Interrupts (EI)";
		case CPUInstructionType::ReturnAndEnableInterrupts:
			return "Return and Enable Interrupts (RETI)";
		case CPUInstructionType::NOP:
			return "NOP";
		case CPUInstructionType::STOP:
			return "STOP";
		case CPUInstructionType::HALT:
			return "HALT";
		default:
			Logger::WriteError("Invalid instruction type encountered.");
			assert(false);
			break;
		}
	}
}