#pragma once
#include <vector>
#include <string>
#include "CPU/CPUInstructionType.hpp"
#include "Memory/Register16.hpp"
#include "CPU/CPUInstructionStorageType.hpp"

namespace SHG
{
	struct CPUInstruction
	{
		CPUInstructionType instructionType{};
		Register16* targetRegister16 = NULL;
		Register8* targetRegister8 = NULL;
		CPUInstructionStorageType storageType{};
		std::vector<int> data{};
		uint32_t duration{};
		uint16_t targetAddress{};

		std::string GetInstructionTypeString();
	};
}