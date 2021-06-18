#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	// NOP
	// Opcode: 0x00
	TEST(CPUControlInstructions, NOP)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x00);
		auto processor = CPU(mmu);

		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::NOP);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::None);
	}

	// STOP
	// Opcode: 0x10
	TEST(CPUControlInstructions, STOP)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x10);
		auto processor = CPU(mmu);

		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::STOP);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::None);
	}

	// HALT
	// Opcode: 0x76
	TEST(CPUControlInstructions, HALT)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x76);
		auto processor = CPU(mmu);

		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::HALT);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::None);
	}

	// DI
	// Opcode: 0xF3
	TEST(CPUControlInstructions, DI)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xF3);
		auto processor = CPU(mmu);

		processor.SetInterruptMasterEnableFlag(true);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::DisableInterrupts);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::None);

		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), false);
	}

	// EI
	// Opcode: 0xFB
	TEST(CPUControlInstructions, EI)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xFB);
		auto processor = CPU(mmu);

		processor.SetInterruptMasterEnableFlag(false);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::EnableInterrupts);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::None);

		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), true);
	}
}