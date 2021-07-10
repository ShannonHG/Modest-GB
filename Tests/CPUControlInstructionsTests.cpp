#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	// NOP
	// Opcode: 0x00
	TEST(CPUControlInstructions, NOP)
	{
		Memory memory = CreatePresetMemory(0x00);
		auto processor = CPU(memory);

		processor.Cycle();

		// TODO: Implement
	}

	// STOP
	// Opcode: 0x10
	TEST(CPUControlInstructions, STOP)
	{
		Memory memory = CreatePresetMemory(0x10);
		auto processor = CPU(memory);

		processor.Cycle();

		// TODO: Implement
	}

	// HALT
	// Opcode: 0x76
	TEST(CPUControlInstructions, HALT)
	{
		Memory memory = CreatePresetMemory(0x76);
		auto processor = CPU(memory);

		processor.Cycle();

		// TODO: Implement
	}

	// DI
	// Opcode: 0xF3
	TEST(CPUControlInstructions, DI)
	{
		Memory memory = CreatePresetMemory(0xF3);
		auto processor = CPU(memory);

		processor.ChangeInterruptMasterEnableFlag(true);
		processor.Cycle();
		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), false);
	}

	// EI
	// Opcode: 0xFB
	TEST(CPUControlInstructions, EI)
	{
		Memory memory = CreatePresetMemory(0xFB);
		auto processor = CPU(memory);

		processor.ChangeInterruptMasterEnableFlag(false);
		processor.Cycle();

		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), true);
	}
}