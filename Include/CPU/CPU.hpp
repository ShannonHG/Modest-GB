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
	/// <summary>
	/// Data structure representing a GameBoy's central processing unit.
	/// </summary>
	class CPU
	{
	public:
		CPU(MemoryManagementUnit& mmu);

		/// <summary>
		/// Returns the 16-bit register with the specified ID
		/// </summary>
		/// <param name="registerID"></param>
		/// <returns></returns>
		Register16& GetRegister(CPURegisterID registerID);

		/// <summary>
		/// Perform a single instruction cycle, and returns the duration of the executed instruction.
		/// </summary>
		uint32_t Cycle();

		/// <summary>
		/// Returns the value of the zero flag.
		/// </summary>
		/// <returns></returns>
		uint8_t GetZeroFlag();

		/// <summary>
		/// Returns the value of the subtraction flag.
		/// </summary>
		/// <returns></returns>
		uint8_t GetSubtractionFlag();

		/// <summary>
		/// Returns the value of the half carry flag.
		/// </summary>
		/// <returns></returns>
		uint8_t GetHalfCarryFlag();

		/// <summary>
		/// Returns the value of the carry flag.
		/// </summary>
		/// <returns></returns>
		uint8_t GetCarryFlag();

		/// <summary>
		/// Sets the value of the zero flag.
		/// </summary>
		/// <param name="enabled"></param>
		void SetZeroFlag(bool enabled);

		/// <summary>
		/// Sets the value of the subtraction flag.
		/// </summary>
		/// <param name="enabled"></param>
		void SetSubtractionFlag(bool enabled);

		/// <summary>
		/// Sets the value of the half carry flag.
		/// </summary>
		/// <param name="enabled"></param>
		void SetHalfCarryFlag(bool enabled);

		/// <summary>
		/// Sets the value of the carry flag.
		/// </summary>
		/// <param name="enabled"></param>
		void SetCarryFlag(bool enabled);

		void SetInterruptMasterEnableFlag(bool enable);
		bool GetInterruptMasterEnableFlag();

		Register8* GetRegisterA();
		Register8* GetRegisterF();
		Register8* GetRegisterB();
		Register8* GetRegisterC();
		Register8* GetRegisterD();
		Register8* GetRegisterE();
		Register8* GetRegisterH();
		Register8* GetRegisterL();

		Register16* GetRegisterAF();
		Register16* GetRegisterBC();
		Register16* GetRegisterDE();
		Register16* GetRegisterHL();

		Register16* GetProgramCounter();
		Register16* GetStackPointer();

		CPUInstruction GetPreviouslyExecutedInstruction();

	private:
		MemoryManagementUnit& memoryManagementUnit;
		std::map<CPURegisterID, Register16> registers;

		bool interruptMasterEnableFlag;

		Register16* regAF;
		Register16* regBC;
		Register16* regDE;
		Register16* regHL;
		Register16* programCounter;
		Register16* stackPointer;

		CPUInstruction previouslyExecutedInstruction;

		uint8_t Fetch8Bit();
		uint16_t Fetch16Bit();

		CPUInstruction Decode(uint8_t opcode);
		uint32_t Execute(const CPUInstruction& instruction);

		void DecodeCBPrefixedInstruction(CPUInstruction& instruction);
		void DecodeLoadAndStoreInstruction(CPUInstruction& instruction);
		void DecodeArithmeticInstruction(CPUInstruction& instruction);

		uint8_t GenerateDataFromOpcode(uint8_t opcode);
		Register8* Get8BitRegisterFromOpcode(uint8_t opcode);

		bool ShouldEnableCarryFlag(CPUInstructionStorageType storageType, int operationResult);
		bool ShouldEnableHalfCarryFlag(uint16_t operand1, uint16_t operand2);

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

		void CreateStandardJumpInstruction(CPUInstruction& instruction, uint16_t data, std::vector<CPUFlag> flags);
		void CreateRelativeJumpInstruction(CPUInstruction& instruction, uint8_t data, std::vector<CPUFlag> flags);

		void CreateCallInstruction(CPUInstruction& instruction, uint16_t data, std::vector<CPUFlag> flags);
		void CreateRestartInstruction(CPUInstruction& instruction, uint8_t data);
		void CreateReturnInstruction(CPUInstruction& instruction, std::vector<CPUFlag> flags);
		void CreateInterruptDisableInstruction(CPUInstruction& instruction);
		void CreateInterruptEnableInstruction(CPUInstruction& instruction);

		void CreatePopInstruction(CPUInstruction& instruction, uint16_t data);
		void CreatePushInstruction(CPUInstruction& instruction, uint16_t data);

		void Set16BitDataInMemory(uint16_t address, uint16_t data);
	};
}