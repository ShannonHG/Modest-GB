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
		/// Perform a single instruction cycle
		/// </summary>
		void Cycle();
		
	private:
		static const uint8_t REGISTER_COUNT = 6;

		MemoryManagementUnit& memoryManagementUnit;
		std::map<CPURegisterID, CPURegister> registers;

		bool TryFetch(uint8_t& result);
		bool TryDecode(uint8_t byte, CPUInstruction& result);
		void Execute(CPUInstruction& instruction);
		void StoreValueInRegister(CPURegister* targetRegister, CPURegisterAddressType addressType, uint16_t value);
	};
}