#include <cassert>
#include "CPU/CPU.hpp"
#include "Logger.hpp"

namespace SHG
{
	CPU::CPU(MemoryManagementUnit& memoryManagementUnit) : memoryManagementUnit(memoryManagementUnit)
	{
		registers = std::map<CPURegisterID, CPURegister>();
	}

	void CPU::Cycle()
	{
		uint8_t byte;
		if (!TryFetch(byte))
		{
			Logger::WriteWarning("[CPU] Failed to fetch byte");
			return;
		}

		CPUInstruction instruction;
		if (!TryDecode(byte, instruction))
		{
			Logger::WriteWarning("[CPU] Failed to decode byte");
			return;
		}

		Execute(instruction);
	}

	bool CPU::TryFetch(uint8_t& result)
	{
		return false;
	}

	bool CPU::TryDecode(uint8_t byte, CPUInstruction& result)
	{
		return false;
	}

	void CPU::Execute(CPUInstruction& instruction)
	{
		CPURegister* targetRegister = instruction.targetRegister;
		CPURegisterAddressType addressType = instruction.registerAddressType;

		uint16_t result = 0;

		switch (instruction.instructionType)
		{
		case CPUInstructionType::Add:
			result = instruction.data[0] + instruction.data[1];
			break;
		case CPUInstructionType::AddWithCarry:
			break;
		case CPUInstructionType::AND:
			result = instruction.data[0] & instruction.data[1];
			break;
		case CPUInstructionType::DecimalAdjust:
			break;
		case CPUInstructionType::Decrement:
			break;
		case CPUInstructionType::Increment:
			break;
		case CPUInstructionType::Load:
			result = instruction.data[0];
			break;
		case CPUInstructionType::OR:
			result = instruction.data[0] | instruction.data[1];
			break;
		case CPUInstructionType::Subtract:
			result = instruction.data[0] - instruction.data[1];
			break;
		case CPUInstructionType::SubtractWithCarry:
			break;
		case CPUInstructionType::XOR:
			result = instruction.data[0] ^ instruction.data[1];
			break;
		}

		switch (instruction.operandSize)
		{
		case CPUInstructionOperandSize::EightBit:
			StoreValueInRegister(instruction.targetRegister, instruction.registerAddressType, (uint8_t)result);
			break;
		case CPUInstructionOperandSize::SixteenBit:
			StoreValueInRegister(instruction.targetRegister, instruction.registerAddressType, result);
			break;
		}
	}

	void CPU::StoreValueInRegister(CPURegister* targetRegister, CPURegisterAddressType addressType, uint16_t value)
	{
	
	}

	CPURegister& CPU::GetRegister(CPURegisterID registerName)
	{
		assert(registers.count(registerName) != 0);
		return registers[registerName];
	}
}