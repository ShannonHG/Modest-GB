#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void Test8BitRotateLeftThroughCarry(CPU& processor, Register8* targetRegister)
	{
		uint8_t data = 2;
		uint8_t carryFlag = 1;
		uint8_t previousHighBit = (data & 0b10000000) >> 7;

		// Shift left and set the least significant bit equal to the carry flag.
		uint8_t expectedResult = (data << 1) | (carryFlag);

		targetRegister->SetData(data);
		processor.SetCarryFlag((bool)carryFlag);
		processor.Cycle();

		EXPECT_EQ(targetRegister->GetData(), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousHighBit);
	}

	void Test8BitRotateRightThroughCarry(CPU& processor, Register8* targetRegister)
	{
		uint8_t data = 2;
		uint8_t carryFlag = 1;
		uint8_t previousLowBit = (data & 1);

		// Shift right and set the most significant bit equal to the carry flag.
		uint8_t expectedResult = (data >> 1) | (carryFlag << 7);

		targetRegister->SetData(data);
		processor.SetCarryFlag((bool)carryFlag);
		processor.Cycle();

		EXPECT_EQ(targetRegister->GetData(), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousLowBit);
	}

	void Test8BitRotateLeft(CPU& processor, Register8* targetRegister)
	{
		uint8_t data = 8;

		// Shift left and set the least significant bit equal to the previous most significant bit.
		uint8_t expectedResult = (data << 1) | (data >> 7);

		targetRegister->SetData(data);
		processor.Cycle();

		EXPECT_EQ(targetRegister->GetData(), expectedResult);
	}

	void Test8BitRotateRight(CPU& processor, Register8* targetRegister)
	{
		uint8_t data = 4;

		// Shift right and set the most significant bit equal to the previous least significant bit.
		uint8_t expectedResult = (data >> 1) | (data << 7);

		targetRegister->SetData(data);
		processor.Cycle();

		EXPECT_EQ(targetRegister->GetData(), expectedResult);
	}

	// RLC B
	// Opcode 0xCB00
	TEST(CPUCBPrefixedInstructions, RLC_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({0xCB, 0x00});
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterB());
	}

	// RLC C
	// Opcode 0xCB01
	TEST(CPUCBPrefixedInstructions, RLC_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x01 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterC());
	}

	// RLC D
	// Opcode 0xCB02
	TEST(CPUCBPrefixedInstructions, RLC_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x02 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterD());
	}

	// RLC E
	// Opcode 0xCB03
	TEST(CPUCBPrefixedInstructions, RLC_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x03 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterE());
	}

	// RLC H
	// Opcode 0xCB04
	TEST(CPUCBPrefixedInstructions, RLC_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x04 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterH());
	}

	// RLC L
	// Opcode 0xCB05
	TEST(CPUCBPrefixedInstructions, RLC_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x05 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterL());
	}

	// RLC (HL)
	// Opcode 0xCB06
	TEST(CPUCBPrefixedInstructions, RLC_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = (data << 1) | (data >> 7);

		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x06 });
		mmu.SetByte(address, data);

		auto processor = CPU(mmu);
		processor.GetRegisterHL()->SetData(address);
		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), expectedResult);
	}

	// RLC A
	// Opcode 0xCB07
	TEST(CPUCBPrefixedInstructions, RLC_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x07 });
		auto processor = CPU(mmu);

		Test8BitRotateLeft(processor, processor.GetRegisterA());
	}

	// RRC B
	// Opcode 0xCB08
	TEST(CPUCBPrefixedInstructions, RRC_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x08 });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterB());
	}

	// RRC C
	// Opcode 0xCB09
	TEST(CPUCBPrefixedInstructions, RRC_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x09 });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterC());
	}

	// RRC D
	// Opcode 0xCB0A
	TEST(CPUCBPrefixedInstructions, RRC_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0A });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterD());
	}

	// RRC E
	// Opcode 0xCB0B
	TEST(CPUCBPrefixedInstructions, RRC_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0B });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterE());
	}

	// RRC H
	// Opcode 0xCB0C
	TEST(CPUCBPrefixedInstructions, RRC_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0C });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterH());
	}

	// RRC L
	// Opcode 0xCB0D
	TEST(CPUCBPrefixedInstructions, RRC_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0D });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterL());
	}

	// RRC (HL)
	// Opcode 0xCB0E
	TEST(CPUCBPrefixedInstructions, RRC_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = (data >> 1) | (data << 7);

		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0E });
		mmu.SetByte(address, data);

		auto processor = CPU(mmu);
		processor.GetRegisterHL()->SetData(address);
		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), expectedResult);
	}

	// RRC A
	// Opcode 0xCB0F
	TEST(CPUCBPrefixedInstructions, RRC_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x0F });
		auto processor = CPU(mmu);

		Test8BitRotateRight(processor, processor.GetRegisterA());
	}

	// RL B
	// Opcode 0xCB10
	TEST(CPUCBPrefixedInstructions, RL_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x10 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterB());
	}

	// RL C
	// Opcode 0xCB11
	TEST(CPUCBPrefixedInstructions, RL_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x11 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterC());
	}

	// RL D
	// Opcode 0xCB12
	TEST(CPUCBPrefixedInstructions, RL_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x12 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterD());
	}

	// RL E
	// Opcode 0xCB13
	TEST(CPUCBPrefixedInstructions, RL_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x13 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterE());
	}

	// RL H
	// Opcode 0xCB14
	TEST(CPUCBPrefixedInstructions, RL_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x14 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterH());
	}

	// RL L
	// Opcode 0xCB15
	TEST(CPUCBPrefixedInstructions, RL_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x15 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterL());
	}

	// RL (HL)
	// Opcode 0xCB16
	TEST(CPUCBPrefixedInstructions, RL_ADDR_HL)
	{
		uint8_t data = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;
		uint8_t expectedResult = (data << 1) | (carryFlag);
		uint8_t previousHighBit = (data & 0b10000000) >> 7;

		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x16 });
		mmu.SetByte(address, data);

		auto processor = CPU(mmu);
		processor.GetRegisterHL()->SetData(address);
		processor.SetCarryFlag((bool)carryFlag);
		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousHighBit);
	}

	// RL A
	// Opcode 0xCB17
	TEST(CPUCBPrefixedInstructions, RL_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x17 });
		auto processor = CPU(mmu);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterA());
	}

	// RR B
	// Opcode 0xCB18
	TEST(CPUCBPrefixedInstructions, RR_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x18 });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterB());
	}

	// RR C
	// Opcode 0xCB19
	TEST(CPUCBPrefixedInstructions, RR_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x19 });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterC());
	}

	// RR D
	// Opcode 0xCB1A
	TEST(CPUCBPrefixedInstructions, RR_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1A });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterD());
	}

	// RR E
	// Opcode 0xCB1B
	TEST(CPUCBPrefixedInstructions, RR_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1B });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterE());
	}

	// RR H
	// Opcode 0xCB1C
	TEST(CPUCBPrefixedInstructions, RR_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1C });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterH());
	}

	// RR L
	// Opcode 0xCB1D
	TEST(CPUCBPrefixedInstructions, RR_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1D });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterL());
	}

	// RR (HL)
	// Opcode 0xCB1E
	TEST(CPUCBPrefixedInstructions, RR_ADDR_HL)
	{
		uint8_t data = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;
		uint8_t expectedResult = (data >> 1) | (carryFlag << 7);
		uint8_t previousLowBit = (data & 1);

		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1E });
		mmu.SetByte(address, data);

		auto processor = CPU(mmu);
		processor.GetRegisterHL()->SetData(address);
		processor.SetCarryFlag((bool)carryFlag);
		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousLowBit);
	}

	// RR A
	// Opcode 0xCB1F
	TEST(CPUCBPrefixedInstructions, RR_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0x1F });
		auto processor = CPU(mmu);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterA());
	}
}