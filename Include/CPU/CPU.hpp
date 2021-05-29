#pragma once
#include <cstdint>
#include <array>
#include <map>
#include <vector>
#include "CPUInstruction.hpp"
#include "Memory/MemoryManagementUnit.hpp"
#include "CPU/CPURegisterID.hpp"

namespace SHG
{
	class CPU
	{
	public:
		CPU(MemoryManagementUnit& mmu);

		/// <summary>
		/// Returns the register with the specified name
		/// </summary>
		/// <param name="registerID"></param>
		/// <returns></returns>
		CPURegister& GetRegister(CPURegisterID registerID);

		/// <summary>
		/// Perform a single instruction cycle, and returns the duration of the executed instruction.
		/// </summary>
		uint32_t Cycle();
		
	private:
		static const uint8_t REGISTER_COUNT = 6;

		MemoryManagementUnit& memoryManagementUnit;
		std::map<CPURegisterID, CPURegister> registers;

		uint8_t Fetch8Bit();
		uint16_t Fetch16Bit();
		CPUInstruction Decode(uint8_t opcode);
		uint32_t Execute(const CPUInstruction& instruction);
		void StoreValueInRegister(CPURegister* targetRegister, CPURegisterAddressType addressType, uint16_t value);
		CPUInstruction DecodeLoadAndStoreInstruction(uint8_t opcode);
		CPUInstruction DecodeArithmeticInstruction(uint8_t opcode);

		void SetInstructionData(uint8_t upperNibble, uint8_t lowerNibble, CPUInstruction& instruction);
	};
}