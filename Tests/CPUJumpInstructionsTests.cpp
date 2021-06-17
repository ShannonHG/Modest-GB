#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void TestStandardJump(CPU& processor, MemoryManagementUnit& mmu, uint16_t address, std::vector<CPUFlag> flags)
	{
		mmu.SetByte(1, address & 0x00FF);
		mmu.SetByte(2, address >> 8);

		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::Jump);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());
		EXPECT_EQ(instruction.flags, flags);
		EXPECT_EQ(instruction.data[0], address);

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), address);
	}

	void TestRelativeJump(CPU& processor, MemoryManagementUnit& mmu, std::vector<CPUFlag> flags)
	{
		uint8_t data = 100;

		// The program counter will be increased to 2 before execution:
		// one increment for the initial byte fetch, and one increment for fetching the 8-bit operand.
		uint16_t programCounterBeforeExecution = 2;

		mmu.SetByte(1, data);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::RelativeJump);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());
		EXPECT_EQ(instruction.flags, flags);
		EXPECT_EQ(instruction.data[0], data);

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), programCounterBeforeExecution + data);
	}

	void TestCall(CPU& processor, MemoryManagementUnit& mmu, std::vector<CPUFlag> flags)
	{
		uint16_t address = 400;
		uint16_t stackPointerData = 320;

		// The program counter will be increased to 3 before execution:
		// one increment for the initial byte fetch, and two increments for fetching a 16 bit number.
		uint8_t programCounterBeforeExecution = 3;

		processor.GetStackPointer()->SetData(stackPointerData);

		mmu.SetByte(1, address & 0x00FF);
		mmu.SetByte(2, address >> 8);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::Call);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());
		EXPECT_EQ(instruction.flags, flags);
		EXPECT_EQ(instruction.data[0], address);

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), address);
		EXPECT_EQ(processor.GetStackPointer()->GetData(), stackPointerData - 2);
		EXPECT_EQ(mmu.GetByte(processor.GetStackPointer()->GetData()), programCounterBeforeExecution);
	}

	void TestReturn(CPU& processor, MemoryManagementUnit& mmu, std::vector<CPUFlag> flags)
	{
		uint16_t address = 400;
		uint16_t memData = 500;

		mmu.SetByte(address, memData & 0x00FF);
		mmu.SetByte(address + 1, memData >> 8);
		processor.GetStackPointer()->SetData(address);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::Return);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());
		EXPECT_EQ(instruction.flags, flags);

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), memData);
		EXPECT_EQ(processor.GetStackPointer()->GetData(), address + 2);
	}

	void TestRestart(CPU& processor, MemoryManagementUnit& mmu, uint8_t data)
	{
		uint16_t stackPointerData = 320;

		// The program counter will be increased to 1 before execution:
		// one increment for the initial byte fetch.
		uint8_t programCounterBeforeExecution = 1;

		processor.GetStackPointer()->SetData(stackPointerData);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::Restart);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());
		EXPECT_EQ(instruction.data[0], data);

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), data);
		EXPECT_EQ(processor.GetStackPointer()->GetData(), stackPointerData - 2);
		EXPECT_EQ(mmu.GetByte(processor.GetStackPointer()->GetData()), programCounterBeforeExecution);
	}

	// JR NZ, I8
	// Opcode: 0x20
	TEST(CPUJumpInstructions, JR_NZ_I8)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x20);
		auto processor = CPU(mmu);

		TestRelativeJump(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Zero });
	}

	// JR NC, I8
	// Opcode: 0x30
	TEST(CPUJumpInstructions, JR_NC_I8)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x30);
		auto processor = CPU(mmu);

		TestRelativeJump(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Carry });
	}

	// JR I8
	// Opcode: 0x18
	TEST(CPUJumpInstructions, JR_I8)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x18);
		auto processor = CPU(mmu);

		TestRelativeJump(processor, mmu, {});
	}

	// JR Z, I8
	// Opcode: 0x28
	TEST(CPUJumpInstructions, JR_Z_I8)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x28);
		auto processor = CPU(mmu);

		TestRelativeJump(processor, mmu, { CPUFlag::Zero});
	}

	// JR C, I8
	// Opcode: 0x38
	TEST(CPUJumpInstructions, JR_C_I8)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x38);
		auto processor = CPU(mmu);

		TestRelativeJump(processor, mmu, { CPUFlag::Carry });
	}

	// RET NZ
	// Opcode: 0xC0
	TEST(CPUJumpInstructions, RET_NZ)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC0);
		auto processor = CPU(mmu);

		TestReturn(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Zero });
	}

	// RET NC
	// Opcode: 0xD0
	TEST(CPUJumpInstructions, RET_NC)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD0);
		auto processor = CPU(mmu);

		TestReturn(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Carry });
	}

	// JP NZ, U16
	// Opcode: 0xC2
	TEST(CPUJumpInstructions, JP_NZ_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC2);
		auto processor = CPU(mmu);

		TestStandardJump(processor, mmu, 320, { CPUFlag::Subtraction, CPUFlag::Zero });
	}

	// JP NC, U16
	// Opcode: 0xD2
	TEST(CPUJumpInstructions, JP_NC_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD2);
		auto processor = CPU(mmu);

		TestStandardJump(processor, mmu, 510, { CPUFlag::Subtraction, CPUFlag::Carry });
	}

	// JP U16
	// Opcode: 0xC3
	TEST(CPUJumpInstructions, JP_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC3);
		auto processor = CPU(mmu);

		TestStandardJump(processor, mmu, 345, {});
	}

	// CALL NZ, U16
	// Opcode: 0xC4
	TEST(CPUJumpInstructions, CALL_NZ_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC4);
		auto processor = CPU(mmu);

		TestCall(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Zero });
	}

	// CALL NC, U16
	// Opcode: 0xD4
	TEST(CPUJumpInstructions, CALL_NC_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD4);
		auto processor = CPU(mmu);

		TestCall(processor, mmu, { CPUFlag::Subtraction, CPUFlag::Carry });
	}

	// RST 00h
	// Opcode: 0xC7
	TEST(CPUJumpInstructions, RST_00H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC7);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x00);
	}

	// RST 10h
	// Opcode: 0xD7
	TEST(CPUJumpInstructions, RST_10H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD7);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x10);
	}

	// RST 20h
	// Opcode: 0xE7
	TEST(CPUJumpInstructions, RST_20H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xE7);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x20);
	}

	// RST 30h
	// Opcode: 0xF7
	TEST(CPUJumpInstructions, RST_30H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xF7);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x30);
	}

	// RET Z
	// Opcode: 0xC8
	TEST(CPUJumpInstructions, RET_Z)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC8);
		auto processor = CPU(mmu);

		TestReturn(processor, mmu, { CPUFlag::Zero });
	}

	// RET C
	// Opcode: 0xD8
	TEST(CPUJumpInstructions, RET_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD8);
		auto processor = CPU(mmu);

		TestReturn(processor, mmu, { CPUFlag::Carry });
	}

	// RET
	// Opcode: 0xC9
	TEST(CPUJumpInstructions, RET)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xC9);
		auto processor = CPU(mmu);

		TestReturn(processor, mmu, {});
	}

	// RETI
	// Opcode: 0xD9
	TEST(CPUJumpInstructions, RETI)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xD9);
		auto processor = CPU(mmu);

		uint16_t address = 400;
		uint16_t memData = 600;

		mmu.SetByte(address, memData & 0x00FF);
		mmu.SetByte(address + 1, memData >> 8);
		processor.GetStackPointer()->SetData(address);
		processor.Cycle();

		CPUInstruction instruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(instruction.instructionType, CPUInstructionType::ReturnAndEnableInterrupts);
		EXPECT_EQ(instruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);
		EXPECT_EQ(instruction.targetRegister16, processor.GetProgramCounter());

		EXPECT_EQ(processor.GetProgramCounter()->GetData(), memData);
		EXPECT_EQ(processor.GetStackPointer()->GetData(), address + 2);

		EXPECT_EQ(processor.GetInterruptMasterEnableFlag(), 1);
	}

	// JP HL
	// Opcode: 0xE9
	TEST(CPUJumpInstructions, JP_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xE9);
		auto processor = CPU(mmu);

		processor.GetRegisterHL()->SetData(367);
		TestStandardJump(processor, mmu, processor.GetRegisterHL()->GetData(), {});
	}

	// JP Z, U16
	// Opcode: 0xCA
	TEST(CPUJumpInstructions, JP_Z_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xCA);
		auto processor = CPU(mmu);

		TestStandardJump(processor, mmu, 678, { CPUFlag::Zero });
	}

	// JP C, U16
	// Opcode: 0xDA
	TEST(CPUJumpInstructions, JP_C_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xDA);
		auto processor = CPU(mmu);

		TestStandardJump(processor, mmu, 700, { CPUFlag::Carry });
	}

	// CALL Z, U16
	// Opcode: 0xCC
	TEST(CPUJumpInstructions, CALL_Z_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xCC);
		auto processor = CPU(mmu);

		TestCall(processor, mmu, { CPUFlag::Zero });
	}

	// CALL C, U16
	// Opcode: 0xDC
	TEST(CPUJumpInstructions, CALL_C_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xDC);
		auto processor = CPU(mmu);

		TestCall(processor, mmu, { CPUFlag::Carry });
	}

	// CALL U16
	// Opcode: 0xCD
	TEST(CPUJumpInstructions, CALL_U16)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xCD);
		auto processor = CPU(mmu);

		TestCall(processor, mmu, {});
	}

	// RST 08h
	// Opcode: 0xCF
	TEST(CPUJumpInstructions, RST_08H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xCF);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x08);
	}

	// RST 18h
	// Opcode: 0xDF
	TEST(CPUJumpInstructions, RST_18H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xDF);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x18);
	}

	// RST 28h
	// Opcode: 0xEF
	TEST(CPUJumpInstructions, RST_28H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xEF);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x28);
	}

	// RST 38h
	// Opcode: 0xFF
	TEST(CPUJumpInstructions, RST_38H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xFF);
		auto processor = CPU(mmu);

		TestRestart(processor, mmu, 0x38);
	}
}