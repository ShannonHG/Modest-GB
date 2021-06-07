#pragma once
#include <cstdint>
#include <array>
#include <map>
#include <vector>
#include "CPUInstruction.hpp"
#include "Memory/MemoryManagementUnit.hpp"
#include "CPU/CPURegisterID.hpp"
#include "Memory/Register16.hpp"

namespace SHG
{
	class CPU
	{
	public:
		CPU(MemoryManagementUnit& mmu);

		/// <summary>
		/// Returns the register with the specified ID
		/// </summary>
		/// <param name="registerID"></param>
		/// <returns></returns>
		Register16& GetRegister(CPURegisterID registerID);

		/// <summary>
		/// Perform a single instruction cycle, and returns the duration of the executed instruction.
		/// </summary>
		uint32_t Cycle();

		uint8_t GetZeroFlag();
		uint8_t GetSubtractionFlag();
		uint8_t GetHalfCarryFlag();
		uint8_t GetCarryFlag();

		void SetZeroFlag(bool enabled);
		void SetSubtractionFlag(bool enabled);
		void SetHalfCarryFlag(bool enabled);
		void SetCarryFlag(bool enabled);

	private:
		static const uint8_t REGISTER_COUNT = 6;

		MemoryManagementUnit& memoryManagementUnit;
		std::map<CPURegisterID, Register16> registers;

		Register16* regAF;
		Register16* regBC;
		Register16* regDE;
		Register16* regHL;
		Register16* programCounter;
		Register16* stackPointer;

		uint8_t Fetch8Bit();
		uint16_t Fetch16Bit();
		CPUInstruction Decode(uint8_t opcode);
		uint32_t Execute(const CPUInstruction& instruction);
		CPUInstruction DecodeLoadAndStoreInstruction(uint8_t opcode);
		CPUInstruction DecodeArithmeticInstruction(uint8_t opcode);

		bool ShouldEnableCarryFlag(const CPUInstruction& instruction, int operationResult);
		bool ShouldEnableHalfCarryFlag(uint16_t operand1, uint16_t operand2);

		void SetInstructionData(uint8_t upperNibble, uint8_t lowerNibble, CPUInstruction& instruction);

		void Create8BitIncrementInstruction(CPUInstruction& instruction, Register8* targetRegister);
		void Create8BitIncrementInstruction(CPUInstruction& instruction, uint16_t targetAddress);
		void Create16BitIncrementInstruction(CPUInstruction& instruction, Register16* targetRegister);
		void Create16BitIncrementInstruction(CPUInstruction& instruction, uint16_t targetAddress);
		
		void Create8BitDecrementInstruction(CPUInstruction& instruction, Register8* targetRegister);
		void Create16BitDecrementInstruction(CPUInstruction& instruction, Register16* targetRegister);
		void Create8BitDecrementInstruction(CPUInstruction& instruction, uint16_t targetAddress);

		void Create8BitLoadInstruction(CPUInstruction& instruction, Register8* targetRegister, uint8_t data);
		void Create8BitLoadInstruction(CPUInstruction& instruction, uint16_t targetAddress, uint8_t data);
		void Create16BitLoadInstruction(CPUInstruction& instruction, Register16* targetRegister, uint16_t data);
		void Create16BitLoadInstruction(CPUInstruction& instruction, uint16_t targetAddress, uint16_t data);

		void Create8BitAddInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
		void Create16BitAddInstruction(CPUInstruction& instruction, Register16* storageRegister, uint16_t operand);
		void Create8BitAddWithCarryInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
	
		void Create8BitSubtractInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
		void Create8BitSubtractWithCarryInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);

		void Create8BitXORInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
		void Create8BitORInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
		void Create8BitANDInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
		void Create8BitCompareInstruction(CPUInstruction& instruction, Register8* storageRegister, uint8_t operand);
	};
}