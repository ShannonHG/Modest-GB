#pragma once
#include <vector>
#include "CPU/CPUInstructionType.hpp"
#include "CPU/CPURegister.hpp"
#include "CPU/CPURegisterAddressType.hpp"
#include "CPU/CPUInstructionOperandSize.hpp"

namespace SHG
{
	struct CPUInstruction
	{
		CPUInstructionType instructionType;
		CPURegister* targetRegister;
		CPURegisterAddressType registerAddressType;
		CPUInstructionOperandSize operandSize;
		std::vector<uint16_t> data;
	};
}