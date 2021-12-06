#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void Test8BitBitTest(CPU& processor, Register8& targetRegister, uint8_t bitNumber)
	{
		uint8_t zeroFlag = 1;
		uint8_t data = ~(1 << bitNumber);

		targetRegister.Write(data);
		processor.Step();

		EXPECT_EQ(processor.GetZeroFlag(), zeroFlag);
	}

	void Test8BitBitTestAtAddress(CPU& processor, Memory& memory, Register16& addressRegister, uint8_t bitNumber)
	{
		uint16_t address = 300;
		uint8_t zeroFlag = 1;
		uint8_t data = ~(1 << bitNumber);

		memory.Write(address, data);
		addressRegister.Write(address);
		processor.Step();

		EXPECT_EQ(processor.GetZeroFlag(), zeroFlag);
	}

	// BIT 0, B
	// Opcode: 0xCB40
	TEST(CPUBitTestInstructions, BIT_0_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x40 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 0);
	}

	// BIT 0, C
	// Opcode: 0xCB41
	TEST(CPUBitTestInstructions, BIT_0_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x41 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 0);
	}

	// BIT 0, D
	// Opcode: 0xCB42
	TEST(CPUBitTestInstructions, BIT_0_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x42 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 0);
	}

	// BIT 0, E
	// Opcode: 0xCB43
	TEST(CPUBitTestInstructions, BIT_0_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x43 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 0);
	}

	// BIT 0, H
	// Opcode: 0xCB44
	TEST(CPUBitTestInstructions, BIT_0_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x44 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 0);
	}

	// BIT 0, L
	// Opcode: 0xCB45
	TEST(CPUBitTestInstructions, BIT_0_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x45 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 0);
	}

	// BIT 0, (HL)
	// Opcode: 0xCB46
	TEST(CPUBitTestInstructions, BIT_0_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x46 });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 0);
	}

	// BIT 0, A
	// Opcode: 0xCB47
	TEST(CPUBitTestInstructions, BIT_0_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x47 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 0);
	}

	// BIT 1, B
	// Opcode: 0xCB48
	TEST(CPUBitTestInstructions, BIT_1_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x48 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 1);
	}

	// BIT 1, C
	// Opcode: 0xCB49
	TEST(CPUBitTestInstructions, BIT_1_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x49 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 1);
	}

	// BIT 1, D
	// Opcode: 0xCB4A
	TEST(CPUBitTestInstructions, BIT_1_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4A });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 1);
	}

	// BIT 1, E
	// Opcode: 0xCB4B
	TEST(CPUBitTestInstructions, BIT_1_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4B });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 1);
	}

	// BIT 1, H
	// Opcode: 0xCB4C
	TEST(CPUBitTestInstructions, BIT_1_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4C });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 1);
	}

	// BIT 1, L
	// Opcode: 0xCB4D
	TEST(CPUBitTestInstructions, BIT_1_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4D });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 1);
	}

	// BIT 1, (HL)
	// Opcode: 0xCB4E
	TEST(CPUBitTestInstructions, BIT_1_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4E });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 1);
	}

	// BIT 1, A
	// Opcode: 0xCB4F
	TEST(CPUBitTestInstructions, BIT_1_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x4F });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 1);
	}

	// BIT 2, B
	// Opcode: 0xCB50
	TEST(CPUBitTestInstructions, BIT_2_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x50 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 2);
	}

	// BIT 2, C
	// Opcode: 0xCB51
	TEST(CPUBitTestInstructions, BIT_2_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x51 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 2);
	}

	// BIT 2, D
	// Opcode: 0xCB52
	TEST(CPUBitTestInstructions, BIT_2_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x52 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 2);
	}

	// BIT 2, E
	// Opcode: 0xCB53
	TEST(CPUBitTestInstructions, BIT_2_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x53 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 2);
	}

	// BIT 2, H
	// Opcode: 0xCB54
	TEST(CPUBitTestInstructions, BIT_2_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x54 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 2);
	}

	// BIT 2, L
	// Opcode: 0xCB55
	TEST(CPUBitTestInstructions, BIT_2_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x55 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 2);
	}

	// BIT 2, (HL)
	// Opcode: 0xCB56
	TEST(CPUBitTestInstructions, BIT_2_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x56 });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 2);
	}

	// BIT 2, A
	// Opcode: 0xCB57
	TEST(CPUBitTestInstructions, BIT_2_F)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x57 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 2);
	}

	// BIT 3, B
	// Opcode: 0xCB58
	TEST(CPUBitTestInstructions, BIT_3_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x58 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 3);
	}

	// BIT 3, C
	// Opcode: 0xCB59
	TEST(CPUBitTestInstructions, BIT_3_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x59 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 3);
	}

	// BIT 3, D
	// Opcode: 0xCB5A
	TEST(CPUBitTestInstructions, BIT_3_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5A });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 3);
	}

	// BIT 3, E
	// Opcode: 0xCB5B
	TEST(CPUBitTestInstructions, BIT_3_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5B });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 3);
	}

	// BIT 3, H
	// Opcode: 0xCB5C
	TEST(CPUBitTestInstructions, BIT_3_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5C });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 3);
	}

	// BIT 3, L
	// Opcode: 0xCB5D
	TEST(CPUBitTestInstructions, BIT_3_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5D });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 3);
	}

	// BIT 3, (HL)
	// Opcode: 0xCB5E
	TEST(CPUBitTestInstructions, BIT_3_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5E });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 3);
	}

	// BIT 3, A
	// Opcode: 0xCB5F
	TEST(CPUBitTestInstructions, BIT_3_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x5F });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 3);
	}

	// BIT 4, B
	// Opcode: 0xCB60
	TEST(CPUBitTestInstructions, BIT_4_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x60 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 4);
	}

	// BIT 4, C
	// Opcode: 0xCB61
	TEST(CPUBitTestInstructions, BIT_4_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x61 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 4);
	}

	// BIT 4, D
	// Opcode: 0xCB62
	TEST(CPUBitTestInstructions, BIT_4_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x62 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 4);
	}

	// BIT 4, E
	// Opcode: 0xCB63
	TEST(CPUBitTestInstructions, BIT_4_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x63 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 4);
	}

	// BIT 4, H
	// Opcode: 0xCB64
	TEST(CPUBitTestInstructions, BIT_4_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x64 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 4);
	}

	// BIT 4, L
	// Opcode: 0xCB65
	TEST(CPUBitTestInstructions, BIT_4_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x65 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 4);
	}

	// BIT 4, (HL)
	// Opcode: 0xCB66
	TEST(CPUBitTestInstructions, BIT_4_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x66 });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 4);
	}

	// BIT 4, A
	// Opcode: 0xCB67
	TEST(CPUBitTestInstructions, BIT_4_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x67 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 4);
	}

	// BIT 5, B
	// Opcode: 0xCB68
	TEST(CPUBitTestInstructions, BIT_5_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x68 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 5);
	}

	// BIT 5, C
	// Opcode: 0xCB69
	TEST(CPUBitTestInstructions, BIT_5_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x69 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 5);
	}

	// BIT 5, D
	// Opcode: 0xCB6A
	TEST(CPUBitTestInstructions, BIT_5_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6A });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 5);
	}

	// BIT 5, E
	// Opcode: 0xCB6B
	TEST(CPUBitTestInstructions, BIT_5_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6B });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 5);
	}

	// BIT 5, H
	// Opcode: 0xCB6C
	TEST(CPUBitTestInstructions, BIT_5_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6C });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 5);
	}

	// BIT 5, L
	// Opcode: 0xCB6D
	TEST(CPUBitTestInstructions, BIT_5_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6D });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 5);
	}

	// BIT 5, (HL)
	// Opcode: 0xCB6E
	TEST(CPUBitTestInstructions, BIT_5_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6E });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 5);
	}

	// BIT 5, A
	// Opcode: 0xCB6F
	TEST(CPUBitTestInstructions, BIT_5_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x6F });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 5);
	}

	// BIT 6, B
	// Opcode: 0xCB70
	TEST(CPUBitTestInstructions, BIT_6_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x70 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 6);
	}

	// BIT 6, C
	// Opcode: 0xCB71
	TEST(CPUBitTestInstructions, BIT_6_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x71 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 6);
	}

	// BIT 6, D
	// Opcode: 0xCB72
	TEST(CPUBitTestInstructions, BIT_6_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x72 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 6);
	}

	// BIT 6, E
	// Opcode: 0xCB73
	TEST(CPUBitTestInstructions, BIT_6_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x73 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 6);
	}

	// BIT 6, H
	// Opcode: 0xCB74
	TEST(CPUBitTestInstructions, BIT_6_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x74 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 6);
	}

	// BIT 6, L
	// Opcode: 0xCB75
	TEST(CPUBitTestInstructions, BIT_6_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x75 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 6);
	}

	// BIT 6, (HL)
	// Opcode: 0xCB76
	TEST(CPUBitTestInstructions, BIT_6_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x76 });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 6);
	}

	// BIT 6, A
	// Opcode: 0xCB77
	TEST(CPUBitTestInstructions, BIT_6_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x77 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 6);
	}

	// BIT 7, B
	// Opcode: 0xCB78
	TEST(CPUBitTestInstructions, BIT_7_B)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x78 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterB(), 7);
	}

	// BIT 7, C
	// Opcode: 0xCB79
	TEST(CPUBitTestInstructions, BIT_7_C)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x79 });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterC(), 7);
	}

	// BIT 7, D
	// Opcode: 0xCB7A
	TEST(CPUBitTestInstructions, BIT_7_D)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7A });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterD(), 7);
	}

	// BIT 7, E
	// Opcode: 0xCB7B
	TEST(CPUBitTestInstructions, BIT_7_E)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7B });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterE(), 7);
	}

	// BIT 7, H
	// Opcode: 0xCB7C
	TEST(CPUBitTestInstructions, BIT_7_H)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7C });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterH(), 7);
	}

	// BIT 7, L
	// Opcode: 0xCB7D
	TEST(CPUBitTestInstructions, BIT_7_L)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7D });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterL(), 7);
	}

	// BIT 7, (HL)
	// Opcode: 0xCB7E
	TEST(CPUBitTestInstructions, BIT_7_ADDR_HL)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7E });
		auto processor = CPU(memory);

		Test8BitBitTestAtAddress(processor, memory, processor.GetRegisterHL(), 7);
	}

	// BIT 7, A
	// Opcode: 0xCB7F
	TEST(CPUBitTestInstructions, BIT_7_A)
	{
		BasicMemory memory = CreatePresetMemory({ 0xCB, 0x7F });
		auto processor = CPU(memory);

		Test8BitBitTest(processor, processor.GetRegisterA(), 7);
	}
}