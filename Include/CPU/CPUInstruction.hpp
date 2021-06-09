#pragma once
#include <vector>
#include <string>
#include "CPU/CPUInstructionType.hpp"
#include "Memory/Register16.hpp"
#include "CPU/CPUInstructionStorageType.hpp"

namespace SHG
{
	/// <summary>
	/// An instruction to be executed by a <see cref="CPU"/>.
	/// </summary>
	struct CPUInstruction
	{
		/// <summary>
		/// The operation that should be performed when this instruction is executed.
		/// </summary>
		CPUInstructionType instructionType{};

		/// <summary>
		/// The opcode associated with this instruction.
		/// </summary>
		uint16_t opcode;

		/// <summary>
		/// The 8-bit register that the instruction's result should be stored in.
		/// </summary>
		Register8* targetRegister8 = NULL;

		/// <summary>
		/// The 16-bit register that the instruction's result should be stored in.
		/// </summary>
		Register16* targetRegister16 = NULL;

		/// <summary>
		/// Determines whether the target storae location is 8-bit or 16-bit, and 
		/// a register or location in memory.
		/// </summary>
		CPUInstructionStorageType storageType{};

		/// <summary>
		/// The data to be stored or used in calculations.
		/// </summary>
		std::vector<int> data{};

		/// <summary>
		/// How many cycles the instruction takes to complete.
		/// </summary>
		uint32_t duration{};

		/// <summary>
		/// The memory address, if applicable, to store the result of the instruction after execution.
		/// </summary>
		uint16_t targetAddress{};

		/// <summary>
		/// Returns a nicely formatted string representation of the instruction type.
		/// </summary>
		/// <returns></returns>
		std::string GetInstructionTypeString();

		std::string ToString();
	};
}