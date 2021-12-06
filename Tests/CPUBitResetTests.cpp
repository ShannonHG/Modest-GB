#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void Test8BitBitReset(CPU& processor, Register8& targetRegister, uint8_t bitNumber)
	{
		uint8_t data = 1 << bitNumber;

		targetRegister.Write(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetBit(bitNumber), 0);
	}

	void Test8BitBitResetAtAddress(CPU& processor, Memory& memory, Register16& addressRegister, uint8_t bitNumber)
	{
		uint16_t address = 300;
		uint8_t data = 1 << bitNumber;

		memory.Write(address, data);
		addressRegister.Write(address);
		processor.Step();

		EXPECT_EQ(memory.Read(address) & (1 << bitNumber), 0);
	}

	// RES 0, B
	// Opcode: 0xCB80
	TEST(CPUBitResetTests, RES_0_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x80 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 0);
	}

	// RES 0, C
	// Opcode: 0xCB81
	TEST(CPUBitResetTests, RES_0_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x81 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 0);
	}

	// RES 0, D
	// Opcode: 0xCB82
	TEST(CPUBitResetTests, RES_0_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x82 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 0);
	}

	// RES 0, E
	// Opcode: 0xCB83
	TEST(CPUBitResetTests, RES_0_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x83 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 0);
	}

	// RES 0, H
	// Opcode: 0xCB84
	TEST(CPUBitResetTests, RES_0_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x84 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 0);
	}

	// RES 0, L
	// Opcode: 0xCB85
	TEST(CPUBitResetTests, RES_0_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x85 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 0);
	}

	// RES 0, (HL)
	// Opcode: 0xCB86
	TEST(CPUBitResetTests, RES_0_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x86 });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 0);
	}

	// RES 0, A
	// Opcode: 0xCB87
	TEST(CPUBitResetTests, RES_0_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x87 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 0);
	}

	// RES 1, B
	// Opcode: 0xCB88
	TEST(CPUBitResetTests, RES_1_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x88 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 1);
	}

	// RES 1, C
	// Opcode: 0xCB89
	TEST(CPUBitResetTests, RES_1_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x89 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 1);
	}

	// RES 1, D
	// Opcode: 0xCB8A
	TEST(CPUBitResetTests, RES_1_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8A });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 1);
	}

	// RES 1, E
	// Opcode: 0xCB8B
	TEST(CPUBitResetTests, RES_1_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8B });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 1);
	}

	// RES 1, H
	// Opcode: 0xCB8C
	TEST(CPUBitResetTests, RES_1_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8C });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 1);
	}

	// RES 1, L
	// Opcode: 0xCB8D
	TEST(CPUBitResetTests, RES_1_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8D });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 1);
	}

	// RES 1, (HL)
	// Opcode: 0xCB8E
	TEST(CPUBitResetTests, RES_1_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8E });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 1);
	}

	// RES 1, A
	// Opcode: 0xCB8F
	TEST(CPUBitResetTests, RES_1_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x8F });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 1);
	}

	// RES 2, B
	// Opcode: 0xCB90
	TEST(CPUBitResetTests, RES_2_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x90 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 2);
	}

	// RES 2, C
	// Opcode: 0xCB91
	TEST(CPUBitResetTests, RES_2_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x91 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 2);
	}

	// RES 2, D
	// Opcode: 0xCB92
	TEST(CPUBitResetTests, RES_2_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x92 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 2);
	}

	// RES 2, E
	// Opcode: 0xCB93
	TEST(CPUBitResetTests, RES_2_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x93 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 2);
	}

	// RES 2, H
	// Opcode: 0xCB94
	TEST(CPUBitResetTests, RES_2_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x94 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 2);
	}

	// RES 2, L
	// Opcode: 0xCB95
	TEST(CPUBitResetTests, RES_2_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x95 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 2);
	}

	// RES 2, (HL)
	// Opcode: 0xCB96
	TEST(CPUBitResetTests, RES_2_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x96 });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 2);
	}

	// RES 2, A
	// Opcode: 0xCB97
	TEST(CPUBitResetTests, RES_2_F)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x97 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 2);
	}

	// RES 3, B
	// Opcode: 0xCB98
	TEST(CPUBitResetTests, RES_3_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x98 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 3);
	}

	// RES 3, C
	// Opcode: 0xCB99
	TEST(CPUBitResetTests, RES_3_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x99 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 3);
	}

	// RES 3, D
	// Opcode: 0xCB9A
	TEST(CPUBitResetTests, RES_3_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9A });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 3);
	}

	// RES 3, E
	// Opcode: 0xCB9B
	TEST(CPUBitResetTests, RES_3_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9B });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 3);
	}

	// RES 3, H
	// Opcode: 0xCB9C
	TEST(CPUBitResetTests, RES_3_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9C });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 3);
	}

	// RES 3, L
	// Opcode: 0xCB9D
	TEST(CPUBitResetTests, RES_3_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9D });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 3);
	}

	// RES 3, (HL)
	// Opcode: 0xCB9E
	TEST(CPUBitResetTests, RES_3_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9E });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 3);
	}

	// RES 3, A
	// Opcode: 0xCB9F
	TEST(CPUBitResetTests, RES_3_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x9F });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 3);
	}

	// RES 4, B
	// Opcode: 0xCBA0
	TEST(CPUBitResetTests, RES_4_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA0 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 4);
	}

	// RES 4, C
	// Opcode: 0xCBA1
	TEST(CPUBitResetTests, RES_4_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA1 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 4);
	}

	// RES 4, D
	// Opcode: 0xCBA2
	TEST(CPUBitResetTests, RES_4_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA2 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 4);
	}

	// RES 4, E
	// Opcode: 0xCBA3
	TEST(CPUBitResetTests, RES_4_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA3 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 4);
	}

	// RES 4, H
	// Opcode: 0xCBA4
	TEST(CPUBitResetTests, RES_4_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA4 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 4);
	}

	// RES 4, L
	// Opcode: 0xCBA5
	TEST(CPUBitResetTests, RES_4_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA5 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 4);
	}

	// RES 4, (HL)
	// Opcode: 0xCBA6
	TEST(CPUBitResetTests, RES_4_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA6 });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 4);
	}

	// RES 4, A
	// Opcode: 0xCBA7
	TEST(CPUBitResetTests, RES_4_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA7 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 4);
	}

	// RES 5, B
	// Opcode: 0xCBA8
	TEST(CPUBitResetTests, RES_5_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA8 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 5);
	}

	// RES 5, C
	// Opcode: 0xCBA9
	TEST(CPUBitResetTests, RES_5_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xA9 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 5);
	}

	// RES 5, D
	// Opcode: 0xCBAA
	TEST(CPUBitResetTests, RES_5_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAA });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 5);
	}

	// RES 5, E
	// Opcode: 0xCBAB
	TEST(CPUBitResetTests, RES_5_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAB });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 5);
	}

	// RES 5, H
	// Opcode: 0xCBAC
	TEST(CPUBitResetTests, RES_5_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAC });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 5);
	}

	// RES 5, L
	// Opcode: 0xCBAD
	TEST(CPUBitResetTests, RES_5_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAD });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 5);
	}

	// RES 5, (HL)
	// Opcode: 0xCBAE
	TEST(CPUBitResetTests, RES_5_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAE });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 5);
	}

	// RES 5, A
	// Opcode: 0xCBAF
	TEST(CPUBitResetTests, RES_5_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xAF });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 5);
	}

	// RES 6, B
	// Opcode: 0xCBB0
	TEST(CPUBitResetTests, RES_6_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB0 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 6);
	}

	// RES 6, C
	// Opcode: 0xCBB1
	TEST(CPUBitResetTests, RES_6_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB1 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 6);
	}

	// RES 6, D
	// Opcode: 0xCBB2
	TEST(CPUBitResetTests, RES_6_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB2 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 6);
	}

	// RES 6, E
	// Opcode: 0xCBB3
	TEST(CPUBitResetTests, RES_6_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB3 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 6);
	}

	// RES 6, H
	// Opcode: 0xCBB4
	TEST(CPUBitResetTests, RES_6_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB4 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 6);
	}

	// RES 6, L
	// Opcode: 0xCBB5
	TEST(CPUBitResetTests, RES_6_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB5 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 6);
	}

	// RES 6, (HL)
	// Opcode: 0xCBB6
	TEST(CPUBitResetTests, RES_6_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB6 });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 6);
	}

	// RES 6, A
	// Opcode: 0xCBB7
	TEST(CPUBitResetTests, RES_6_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB7 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 6);
	}

	// RES 7, B
	// Opcode: 0xCBB8
	TEST(CPUBitResetTests, RES_7_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB8 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterB(), 7);
	}

	// RES 7, C
	// Opcode: 0xCBB9
	TEST(CPUBitResetTests, RES_7_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xB9 });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterC(), 7);
	}

	// RES 7, D
	// Opcode: 0xCBBA
	TEST(CPUBitResetTests, RES_7_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBA });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterD(), 7);
	}

	// RES 7, E
	// Opcode: 0xCBBB
	TEST(CPUBitResetTests, RES_7_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBB });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterE(), 7);
	}

	// RES 7, H
	// Opcode: 0xCBBC
	TEST(CPUBitResetTests, RES_7_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBC });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterH(), 7);
	}

	// RES 7, L
	// Opcode: 0xCBBD
	TEST(CPUBitResetTests, RES_7_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBD });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterL(), 7);
	}

	// RES 7, (HL)
	// Opcode: 0xCBBE
	TEST(CPUBitResetTests, RES_7_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBE });
		auto processor = CPU(memory);

		Test8BitBitResetAtAddress(processor, memory, processor.GetRegisterHL(), 7);
	}

	// RES 7, A
	// Opcode: 0xCBBF
	TEST(CPUBitResetTests, RES_7_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0xBF });
		auto processor = CPU(memory);

		Test8BitBitReset(processor, processor.GetRegisterA(), 7);
	}
}