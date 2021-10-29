#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	// TODO: Make jumps dependent on flags

	void TestStandardJump(CPU& processor, DataStorageDevice& memory, uint16_t address)
	{
		memory.Write(1, address & 0x00FF);
		memory.Write(2, address >> 8);

		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), address);
	}

	void TestRelativeJump(CPU& processor, DataStorageDevice& memory)
	{
		uint8_t data = 100;

		// The program counter will be increased to 2 before execution:
		// one increment for the initial byte fetch, and one increment for fetching the 8-bit operand.
		uint16_t programCounterBeforeExecution = 2;

		memory.Write(1, data);
		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), programCounterBeforeExecution + data);
	}

	void TestCall(CPU& processor, DataStorageDevice& memory)
	{
		uint16_t address = 400;
		uint16_t stackPointerData = 320;

		// The program counter will be increased to 3 before execution:
		// one increment for the initial byte fetch, and two increments for fetching a 16 bit number.
		uint8_t programCounterBeforeExecution = 3;

		processor.GetStackPointer().SetData(stackPointerData);

		memory.Write(1, address & 0x00FF);
		memory.Write(2, address >> 8);
		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), address);
		EXPECT_EQ(processor.GetStackPointer().GetData(), stackPointerData - 2);
		EXPECT_EQ(memory.Read(processor.GetStackPointer().GetData()), programCounterBeforeExecution);
	}

	void TestReturn(CPU& processor, DataStorageDevice& memory)
	{
		uint16_t address = 400;
		uint16_t memData = 500;

		memory.Write(address, memData & 0x00FF);
		memory.Write(address + 1, memData >> 8);
		processor.GetStackPointer().SetData(address);
		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), memData);
		EXPECT_EQ(processor.GetStackPointer().GetData(), address + 2);
	}

	void TestRestart(CPU& processor, DataStorageDevice& memory, uint8_t data)
	{
		uint16_t stackPointerData = 320;

		// The program counter will be increased to 1 before execution:
		// one increment for the initial byte fetch.
		uint8_t programCounterBeforeExecution = 1;

		processor.GetStackPointer().SetData(stackPointerData);
		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), data);
		EXPECT_EQ(processor.GetStackPointer().GetData(), stackPointerData - 2);
		EXPECT_EQ(memory.Read(processor.GetStackPointer().GetData()), programCounterBeforeExecution);
	}

	// JR NZ, I8
	// Opcode: 0x20
	TEST(CPUJumpInstructions, JR_NZ_I8)
	{
		Memory memory = CreatePresetMemory(0x20);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(false);
		TestRelativeJump(processor, memory);
	}

	// JR NC, I8
	// Opcode: 0x30
	TEST(CPUJumpInstructions, JR_NC_I8)
	{
		Memory memory = CreatePresetMemory(0x30);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(false);
		TestRelativeJump(processor, memory);
	}

	// JR I8
	// Opcode: 0x18
	TEST(CPUJumpInstructions, JR_I8)
	{
		Memory memory = CreatePresetMemory(0x18);
		auto processor = CPU(memory);
		
		TestRelativeJump(processor, memory);
	}

	// JR Z, I8
	// Opcode: 0x28
	TEST(CPUJumpInstructions, JR_Z_I8)
	{
		Memory memory = CreatePresetMemory(0x28);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(true);
		TestRelativeJump(processor, memory);
	}

	// JR C, I8
	// Opcode: 0x38
	TEST(CPUJumpInstructions, JR_C_I8)
	{
		Memory memory = CreatePresetMemory(0x38);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(true);
		TestRelativeJump(processor, memory);
	}

	// RET NZ
	// Opcode: 0xC0
	TEST(CPUJumpInstructions, RET_NZ)
	{
		Memory memory = CreatePresetMemory(0xC0);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(false);
		TestReturn(processor, memory);
	}

	// RET NC
	// Opcode: 0xD0
	TEST(CPUJumpInstructions, RET_NC)
	{
		Memory memory = CreatePresetMemory(0xD0);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(false);
		TestReturn(processor, memory);
	}

	// JP NZ, U16
	// Opcode: 0xC2
	TEST(CPUJumpInstructions, JP_NZ_U16)
	{
		Memory memory = CreatePresetMemory(0xC2);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(false);
		TestStandardJump(processor, memory, 320);
	}

	// JP NC, U16
	// Opcode: 0xD2
	TEST(CPUJumpInstructions, JP_NC_U16)
	{
		Memory memory = CreatePresetMemory(0xD2);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(false);
		TestStandardJump(processor, memory, 510);
	}

	// JP U16
	// Opcode: 0xC3
	TEST(CPUJumpInstructions, JP_U16)
	{
		Memory memory = CreatePresetMemory(0xC3);
		auto processor = CPU(memory);

		TestStandardJump(processor, memory, 345);
	}

	// CALL NZ, U16
	// Opcode: 0xC4
	TEST(CPUJumpInstructions, CALL_NZ_U16)
	{
		Memory memory = CreatePresetMemory(0xC4);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(false);
		TestCall(processor, memory);
	}

	// CALL NC, U16
	// Opcode: 0xD4
	TEST(CPUJumpInstructions, CALL_NC_U16)
	{
		Memory memory = CreatePresetMemory(0xD4);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(false);
		TestCall(processor, memory);
	}

	// RST 00h
	// Opcode: 0xC7
	TEST(CPUJumpInstructions, RST_00H)
	{
		Memory memory = CreatePresetMemory(0xC7);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x00);
	}

	// RST 10h
	// Opcode: 0xD7
	TEST(CPUJumpInstructions, RST_10H)
	{
		Memory memory = CreatePresetMemory(0xD7);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x10);
	}

	// RST 20h
	// Opcode: 0xE7
	TEST(CPUJumpInstructions, RST_20H)
	{
		Memory memory = CreatePresetMemory(0xE7);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x20);
	}

	// RST 30h
	// Opcode: 0xF7
	TEST(CPUJumpInstructions, RST_30H)
	{
		Memory memory = CreatePresetMemory(0xF7);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x30);
	}

	// RET Z
	// Opcode: 0xC8
	TEST(CPUJumpInstructions, RET_Z)
	{
		Memory memory = CreatePresetMemory(0xC8);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(true);
		TestReturn(processor, memory);
	}

	// RET C
	// Opcode: 0xD8
	TEST(CPUJumpInstructions, RET_C)
	{
		Memory memory = CreatePresetMemory(0xD8);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(true);
		TestReturn(processor, memory);
	}

	// RET
	// Opcode: 0xC9
	TEST(CPUJumpInstructions, RET)
	{
		Memory memory = CreatePresetMemory(0xC9);
		auto processor = CPU(memory);

		TestReturn(processor, memory);
	}

	// RETI
	// Opcode: 0xD9
	TEST(CPUJumpInstructions, RETI)
	{
		Memory memory = CreatePresetMemory(0xD9);
		auto processor = CPU(memory);

		uint16_t address = 400;
		uint16_t memData = 600;

		memory.Write(address, memData & 0x00FF);
		memory.Write(address + 1, memData >> 8);
		processor.GetStackPointer().SetData(address);
		processor.Step();

		EXPECT_EQ(processor.GetProgramCounter().GetData(), memData);
		EXPECT_EQ(processor.GetStackPointer().GetData(), address + 2);

		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), 1);
	}

	// JP HL
	// Opcode: 0xE9
	TEST(CPUJumpInstructions, JP_HL)
	{
		Memory memory = CreatePresetMemory(0xE9);
		auto processor = CPU(memory);

		processor.GetRegisterHL().SetData(367);
		TestStandardJump(processor, memory, processor.GetRegisterHL().GetData());
	}

	// JP Z, U16
	// Opcode: 0xCA
	TEST(CPUJumpInstructions, JP_Z_U16)
	{
		Memory memory = CreatePresetMemory(0xCA);
		auto processor = CPU(memory);

		processor.ChangeZeroFlag(true);
		TestStandardJump(processor, memory, 678);
	}

	// JP C, U16
	// Opcode: 0xDA
	TEST(CPUJumpInstructions, JP_C_U16)
	{
		Memory memory = CreatePresetMemory(0xDA);
		auto processor = CPU(memory);

		processor.ChangeCarryFlag(true);
		TestStandardJump(processor, memory, 700);
	}

	// CALL Z, U16
	// Opcode: 0xCC
	TEST(CPUJumpInstructions, CALL_Z_U16)
	{
		Memory memory = CreatePresetMemory(0xCC);
		auto processor = CPU(memory);
		processor.ChangeZeroFlag(true);
		TestCall(processor, memory);
	}

	// CALL C, U16
	// Opcode: 0xDC
	TEST(CPUJumpInstructions, CALL_C_U16)
	{
		Memory memory = CreatePresetMemory(0xDC);
		auto processor = CPU(memory);
		processor.ChangeCarryFlag(true);
		TestCall(processor, memory);
	}

	// CALL U16
	// Opcode: 0xCD
	TEST(CPUJumpInstructions, CALL_U16)
	{
		Memory memory = CreatePresetMemory(0xCD);
		auto processor = CPU(memory);

		TestCall(processor, memory);
	}

	// RST 08h
	// Opcode: 0xCF
	TEST(CPUJumpInstructions, RST_08H)
	{
		Memory memory = CreatePresetMemory(0xCF);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x08);
	}

	// RST 18h
	// Opcode: 0xDF
	TEST(CPUJumpInstructions, RST_18H)
	{
		Memory memory = CreatePresetMemory(0xDF);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x18);
	}

	// RST 28h
	// Opcode: 0xEF
	TEST(CPUJumpInstructions, RST_28H)
	{
		Memory memory = CreatePresetMemory(0xEF);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x28);
	}

	// RST 38h
	// Opcode: 0xFF
	TEST(CPUJumpInstructions, RST_38H)
	{
		Memory memory = CreatePresetMemory(0xFF);
		auto processor = CPU(memory);

		TestRestart(processor, memory, 0x38);
	}
}