#include "CPU/CPU.hpp"
#include "gtest/gtest.h"
#include "CPUTestGlobals.hpp"
#include "Logger.hpp"

namespace SHG
{
	void Test8BitAddition(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2);
	}

	void Test16BitAddition(CPU& processor, Register16& targetRegister)
	{
		uint16_t operand1 = 300;
		uint16_t operand2 = 500;

		processor.GetRegisterHL().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), operand1 + operand2);
	}

	void Test8BitSubtraction(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2);
	}

	void Test8BitAND(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 & operand2);
	}

	void Test8BitXOR(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 ^ operand2);
	}

	void Test8BitOR(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 | operand2);
	}

	void Test8BitAddWithCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2 + carryFlag);
	}

	void Test8BitSubtractWithCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 8;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2 - carryFlag);
	}

	void Test8BitIncrement(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		EXPECT_EQ(targetRegister.GetData(), operand + 1);
	}

	void Test16BitIncrement(CPU& processor, Register16& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		EXPECT_EQ(targetRegister.GetData(), operand + 1);
	}

	void Test8BitDecrement(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		EXPECT_EQ(targetRegister.GetData(), operand - 1);
	}

	void Test16BitDecrement(CPU& processor, Register16& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		EXPECT_EQ(targetRegister.GetData(), operand - 1);
	}

	void Test8BitCompare(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 5;
		uint8_t operand2 = 5;

		processor.GetRegisterA().SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	// INC BC
	// Opcode: 0x03
	TEST(CPUArithmeticAndLogic, INC_BC)
	{
		Memory memory = CreatePresetMemory(0x03);
		auto processor = CPU(memory);

		Test16BitIncrement(processor, processor.GetRegisterBC());
	}

	// INC DE
	// Opcode: 0x13
	TEST(CPUArithmeticAndLogic, INC_DE)
	{
		Memory memory = CreatePresetMemory(0x13);
		auto processor = CPU(memory);

		Test16BitIncrement(processor, processor.GetRegisterDE());
	}

	// INC HL
	// Opcode: 0x23
	TEST(CPUArithmeticAndLogic, INC_HL)
	{
		Memory memory = CreatePresetMemory(0x23);
		auto processor = CPU(memory);

		Test16BitIncrement(processor, processor.GetRegisterHL());
	}

	// INC SP
	// Opcode: 0x33
	TEST(CPUArithmeticAndLogic, INC_SP)
	{
		Memory memory = CreatePresetMemory(0x33);
		auto processor = CPU(memory);

		Test16BitIncrement(processor, processor.GetStackPointer());
	}

	// INC B
	// Opcode: 0x04
	TEST(CPUArithmeticAndLogic, INC_B)
	{
		Memory memory = CreatePresetMemory(0x04);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterB());
	}

	// INC D
	// Opcode: 0x14
	TEST(CPUArithmeticAndLogic, INC_D)
	{
		Memory memory = CreatePresetMemory(0x14);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterD());
	}

	// INC H
	// Opcode: 0x24
	TEST(CPUArithmeticAndLogic, INC_H)
	{
		Memory memory = CreatePresetMemory(0x24);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterH());
	}

	// INC (HL)
	// Opcode: 0x34
	TEST(CPUArithmeticAndLogic, INC_ADDR_HL)
	{
		uint8_t value = 5;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x34);
		auto processor = CPU(memory);

		memory.SetByte(address, value);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value + 1);
	}

	// DEC B
	// Opcode: 0x05
	TEST(CPUArithmeticAndLogic, DEC_B)
	{
		Memory memory = CreatePresetMemory(0x05);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterB());
	}

	// DEC D
	// Opcode: 0x15
	TEST(CPUArithmeticAndLogic, DEC_D)
	{
		Memory memory = CreatePresetMemory(0x15);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterD());
	}

	// DEC H
	// Opcode: 0x25
	TEST(CPUArithmeticAndLogic, DEC_H)
	{
		Memory memory = CreatePresetMemory(0x25);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterH());
	}

	// DEC (HL)
	// Opcode: 0x35
	TEST(CPUArithmeticAndLogic, DEC_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x35);
		auto processor = CPU(memory);

		memory.SetByte(address, value);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value - 1);
	}

	// DAA
	// Opcode: 0x27
	TEST(CPUArithmeticAndLogic, DAA)
	{
		uint8_t operand1 = 0x24;
		uint8_t operand2 = 0x57;
		uint8_t expectedResult = 0x81;

		Memory memory = CreatePresetMemory(0x27);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1 + operand2);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), expectedResult);
	}

	// SCF
	// Opcode: 0x37
	TEST(CPUArithmeticAndLogic, SCF)
	{
		Memory memory = CreatePresetMemory(0x37);
		auto processor = CPU(memory);

		processor.ChangeCarryFlag(false);
		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 1);
	}

	// ADD HL, BC
	// Opcode: 0x09
	TEST(CPUArithmeticAndLogic, ADD_HL_BC)
	{
		Memory memory = CreatePresetMemory(0x09);
		auto processor = CPU(memory);

		Test16BitAddition(processor, processor.GetRegisterBC());
	}

	// ADD HL, DE
	// Opcode: 0x19
	TEST(CPUArithmeticAndLogic, ADD_HL_DE)
	{
		Memory memory = CreatePresetMemory(0x19);
		auto processor = CPU(memory);

		Test16BitAddition(processor, processor.GetRegisterDE());
	}

	// ADD HL, HL
	// Opcode: 0x29
	TEST(CPUArithmeticAndLogic, ADD_HL_HL)
	{
		uint16_t value = 300;

		Memory memory = CreatePresetMemory(0x29);
		auto processor = CPU(memory);

		processor.GetRegisterHL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), value * 2);
	}

	// ADD HL, SP
	// Opcode: 0x39
	TEST(CPUArithmeticAndLogic, ADD_HL_SP)
	{
		Memory memory = CreatePresetMemory(0x39);
		auto processor = CPU(memory);

		Test16BitAddition(processor, processor.GetStackPointer());
	}

	// DEC BC
	// Opcode: 0x0B
	TEST(CPUArithmeticAndLogic, DEC_BC)
	{
		Memory memory = CreatePresetMemory(0x0B);
		auto processor = CPU(memory);

		Test16BitDecrement(processor, processor.GetRegisterBC());
	}

	// DEC DE
	// Opcode: 0x1B
	TEST(CPUArithmeticAndLogic, DEC_DE)
	{
		Memory memory = CreatePresetMemory(0x1B);
		auto processor = CPU(memory);

		Test16BitDecrement(processor, processor.GetRegisterDE());
	}

	// DEC HL
	// Opcode: 0x2B
	TEST(CPUArithmeticAndLogic, DEC_HL)
	{
		Memory memory = CreatePresetMemory(0x2B);
		auto processor = CPU(memory);

		Test16BitDecrement(processor, processor.GetRegisterHL());
	}

	// DEC SP
	// Opcode: 0x3B
	TEST(CPUArithmeticAndLogic, DEC_SP)
	{
		Memory memory = CreatePresetMemory(0x3B);
		auto processor = CPU(memory);

		Test16BitDecrement(processor, processor.GetStackPointer());
	}

	// INC C
	// Opcode: 0x0C
	TEST(CPUArithmeticAndLogic, INC_C)
	{
		Memory memory = CreatePresetMemory(0x0C);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterC());
	}

	// INC E
	// Opcode: 0x1C
	TEST(CPUArithmeticAndLogic, INC_E)
	{
		Memory memory = CreatePresetMemory(0x1C);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterE());
	}

	// INC L
	// Opcode: 0x2C
	TEST(CPUArithmeticAndLogic, INC_L)
	{
		Memory memory = CreatePresetMemory(0x2C);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterL());
	}

	// INC A
	// Opcode: 0x3C
	TEST(CPUArithmeticAndLogic, INC_A)
	{
		Memory memory = CreatePresetMemory(0x3C);
		auto processor = CPU(memory);

		Test8BitIncrement(processor, processor.GetRegisterA());
	}

	// DEC C
	// Opcode: 0x0D
	TEST(CPUArithmeticAndLogic, DEC_C)
	{
		Memory memory = CreatePresetMemory(0x0D);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterC());
	}

	// DEC E
	// Opcode: 0x1D
	TEST(CPUArithmeticAndLogic, DEC_E)
	{
		Memory memory = CreatePresetMemory(0x1D);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterE());
	}

	// DEC L
	// Opcode: 0x2D
	TEST(CPUArithmeticAndLogic, DEC_L)
	{
		Memory memory = CreatePresetMemory(0x2D);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterL());
	}

	// DEC A
	// Opcode: 0x3D
	TEST(CPUArithmeticAndLogic, DEC_A)
	{
		Memory memory = CreatePresetMemory(0x3D);
		auto processor = CPU(memory);

		Test8BitDecrement(processor, processor.GetRegisterA());
	}

	// CPL
	// Opcode: 0x2F
	TEST(CPUArithmeticAndLogic, CPL)
	{
		uint8_t operand = 5;

		Memory memory = CreatePresetMemory(0x2F);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand ^ 0x00FF);
	}

	// CCF
	// Opcode: 0x3F
	TEST(CPUArithmeticAndLogic, CCF)
	{
		uint8_t opcode = 0x3F;

		Memory memory = CreatePresetMemory(opcode);
		auto processor = CPU(memory);
		memory.SetByte(1, opcode);

		processor.ChangeCarryFlag(false);
		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 1);

		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 0);
	}

	// ADD A, B
	// Opcode: 0x80
	TEST(CPUArithmeticAndLogic, ADD_A_B)
	{
		Memory memory = CreatePresetMemory(0x80);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterB());
	}

	// ADD A, C
	// Opcode: 0x81
	TEST(CPUArithmeticAndLogic, ADD_A_C)
	{
		Memory memory = CreatePresetMemory(0x81);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterC());
	}

	// ADD A, D
	// Opcode: 0x82
	TEST(CPUArithmeticAndLogic, ADD_A_D)
	{
		Memory memory = CreatePresetMemory(0x82);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterD());
	}

	// ADD A, E
	// Opcode: 0x83
	TEST(CPUArithmeticAndLogic, ADD_A_E)
	{
		Memory memory = CreatePresetMemory(0x83);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterE());
	}

	// ADD A, H
	// Opcode: 0x84
	TEST(CPUArithmeticAndLogic, ADD_A_H)
	{
		Memory memory = CreatePresetMemory(0x84);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterH());
	}

	// ADD A, L
	// Opcode: 0x85
	TEST(CPUArithmeticAndLogic, ADD_A_L)
	{
		Memory memory = CreatePresetMemory(0x85);
		auto processor = CPU(memory);

		Test8BitAddition(processor, processor.GetRegisterL());
	}

	// ADD A, (HL)
	// Opcode: 0x86
	TEST(CPUArithmeticAndLogic, ADD_A_ADDR_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		Memory memory = CreatePresetMemory(0x86);
		memory.SetByte(address, operand2);

		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2);
	}

	// ADD A, A
	// Opcode: 0x87
	TEST(CPUArithmeticAndLogic, ADD_A_A)
	{
		uint8_t value = 10;

		Memory memory = CreatePresetMemory(0x87);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value * 2);
	}

	// ADC A, B
	// Opcode: 0x88
	TEST(CPUArithmeticAndLogic, ADC_A_B)
	{
		Memory memory = CreatePresetMemory(0x88);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterB());
	}

	// ADC A, C
	// Opcode: 0x89
	TEST(CPUArithmeticAndLogic, ADC_A_C)
	{
		Memory memory = CreatePresetMemory(0x89);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterC());
	}

	// ADC A, D
	// Opcode: 0x8A
	TEST(CPUArithmeticAndLogic, ADC_A_D)
	{
		Memory memory = CreatePresetMemory(0x8A);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterD());
	}

	// ADC A, E
	// Opcode: 0x8B
	TEST(CPUArithmeticAndLogic, ADC_A_E)
	{
		Memory memory = CreatePresetMemory(0x8B);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterE());
	}

	// ADC A, H
	// Opcode: 0x8C
	TEST(CPUArithmeticAndLogic, ADC_A_H)
	{
		Memory memory = CreatePresetMemory(0x8C);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterH());
	}

	// ADC A, L
	// Opcode: 0x8D
	TEST(CPUArithmeticAndLogic, ADC_A_L)
	{
		Memory memory = CreatePresetMemory(0x8D);
		auto processor = CPU(memory);

		Test8BitAddWithCarry(processor, processor.GetRegisterL());
	}

	// ADC A, (HL)
	// Opcode: 0x8E
	TEST(CPUArithmeticAndLogic, ADC_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0x8E);
		auto processor = CPU(memory);

		memory.SetByte(address, operand2);

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2 + carryFlag);
	}

	// ADC A, A
	// Opcode: 0x8F
	TEST(CPUArithmeticAndLogic, ADC_A_A)
	{
		uint8_t operand1 = 2;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0x8F);
		auto processor = CPU(memory);

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand1 + carryFlag);
	}

	// SUB A, B
	// Opcode: 0x90
	TEST(CPUArithmeticAndLogic, SUB_A_B)
	{
		Memory memory = CreatePresetMemory(0x90);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterB());
	}

	// SUB A, C
	// Opcode: 0x91
	TEST(CPUArithmeticAndLogic, SUB_A_C)
	{
		Memory memory = CreatePresetMemory(0x91);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterC());
	}

	// SUB A, D
	// Opcode: 0x92
	TEST(CPUArithmeticAndLogic, SUB_A_D)
	{
		Memory memory = CreatePresetMemory(0x92);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterD());
	}

	// SUB A, E
	// Opcode: 0x93
	TEST(CPUArithmeticAndLogic, SUB_A_E)
	{
		Memory memory = CreatePresetMemory(0x93);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterE());
	}

	// SUB A, H
	// Opcode: 0x94
	TEST(CPUArithmeticAndLogic, SUB_A_H)
	{
		Memory memory = CreatePresetMemory(0x94);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterH());
	}

	// SUB A, L
	// Opcode: 0x95
	TEST(CPUArithmeticAndLogic, SUB_A_L)
	{
		Memory memory = CreatePresetMemory(0x95);
		auto processor = CPU(memory);

		Test8BitSubtraction(processor, processor.GetRegisterL());
	}

	// SUB A, (HL)
	// Opcode: 0x96
	TEST(CPUArithmeticAndLogic, SUB_A_HL)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		Memory memory = CreatePresetMemory(0x96);
		memory.SetByte(address, operand2);

		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2);
	}

	// SUB A, A
	// Opcode: 0x97
	TEST(CPUArithmeticAndLogic, SUB_A_A)
	{
		uint8_t operand = 2;

		Memory memory = CreatePresetMemory(0x97);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), 0);
	}

	// SBC A, B
	// Opcode: 0x98
	TEST(CPUArithmeticAndLogic, SBC_A_B)
	{
		Memory memory = CreatePresetMemory(0x98);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterB());
	}

	// SBC A, C
	// Opcode: 0x99
	TEST(CPUArithmeticAndLogic, SBC_A_C)
	{
		Memory memory = CreatePresetMemory(0x99);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterC());
	}

	// SBC A, D
	// Opcode: 0x9A
	TEST(CPUArithmeticAndLogic, SBC_A_D)
	{
		Memory memory = CreatePresetMemory(0x9A);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterD());
	}

	// SBC A, E
	// Opcode: 0x9B
	TEST(CPUArithmeticAndLogic, SBC_A_E)
	{
		Memory memory = CreatePresetMemory(0x9B);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterE());
	}

	// SBC A, H
	// Opcode: 0x9C
	TEST(CPUArithmeticAndLogic, SBC_A_H)
	{
		Memory memory = CreatePresetMemory(0x9C);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterH());
	}

	// SBC A, L
	// Opcode: 0x9D
	TEST(CPUArithmeticAndLogic, SBC_A_L)
	{
		Memory memory = CreatePresetMemory(0x9D);
		auto processor = CPU(memory);

		Test8BitSubtractWithCarry(processor, processor.GetRegisterL());
	}

	// SBC A, (HL)
	// Opcode: 0x9E
	TEST(CPUArithmeticAndLogic, SBC_A_ADDR_HL)
	{
		uint8_t operand1 = 6;
		uint8_t operand2 = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0x9E);
		auto processor = CPU(memory);

		memory.SetByte(address, operand2);

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2 - carryFlag);
	}

	// SBC A, A
	// Opcode: 0x9F
	TEST(CPUArithmeticAndLogic, SBC_A_A)
	{
		uint8_t operand1 = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0x9F);
		auto processor = CPU(memory);

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		// TODO: Re-assess this
		EXPECT_EQ(processor.GetRegisterA().GetData(), (uint8_t)-1);
	}

	// AND A, B
	// Opcode: 0xA0
	TEST(CPUArithmeticAndLogic, AND_A_B)
	{
		Memory memory = CreatePresetMemory(0xA0);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterB());
	}

	// AND A, C
	// Opcode: 0xA1
	TEST(CPUArithmeticAndLogic, AND_A_C)
	{
		Memory memory = CreatePresetMemory(0xA1);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterC());
	}

	// AND A, D
	// Opcode: 0xA2
	TEST(CPUArithmeticAndLogic, AND_A_D)
	{
		Memory memory = CreatePresetMemory(0xA2);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterD());
	}

	// AND A, E
	// Opcode: 0xA3
	TEST(CPUArithmeticAndLogic, AND_A_E)
	{
		Memory memory = CreatePresetMemory(0xA3);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterE());
	}

	// AND A, H
	// Opcode: 0xA4
	TEST(CPUArithmeticAndLogic, AND_A_H)
	{
		Memory memory = CreatePresetMemory(0xA4);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterH());
	}

	// AND A, L
	// Opcode: 0xA5
	TEST(CPUArithmeticAndLogic, AND_A_L)
	{
		Memory memory = CreatePresetMemory(0xA5);
		auto processor = CPU(memory);

		Test8BitAND(processor, processor.GetRegisterL());
	}

	// AND A, (HL)
	// Opcode: 0xA6
	TEST(CPUArithmeticAndLogic, AND_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		Memory memory = CreatePresetMemory(0xA6);
		memory.SetByte(address, operand2);

		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 & operand2);
	}

	// AND A, A
	// Opcode: 0xA7
	TEST(CPUArithmeticAndLogic, AND_A_A)
	{
		uint8_t operand = 7;

		Memory memory = CreatePresetMemory(0xA7);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand & operand);
	}

	// XOR A, B
	// Opcode: 0xA8
	TEST(CPUArithmeticAndLogic, XOR_A_B)
	{
		Memory memory = CreatePresetMemory(0xA8);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterB());
	}

	// XOR A, C
	// Opcode: 0xA9
	TEST(CPUArithmeticAndLogic, XOR_A_C)
	{
		Memory memory = CreatePresetMemory(0xA9);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterC());
	}

	// XOR A, D
	// Opcode: 0xAA
	TEST(CPUArithmeticAndLogic, XOR_A_D)
	{
		Memory memory = CreatePresetMemory(0xAA);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterD());
	}

	// XOR A, E
	// Opcode: 0xAB
	TEST(CPUArithmeticAndLogic, XOR_A_E)
	{
		Memory memory = CreatePresetMemory(0xAB);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterE());
	}

	// XOR A, H
	// Opcode: 0xAC
	TEST(CPUArithmeticAndLogic, XOR_A_H)
	{
		Memory memory = CreatePresetMemory(0xAC);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterH());
	}

	// XOR A, L
	// Opcode: 0xAD
	TEST(CPUArithmeticAndLogic, XOR_A_L)
	{
		Memory memory = CreatePresetMemory(0xAD);
		auto processor = CPU(memory);

		Test8BitXOR(processor, processor.GetRegisterL());
	}

	// XOR A, (HL)
	// Opcode: 0xAE
	TEST(CPUArithmeticAndLogic, XOR_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		Memory memory = CreatePresetMemory(0xAE);
		memory.SetByte(address, operand2);

		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 ^ operand2);
	}

	// XOR A, A
	// Opcode: 0xAF
	TEST(CPUArithmeticAndLogic, XOR_A_A)
	{
		uint8_t operand = 7;

		Memory memory = CreatePresetMemory(0xAF);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand ^ operand);
	}

	// OR A, B
	// Opcode: 0xB0
	TEST(CPUArithmeticAndLogic, OR_A_B)
	{
		Memory memory = CreatePresetMemory(0xB0);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterB());
	}

	// OR A, C
	// Opcode: 0xB1
	TEST(CPUArithmeticAndLogic, OR_A_C)
	{
		Memory memory = CreatePresetMemory(0xB1);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterC());
	}

	// OR A, D
	// Opcode: 0xB2
	TEST(CPUArithmeticAndLogic, OR_A_D)
	{
		Memory memory = CreatePresetMemory(0xB2);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterD());
	}

	// OR A, E
	// Opcode: 0xB3
	TEST(CPUArithmeticAndLogic, OR_A_E)
	{
		Memory memory = CreatePresetMemory(0xB3);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterE());
	}

	// OR A, H
	// Opcode: 0xB4
	TEST(CPUArithmeticAndLogic, OR_A_H)
	{
		Memory memory = CreatePresetMemory(0xB4);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterH());
	}

	// OR A, L
	// Opcode: 0xB5
	TEST(CPUArithmeticAndLogic, OR_A_L)
	{
		Memory memory = CreatePresetMemory(0xB5);
		auto processor = CPU(memory);

		Test8BitOR(processor, processor.GetRegisterL());
	}

	// OR A, (HL)
	// Opcode: 0xB6
	TEST(CPUArithmeticAndLogic, OR_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		Memory memory = CreatePresetMemory(0xB6);
		memory.SetByte(address, operand2);

		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 | operand2);
	}

	// OR A, A
	// Opcode: 0xB7
	TEST(CPUArithmeticAndLogic, OR_A_A)
	{
		uint8_t operand = 7;

		Memory memory = CreatePresetMemory(0xB7);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand | operand);
	}

	// CP A, B
	// Opcode: 0xB8
	TEST(CPUArithmeticAndLogic, CP_A_B)
	{
		Memory memory = CreatePresetMemory(0xB8);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterB());
	}

	// CP A, C
	// Opcode: 0xB9
	TEST(CPUArithmeticAndLogic, CP_A_C)
	{
		Memory memory = CreatePresetMemory(0xB9);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterC());
	}

	// CP A, D
	// Opcode: 0xBA
	TEST(CPUArithmeticAndLogic, CP_A_D)
	{
		Memory memory = CreatePresetMemory(0xBA);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterD());
	}

	// CP A, E
	// Opcode: 0xBB
	TEST(CPUArithmeticAndLogic, CP_A_E)
	{
		Memory memory = CreatePresetMemory(0xBB);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterE());
	}

	// CP A, H
	// Opcode: 0xBC
	TEST(CPUArithmeticAndLogic, CP_A_H)
	{
		Memory memory = CreatePresetMemory(0xBC);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterH());
	}

	// CP A, L
	// Opcode: 0xBD
	TEST(CPUArithmeticAndLogic, CP_A_L)
	{
		Memory memory = CreatePresetMemory(0xBD);
		auto processor = CPU(memory);

		Test8BitCompare(processor, processor.GetRegisterL());
	}

	// CP A, (HL)
	// Opcode: 0xBE
	TEST(CPUArithmeticAndLogic, CP_A_ADDR_HL)
	{

		uint8_t operand1 = 7;
		uint8_t operand2 = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0xBE);
		auto processor = CPU(memory);
		memory.SetByte(address, operand2);

		processor.GetRegisterA().SetData(operand1);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	// CP A, A
	// Opcode: 0xBF
	TEST(CPUArithmeticAndLogic, CP_A_A)
	{
		Memory memory = CreatePresetMemory(0xBF);
		auto processor = CPU(memory);

		uint8_t operand = 5;

		processor.GetRegisterA().SetData(operand);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	// ADD A, U8
	// Opcode: 0xC6
	TEST(CPUArithmeticAndLogic, ADD_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		Memory memory = CreatePresetMemory(0xC6);
		auto processor = CPU(memory);

		memory.SetByte(1, operand2);
		processor.GetRegisterA().SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2);
	}

	// SUB A, U8
	// Opcode: 0xD6
	TEST(CPUArithmeticAndLogic, SUB_A_U8)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;

		Memory memory = CreatePresetMemory(0xD6);
		auto processor = CPU(memory);

		memory.SetByte(1, operand2);

		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2);
	}

	// AND A, U8
	// Opcode: 0xE6
	TEST(CPUArithmeticAndLogic, AND_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		Memory memory = CreatePresetMemory(0xE6);
		auto processor = CPU(memory);

		processor.GetRegisterA().SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 & operand2);
	}

	// OR A, U8
	// Opcode: 0xF6
	TEST(CPUArithmeticAndLogic, OR_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		Memory memory = CreatePresetMemory(0xF6);
		auto processor = CPU(memory);

		memory.SetByte(1, operand2);
		processor.GetRegisterA().SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 | operand2);
	}

	// ADD SP, I8
	// Opcode: 0xE8
	TEST(CPUArithmeticAndLogic, ADD_SP_I8)
	{
		uint16_t operand1 = 300;
		int8_t operand2 = -3;

		Memory memory = CreatePresetMemory(0xE8);
		auto processor = CPU(memory);
		memory.SetByte(1, operand2);

		processor.GetStackPointer().SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetStackPointer().GetData(), operand1 + operand2);
	}

	// LD HL, SP + I8
	// Opcode: 0xF8
	TEST(CPUArithmeticAndLogic, LD_HL_SPI8)
	{
		uint16_t stackPointerValue = 320;
		int8_t signedIntValue = -5;

		Memory memory = CreatePresetMemory(0xF8);
		auto processor = CPU(memory);

		memory.SetByte(1, signedIntValue);
		processor.GetStackPointer().SetData(stackPointerValue);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), stackPointerValue + signedIntValue);
	}

	// ADC A, U8
	// Opcode: 0xCE
	TEST(CPUArithmeticAndLogic, ADC_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0xCE);
		auto processor = CPU(memory);
		memory.SetByte(1, operand2);

		processor.ChangeCarryFlag((bool)carryFlag);
		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 + operand2 + carryFlag);
	}

	// SBC A, U8
	// Opcode: 0xDE
	TEST(CPUArithmeticAndLogic, SBC_A_U8)
	{
		uint8_t operand1 = 5;
		uint8_t operand2 = 2;
		uint8_t carryFlag = 1;

		Memory memory = CreatePresetMemory(0xDE);
		auto processor = CPU(memory);

		memory.SetByte(1, operand2);
		processor.ChangeCarryFlag(carryFlag);
		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 - operand2 - carryFlag);
	}

	// XOR A, U8
	// Opcode: 0xEE
	TEST(CPUArithmeticAndLogic, XOR_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		Memory memory = CreatePresetMemory(0xEE);
		auto processor = CPU(memory);

		memory.SetByte(1, operand2);

		processor.GetRegisterA().SetData(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), operand1 ^ operand2);
	}

	// CP A, U8
	// Opcode: 0xFE
	TEST(CPUArithmeticAndLogic, CP_A_U8)
	{
		uint8_t operand1 = 9;
		uint8_t operand2 = 9;

		Memory memory = CreatePresetMemory(0xFE);
		auto processor = CPU(memory);
		memory.SetByte(1, operand2);

		processor.GetRegisterA().SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}
}