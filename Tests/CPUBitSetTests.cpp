#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void Test8BitBitSet(CPU& processor, Register8* targetRegister, uint8_t bitNumber)
	{
		uint8_t data = ~(1 << bitNumber);

		targetRegister->SetData(data);
		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::BitSet);
		EXPECT_EQ(prevInstruction.data[0], data);
		EXPECT_EQ(prevInstruction.data[1], bitNumber);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(targetRegister->GetBit(bitNumber), 1);
	}

	void Test8BitBitSetAtAddress(CPU& processor, MockMemoryManagementUnit& mmu, Register16* addressRegister, uint8_t bitNumber)
	{
		uint16_t address = 300;
		uint8_t data = ~(1 << bitNumber);

		mmu.SetByte(address, data);
		addressRegister->SetData(address);
		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::BitSet);
		EXPECT_EQ(prevInstruction.data[0], data);
		EXPECT_EQ(prevInstruction.data[1], bitNumber);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitMemoryData);

		EXPECT_EQ((mmu.GetByte(address) >> bitNumber) & 1, 1);
	}

	// SET 0, B
	// Opcode: 0xCBC0
	TEST(CPUBitSetTests, SET_0_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC0 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 0);
	}

	// SET 0, C
	// Opcode: 0xCBC1
	TEST(CPUBitSetTests, SET_0_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC1 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 0);
	}

	// SET 0, D
	// Opcode: 0xCBC2
	TEST(CPUBitSetTests, SET_0_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC2 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 0);
	}

	// SET 0, E
	// Opcode: 0xCBC3
	TEST(CPUBitSetTests, SET_0_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC3 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 0);
	}

	// SET 0, H
	// Opcode: 0xCBC4
	TEST(CPUBitSetTests, SET_0_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC4 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 0);
	}

	// SET 0, L
	// Opcode: 0xCBC5
	TEST(CPUBitSetTests, SET_0_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC5 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 0);
	}

	// SET 0, (HL)
	// Opcode: 0xCBC6
	TEST(CPUBitSetTests, SET_0_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC6 });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 0);
	}

	// SET 0, A
	// Opcode: 0xCBC7
	TEST(CPUBitSetTests, SET_0_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC7 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 0);
	}

	// SET 1, B
	// Opcode: 0xCBC8
	TEST(CPUBitSetTests, SET_1_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC8 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 1);
	}

	// SET 1, C
	// Opcode: 0xCBC9
	TEST(CPUBitSetTests, SET_1_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xC9 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 1);
	}

	// SET 1, D
	// Opcode: 0xCBCA
	TEST(CPUBitSetTests, SET_1_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCA });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 1);
	}

	// SET 1, E
	// Opcode: 0xCBCB
	TEST(CPUBitSetTests, SET_1_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCB });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 1);
	}

	// SET 1, H
	// Opcode: 0xCBCC
	TEST(CPUBitSetTests, SET_1_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCC });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 1);
	}

	// SET 1, L
	// Opcode: 0xCBCD
	TEST(CPUBitSetTests, SET_1_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCD });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 1);
	}

	// SET 1, (HL)
	// Opcode: 0xCBCE
	TEST(CPUBitSetTests, SET_1_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCE });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 1);
	}

	// SET 1, A
	// Opcode: 0xCBCF
	TEST(CPUBitSetTests, SET_1_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xCF });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 1);
	}

	// SET 2, B
	// Opcode: 0xCBD0
	TEST(CPUBitSetTests, SET_2_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD0 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 2);
	}

	// SET 2, C
	// Opcode: 0xCBD1
	TEST(CPUBitSetTests, SET_2_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD1 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 2);
	}

	// SET 2, D
	// Opcode: 0xCBD2
	TEST(CPUBitSetTests, SET_2_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD2 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 2);
	}

	// SET 2, E
	// Opcode: 0xCBD3
	TEST(CPUBitSetTests, SET_2_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD3 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 2);
	}

	// SET 2, H
	// Opcode: 0xCBD4
	TEST(CPUBitSetTests, SET_2_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD4 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 2);
	}

	// SET 2, L
	// Opcode: 0xCBD5
	TEST(CPUBitSetTests, SET_2_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD5 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 2);
	}

	// SET 2, (HL)
	// Opcode: 0xCBD6
	TEST(CPUBitSetTests, SET_2_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD6 });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 2);
	}

	// SET 2, A
	// Opcode: 0xCBD7
	TEST(CPUBitSetTests, SET_2_F)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD7 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 2);
	}

	// SET 3, B
	// Opcode: 0xCBD8
	TEST(CPUBitSetTests, SET_3_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD8 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 3);
	}

	// SET 3, C
	// Opcode: 0xCBD9
	TEST(CPUBitSetTests, SET_3_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xD9 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 3);
	}

	// SET 3, D
	// Opcode: 0xCBDA
	TEST(CPUBitSetTests, SET_3_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDA });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 3);
	}

	// SET 3, E
	// Opcode: 0xCBDB
	TEST(CPUBitSetTests, SET_3_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDB });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 3);
	}

	// SET 3, H
	// Opcode: 0xCBDC
	TEST(CPUBitSetTests, SET_3_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDC });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 3);
	}

	// SET 3, L
	// Opcode: 0xCBDD
	TEST(CPUBitSetTests, SET_3_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDD });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 3);
	}

	// SET 3, (HL)
	// Opcode: 0xCBDE
	TEST(CPUBitSetTests, SET_3_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDE });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 3);
	}

	// SET 3, A
	// Opcode: 0xCBDF
	TEST(CPUBitSetTests, SET_3_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xDF });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 3);
	}

	// SET 4, B
	// Opcode: 0xCBE0
	TEST(CPUBitSetTests, SET_4_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE0 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 4);
	}

	// SET 4, C
	// Opcode: 0xCBE1
	TEST(CPUBitSetTests, SET_4_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE1 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 4);
	}

	// SET 4, D
	// Opcode: 0xCBE2
	TEST(CPUBitSetTests, SET_4_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE2 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 4);
	}

	// SET 4, E
	// Opcode: 0xCBE3
	TEST(CPUBitSetTests, SET_4_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE3 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 4);
	}

	// SET 4, H
	// Opcode: 0xCBE4
	TEST(CPUBitSetTests, SET_4_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE4 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 4);
	}

	// SET 4, L
	// Opcode: 0xCBE5
	TEST(CPUBitSetTests, SET_4_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE5 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 4);
	}

	// SET 4, (HL)
	// Opcode: 0xCBE6
	TEST(CPUBitSetTests, SET_4_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE6 });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 4);
	}

	// SET 4, A
	// Opcode: 0xCBE7
	TEST(CPUBitSetTests, SET_4_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE7 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 4);
	}

	// SET 5, B
	// Opcode: 0xCBE8
	TEST(CPUBitSetTests, SET_5_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE8 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 5);
	}

	// SET 5, C
	// Opcode: 0xCBE9
	TEST(CPUBitSetTests, SET_5_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xE9 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 5);
	}

	// SET 5, D
	// Opcode: 0xCBEA
	TEST(CPUBitSetTests, SET_5_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xEA });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 5);
	}

	// SET 5, E
	// Opcode: 0xCBEB
	TEST(CPUBitSetTests, SET_5_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xEB });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 5);
	}

	// SET 5, H
	// Opcode: 0xCBEC
	TEST(CPUBitSetTests, SET_5_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xEC });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 5);
	}

	// SET 5, L
	// Opcode: 0xCBED
	TEST(CPUBitSetTests, SET_5_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xED });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 5);
	}

	// SET 5, (HL)
	// Opcode: 0xCBEE
	TEST(CPUBitSetTests, SET_5_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xEE });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 5);
	}

	// SET 5, A
	// Opcode: 0xCBEF
	TEST(CPUBitSetTests, SET_5_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xEF });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 5);
	}

	// SET 6, B
	// Opcode: 0xCBF0
	TEST(CPUBitSetTests, SET_6_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF0 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 6);
	}

	// SET 6, C
	// Opcode: 0xCBF1
	TEST(CPUBitSetTests, SET_6_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF1 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 6);
	}

	// SET 6, D
	// Opcode: 0xCBF2
	TEST(CPUBitSetTests, SET_6_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF2 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 6);
	}

	// SET 6, E
	// Opcode: 0xCBF3
	TEST(CPUBitSetTests, SET_6_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF3 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 6);
	}

	// SET 6, H
	// Opcode: 0xCBF4
	TEST(CPUBitSetTests, SET_6_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF4 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 6);
	}

	// SET 6, L
	// Opcode: 0xCBF5
	TEST(CPUBitSetTests, SET_6_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF5 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 6);
	}

	// SET 6, (HL)
	// Opcode: 0xCBF6
	TEST(CPUBitSetTests, SET_6_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF6 });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 6);
	}

	// SET 6, A
	// Opcode: 0xCBF7
	TEST(CPUBitSetTests, SET_6_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF7 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 6);
	}

	// SET 7, B
	// Opcode: 0xCBF8
	TEST(CPUBitSetTests, SET_7_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF8 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterB(), 7);
	}

	// SET 7, C
	// Opcode: 0xCBF9
	TEST(CPUBitSetTests, SET_7_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xF9 });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterC(), 7);
	}

	// SET 7, D
	// Opcode: 0xCBFA
	TEST(CPUBitSetTests, SET_7_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFA });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterD(), 7);
	}

	// SET 7, E
	// Opcode: 0xCBFB
	TEST(CPUBitSetTests, SET_7_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFB });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterE(), 7);
	}

	// SET 7, H
	// Opcode: 0xCBFC
	TEST(CPUBitSetTests, SET_7_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFC });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterH(), 7);
	}

	// SET 7, L
	// Opcode: 0xCBFD
	TEST(CPUBitSetTests, SET_7_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFD });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterL(), 7);
	}

	// SET 7, (HL)
	// Opcode: 0xCBFE
	TEST(CPUBitSetTests, SET_7_ADDR_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFE });
		auto processor = CPU(mmu);

		Test8BitBitSetAtAddress(processor, mmu, processor.GetRegisterHL(), 7);
	}

	// SET 7, A
	// Opcode: 0xCBFF
	TEST(CPUBitSetTests, SET_7_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit({ 0xCB, 0xFF });
		auto processor = CPU(mmu);

		Test8BitBitSet(processor, processor.GetRegisterA(), 7);
	}
}