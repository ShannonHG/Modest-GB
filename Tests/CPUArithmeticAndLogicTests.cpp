#include "CPU/CPU.hpp"
#include "gtest/gtest.h"
#include "CPUTestGlobals.hpp"
#include "Logger.hpp"

namespace SHG
{
	void Test8BitAddition(CPU& processor, Register8& targetRegister);
	void Test16BitAddition(CPU& processor, Register16& targetRegister);

	void Test8BitSubtraction(CPU& processor, Register8& targetRegister);
	void Test8BitAND(CPU& processor, Register8& targetRegister);
	void Test8BitXOR(CPU& processor, Register8& targetRegister);
	void Test8BitOR(CPU& processor, Register8& targetRegister);
	void Test8BitAddWithCarry(CPU& processor, Register8& targetRegister);
	void Test8BitSubtractWithCarry(CPU& processor, Register8& targetRegister);

	void Test8BitDecrement(CPU& processor, Register8& targetRegister);
	void Test16BitDecrement(CPU& processor, Register16& targetRegister);

	void Test8BitIncrement(CPU& processor, Register8& targetRegister);
	void Test16BitIncrement(CPU& processor, Register16& targetRegister);

	void Test8BitCompare(CPU& processor, Register8& targetRegister);

	// INC BC
	// Opcode: 0x03
	TEST(CPUArithmeticAndLogic, INC_BC)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x03);
		auto processor = CPU(mmu);

		Test16BitIncrement(processor, processor.GetRegister(CPURegisterID::BC));
	}

	// INC DE
	// Opcode: 0x13
	TEST(CPUArithmeticAndLogic, INC_DE)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x13);
		auto processor = CPU(mmu);

		Test16BitIncrement(processor, processor.GetRegister(CPURegisterID::DE));
	}

	// INC HL
	// Opcode: 0x23
	TEST(CPUArithmeticAndLogic, INC_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x23);
		auto processor = CPU(mmu);

		Test16BitIncrement(processor, processor.GetRegister(CPURegisterID::HL));
	}

	// INC SP
	// Opcode: 0x33
	TEST(CPUArithmeticAndLogic, INC_SP)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x33);
		auto processor = CPU(mmu);

		Test16BitIncrement(processor, processor.GetRegister(CPURegisterID::SP));
	}

	// INC B
	// Opcode: 0x04
	TEST(CPUArithmeticAndLogic, INC_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x04);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// INC D
	// Opcode: 0x14
	TEST(CPUArithmeticAndLogic, INC_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x14);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// INC H
	// Opcode: 0x24
	TEST(CPUArithmeticAndLogic, INC_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x24);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// INC (HL)
	// Opcode: 0x34
	TEST(CPUArithmeticAndLogic, INC_ADDR_HL)
	{
		uint8_t value = 5;
		uint16_t address = 300;

		auto mmu = CreatePresetMemoryManagementUnit(0x34);
		auto processor = CPU(mmu);

		mmu.SetByte(address, value);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), value + 1);
	}

	// DEC B
	// Opcode: 0x05
	TEST(CPUArithmeticAndLogic, DEC_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x05);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// DEC D
	// Opcode: 0x15
	TEST(CPUArithmeticAndLogic, DEC_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x15);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// DEC H
	// Opcode: 0x25
	TEST(CPUArithmeticAndLogic, DEC_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x25);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// DEC (HL)
	// Opcode: 0x35
	TEST(CPUArithmeticAndLogic, DEC_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		auto mmu = CreatePresetMemoryManagementUnit(0x35);
		auto processor = CPU(mmu);

		mmu.SetByte(address, value);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(mmu.GetByte(address), value - 1);
	}

	// DAA
	// Opcode: 0x27
	TEST(CPUArithmeticAndLogic, DAA)
	{
		uint8_t operand1 = 0x24;
		uint8_t operand2 = 0x57;
		uint8_t expectedResult = 0x81;

		auto mmu = CreatePresetMemoryManagementUnit(0x27);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1 + operand2);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), expectedResult);
	}

	// SCF
	// Opcode: 0x37
	TEST(CPUArithmeticAndLogic, SCF)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x37);
		auto processor = CPU(mmu);

		processor.SetCarryFlag(false);
		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 1);
	}

	// ADD HL, BC
	// Opcode: 0x09
	TEST(CPUArithmeticAndLogic, ADD_HL_BC)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x09);
		auto processor = CPU(mmu);

		Test16BitAddition(processor, processor.GetRegister(CPURegisterID::BC));
	}

	// ADD HL, DE
	// Opcode: 0x19
	TEST(CPUArithmeticAndLogic, ADD_HL_DE)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x19);
		auto processor = CPU(mmu);

		Test16BitAddition(processor, processor.GetRegister(CPURegisterID::DE));
	}

	// ADD HL, HL
	// Opcode: 0x29
	TEST(CPUArithmeticAndLogic, ADD_HL_HL)
	{
		uint16_t value = 300;

		auto mmu = CreatePresetMemoryManagementUnit(0x29);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value * 2);
	}

	// ADD HL, SP
	// Opcode: 0x39
	TEST(CPUArithmeticAndLogic, ADD_HL_SP)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x39);
		auto processor = CPU(mmu);

		Test16BitAddition(processor, processor.GetRegister(CPURegisterID::SP));
	}

	// DEC BC
	// Opcode: 0x0B
	TEST(CPUArithmeticAndLogic, DEC_BC)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x0B);
		auto processor = CPU(mmu);

		Test16BitDecrement(processor, processor.GetRegister(CPURegisterID::BC));
	}

	// DEC DE
	// Opcode: 0x1B
	TEST(CPUArithmeticAndLogic, DEC_DE)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x1B);
		auto processor = CPU(mmu);

		Test16BitDecrement(processor, processor.GetRegister(CPURegisterID::DE));
	}

	// DEC HL
	// Opcode: 0x2B
	TEST(CPUArithmeticAndLogic, DEC_HL)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x2B);
		auto processor = CPU(mmu);

		Test16BitDecrement(processor, processor.GetRegister(CPURegisterID::HL));
	}

	// DEC SP
	// Opcode: 0x3B
	TEST(CPUArithmeticAndLogic, DEC_SP)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x3B);
		auto processor = CPU(mmu);

		Test16BitDecrement(processor, processor.GetRegister(CPURegisterID::SP));
	}

	// INC C
	// Opcode: 0x0C
	TEST(CPUArithmeticAndLogic, INC_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x0C);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// INC E
	// Opcode: 0x1C
	TEST(CPUArithmeticAndLogic, INC_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x1C);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// INC L
	// Opcode: 0x2C
	TEST(CPUArithmeticAndLogic, INC_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x2C);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// INC A
	// Opcode: 0x3C
	TEST(CPUArithmeticAndLogic, INC_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x3C);
		auto processor = CPU(mmu);

		Test8BitIncrement(processor, processor.GetRegister(CPURegisterID::AF).GetHighRegister());
	}

	// DEC C
	// Opcode: 0x0D
	TEST(CPUArithmeticAndLogic, DEC_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x0D);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// DEC E
	// Opcode: 0x1D
	TEST(CPUArithmeticAndLogic, DEC_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x1D);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// DEC L
	// Opcode: 0x2D
	TEST(CPUArithmeticAndLogic, DEC_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x2D);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// DEC A
	// Opcode: 0x3D
	TEST(CPUArithmeticAndLogic, DEC_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x3D);
		auto processor = CPU(mmu);

		Test8BitDecrement(processor, processor.GetRegister(CPURegisterID::AF).GetHighRegister());
	}

	// CPL
	// Opcode: 0x2F
	TEST(CPUArithmeticAndLogic, CPL)
	{
		uint8_t operand = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0x2F);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand ^ 0x00FF);
	}

	// CCF
	// Opcode: 0x3F
	TEST(CPUArithmeticAndLogic, CCF)
	{
		uint8_t opcode = 0x3F;

		auto mmu = CreatePresetMemoryManagementUnit(opcode);
		auto processor = CPU(mmu);
		mmu.SetByte(1, opcode);

		processor.SetCarryFlag(false);
		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 1);

		processor.Cycle();

		EXPECT_EQ(processor.GetCarryFlag(), 0);
	}

	// ADD A, B
	// Opcode: 0x80
	TEST(CPUArithmeticAndLogic, ADD_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x80);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// ADD A, C
	// Opcode: 0x81
	TEST(CPUArithmeticAndLogic, ADD_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x81);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// ADD A, D
	// Opcode: 0x82
	TEST(CPUArithmeticAndLogic, ADD_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x82);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// ADD A, E
	// Opcode: 0x83
	TEST(CPUArithmeticAndLogic, ADD_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x83);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// ADD A, H
	// Opcode: 0x84
	TEST(CPUArithmeticAndLogic, ADD_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x84);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// ADD A, L
	// Opcode: 0x85
	TEST(CPUArithmeticAndLogic, ADD_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x85);
		auto processor = CPU(mmu);

		Test8BitAddition(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// ADD A, (HL)
	// Opcode: 0x86
	TEST(CPUArithmeticAndLogic, ADD_A_ADDR_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		auto mmu = CreatePresetMemoryManagementUnit(0x86);
		mmu.SetByte(address, operand2);

		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2);
	}

	// ADD A, A
	// Opcode: 0x87
	TEST(CPUArithmeticAndLogic, ADD_A_A)
	{
		uint8_t value = 10;

		auto mmu = CreatePresetMemoryManagementUnit(0x87);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), value * 2);
	}

	// ADC A, B
	// Opcode: 0x88
	TEST(CPUArithmeticAndLogic, ADC_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x88);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// ADC A, C
	// Opcode: 0x89
	TEST(CPUArithmeticAndLogic, ADC_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x89);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// ADC A, D
	// Opcode: 0x8A
	TEST(CPUArithmeticAndLogic, ADC_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x8A);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// ADC A, E
	// Opcode: 0x8B
	TEST(CPUArithmeticAndLogic, ADC_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x8B);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// ADC A, H
	// Opcode: 0x8C
	TEST(CPUArithmeticAndLogic, ADC_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x8C);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// ADC A, L
	// Opcode: 0x8D
	TEST(CPUArithmeticAndLogic, ADC_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x8D);
		auto processor = CPU(mmu);

		Test8BitAddWithCarry(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// ADC A, (HL)
	// Opcode: 0x8E
	TEST(CPUArithmeticAndLogic, ADC_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0x8E);
		auto processor = CPU(mmu);

		mmu.SetByte(address, operand2);

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2 + carryFlag);
	}

	// ADC A, A
	// Opcode: 0x8F
	TEST(CPUArithmeticAndLogic, ADC_A_A)
	{
		uint8_t operand1 = 2;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0x8F);
		auto processor = CPU(mmu);

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand1 + carryFlag);
	}

	// SUB A, B
	// Opcode: 0x90
	TEST(CPUArithmeticAndLogic, SUB_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x90);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// SUB A, C
	// Opcode: 0x91
	TEST(CPUArithmeticAndLogic, SUB_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x91);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// SUB A, D
	// Opcode: 0x92
	TEST(CPUArithmeticAndLogic, SUB_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x92);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// SUB A, E
	// Opcode: 0x93
	TEST(CPUArithmeticAndLogic, SUB_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x93);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// SUB A, H
	// Opcode: 0x94
	TEST(CPUArithmeticAndLogic, SUB_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x94);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// SUB A, L
	// Opcode: 0x95
	TEST(CPUArithmeticAndLogic, SUB_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x95);
		auto processor = CPU(mmu);

		Test8BitSubtraction(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// SUB A, (HL)
	// Opcode: 0x96
	TEST(CPUArithmeticAndLogic, SUB_A_HL)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		auto mmu = CreatePresetMemoryManagementUnit(0x96);
		mmu.SetByte(address, operand2);

		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2);
	}

	// SUB A, A
	// Opcode: 0x97
	TEST(CPUArithmeticAndLogic, SUB_A_A)
	{
		uint8_t operand = 2;

		auto mmu = CreatePresetMemoryManagementUnit(0x97);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), 0);
	}

	// SBC A, B
	// Opcode: 0x98
	TEST(CPUArithmeticAndLogic, SBC_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x98);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// SBC A, C
	// Opcode: 0x99
	TEST(CPUArithmeticAndLogic, SBC_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x99);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// SBC A, D
	// Opcode: 0x9A
	TEST(CPUArithmeticAndLogic, SBC_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x9A);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// SBC A, E
	// Opcode: 0x9B
	TEST(CPUArithmeticAndLogic, SBC_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x9B);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// SBC A, H
	// Opcode: 0x9C
	TEST(CPUArithmeticAndLogic, SBC_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x9C);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// SBC A, L
	// Opcode: 0x9D
	TEST(CPUArithmeticAndLogic, SBC_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0x9D);
		auto processor = CPU(mmu);

		Test8BitSubtractWithCarry(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// SBC A, (HL)
	// Opcode: 0x9E
	TEST(CPUArithmeticAndLogic, SBC_A_ADDR_HL)
	{
		uint8_t operand1 = 6;
		uint8_t operand2 = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0x9E);
		auto processor = CPU(mmu);

		mmu.SetByte(address, operand2);

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2 - carryFlag);
	}

	// SBC A, A
	// Opcode: 0x9F
	TEST(CPUArithmeticAndLogic, SBC_A_A)
	{
		uint8_t operand1 = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0x9F);
		auto processor = CPU(mmu);

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		// TODO: Re-assess this
		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), (uint8_t)-1);
	}

	// AND A, B
	// Opcode: 0xA0
	TEST(CPUArithmeticAndLogic, AND_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA0);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// AND A, C
	// Opcode: 0xA1
	TEST(CPUArithmeticAndLogic, AND_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA1);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// AND A, D
	// Opcode: 0xA2
	TEST(CPUArithmeticAndLogic, AND_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA2);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// AND A, E
	// Opcode: 0xA3
	TEST(CPUArithmeticAndLogic, AND_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA3);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// AND A, H
	// Opcode: 0xA4
	TEST(CPUArithmeticAndLogic, AND_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA4);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// AND A, L
	// Opcode: 0xA5
	TEST(CPUArithmeticAndLogic, AND_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA5);
		auto processor = CPU(mmu);

		Test8BitAND(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// AND A, (HL)
	// Opcode: 0xA6
	TEST(CPUArithmeticAndLogic, AND_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		auto mmu = CreatePresetMemoryManagementUnit(0xA6);
		mmu.SetByte(address, operand2);

		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 & operand2);
	}

	// AND A, A
	// Opcode: 0xA7
	TEST(CPUArithmeticAndLogic, AND_A_A)
	{
		uint8_t operand = 7;

		auto mmu = CreatePresetMemoryManagementUnit(0xA7);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand & operand);
	}

	// XOR A, B
	// Opcode: 0xA8
	TEST(CPUArithmeticAndLogic, XOR_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA8);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// XOR A, C
	// Opcode: 0xA9
	TEST(CPUArithmeticAndLogic, XOR_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xA9);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// XOR A, D
	// Opcode: 0xAA
	TEST(CPUArithmeticAndLogic, XOR_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xAA);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// XOR A, E
	// Opcode: 0xAB
	TEST(CPUArithmeticAndLogic, XOR_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xAB);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// XOR A, H
	// Opcode: 0xAC
	TEST(CPUArithmeticAndLogic, XOR_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xAC);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// XOR A, L
	// Opcode: 0xAD
	TEST(CPUArithmeticAndLogic, XOR_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xAD);
		auto processor = CPU(mmu);

		Test8BitXOR(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// XOR A, (HL)
	// Opcode: 0xAE
	TEST(CPUArithmeticAndLogic, XOR_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		auto mmu = CreatePresetMemoryManagementUnit(0xAE);
		mmu.SetByte(address, operand2);

		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 ^ operand2);
	}

	// XOR A, A
	// Opcode: 0xAF
	TEST(CPUArithmeticAndLogic, XOR_A_A)
	{
		uint8_t operand = 7;

		auto mmu = CreatePresetMemoryManagementUnit(0xAF);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand ^ operand);
	}

	// OR A, B
	// Opcode: 0xB0
	TEST(CPUArithmeticAndLogic, OR_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB0);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// OR A, C
	// Opcode: 0xB1
	TEST(CPUArithmeticAndLogic, OR_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB1);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// OR A, D
	// Opcode: 0xB2
	TEST(CPUArithmeticAndLogic, OR_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB2);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// OR A, E
	// Opcode: 0xB3
	TEST(CPUArithmeticAndLogic, OR_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB3);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// OR A, H
	// Opcode: 0xB4
	TEST(CPUArithmeticAndLogic, OR_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB4);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// OR A, L
	// Opcode: 0xB5
	TEST(CPUArithmeticAndLogic, OR_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB5);
		auto processor = CPU(mmu);

		Test8BitOR(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// OR A, (HL)
	// Opcode: 0xB6
	TEST(CPUArithmeticAndLogic, OR_A_HL)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint16_t address = 320;

		auto mmu = CreatePresetMemoryManagementUnit(0xB6);
		mmu.SetByte(address, operand2);

		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 | operand2);
	}

	// OR A, A
	// Opcode: 0xB7
	TEST(CPUArithmeticAndLogic, OR_A_A)
	{
		uint8_t operand = 7;

		auto mmu = CreatePresetMemoryManagementUnit(0xB7);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand | operand);
	}

	// CP A, B
	// Opcode: 0xB8
	TEST(CPUArithmeticAndLogic, CP_A_B)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB8);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::BC).GetHighRegister());
	}

	// CP A, C
	// Opcode: 0xB9
	TEST(CPUArithmeticAndLogic, CP_A_C)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xB9);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::BC).GetLowRegister());
	}

	// CP A, D
	// Opcode: 0xBA
	TEST(CPUArithmeticAndLogic, CP_A_D)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xBA);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::DE).GetHighRegister());
	}

	// CP A, E
	// Opcode: 0xBB
	TEST(CPUArithmeticAndLogic, CP_A_E)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xBB);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::DE).GetLowRegister());
	}

	// CP A, H
	// Opcode: 0xBC
	TEST(CPUArithmeticAndLogic, CP_A_H)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xBC);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::HL).GetHighRegister());
	}

	// CP A, L
	// Opcode: 0xBD
	TEST(CPUArithmeticAndLogic, CP_A_L)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xBD);
		auto processor = CPU(mmu);

		Test8BitCompare(processor, processor.GetRegister(CPURegisterID::HL).GetLowRegister());
	}

	// CP A, (HL)
	// Opcode: 0xBE
	TEST(CPUArithmeticAndLogic, CP_A_ADDR_HL)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 7;
		uint16_t address = 300;

		auto mmu = CreatePresetMemoryManagementUnit(0xBE);
		auto processor = CPU(mmu);
		mmu.SetByte(address, operand2);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.GetRegister(CPURegisterID::HL).SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	// CP A, A
	// Opcode: 0xBF
	TEST(CPUArithmeticAndLogic, CP_A_A)
	{
		auto mmu = CreatePresetMemoryManagementUnit(0xBF);
		auto processor = CPU(mmu);

		uint8_t operand = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	// ADD A, U8
	// Opcode: 0xC6
	TEST(CPUArithmeticAndLogic, ADD_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0xC6);
		auto processor = CPU(mmu);

		mmu.SetByte(1, operand2);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2);
	}

	// SUB A, U8
	// Opcode: 0xD6
	TEST(CPUArithmeticAndLogic, SUB_A_U8)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0xD6);
		auto processor = CPU(mmu);

		mmu.SetByte(1, operand2);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2);
	}

	// AND A, U8
	// Opcode: 0xE6
	TEST(CPUArithmeticAndLogic, AND_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0xE6);
		auto processor = CPU(mmu);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 & operand2);
	}

	// OR A, U8
	// Opcode: 0xF6
	TEST(CPUArithmeticAndLogic, OR_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0xF6);
		auto processor = CPU(mmu);

		mmu.SetByte(1, operand2);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 | operand2);
	}

	// ADD SP, I8
	// Opcode: 0xE8
	TEST(CPUArithmeticAndLogic, ADD_SP_I8)
	{
		uint16_t operand1 = 300;
		int8_t operand2 = -3;

		auto mmu = CreatePresetMemoryManagementUnit(0xE8);
		auto processor = CPU(mmu);
		mmu.SetByte(1, operand2);

		processor.GetRegister(CPURegisterID::SP).SetData(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::SP).GetData(), operand1 + operand2);
	}

	// LD HL, SP + I8
	// Opcode: 0xF8
	TEST(CPUArithmeticAndLogic, LD_HL_SPI8)
	{
		uint16_t stackPointerValue = 320;
		int8_t signedIntValue = -5;

		auto mmu = CreatePresetMemoryManagementUnit(0xF8);
		auto processor = CPU(mmu);

		mmu.SetByte(1, signedIntValue);
		processor.GetRegister(CPURegisterID::SP).SetData(stackPointerValue);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), stackPointerValue + signedIntValue);
	}

	// ADC A, U8
	// Opcode: 0xCE
	TEST(CPUArithmeticAndLogic, ADC_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0xCE);
		auto processor = CPU(mmu);
		mmu.SetByte(1, operand2);

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2 + carryFlag);
	}

	// SBC A, U8
	// Opcode: 0xDE
	TEST(CPUArithmeticAndLogic, SBC_A_U8)
	{
		uint8_t operand1 = 5;
		uint8_t operand2 = 2;
		uint8_t carryFlag = 1;

		auto mmu = CreatePresetMemoryManagementUnit(0xDE);
		auto processor = CPU(mmu);

		mmu.SetByte(1, operand2);
		processor.SetCarryFlag(carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2 - carryFlag);
	}

	// XOR A, U8
	// Opcode: 0xEE
	TEST(CPUArithmeticAndLogic, XOR_A_U8)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		auto mmu = CreatePresetMemoryManagementUnit(0xEE);
		auto processor = CPU(mmu);

		mmu.SetByte(1, operand2);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 ^ operand2);
	}

	// CP A, U8
	// Opcode: 0xFE
	TEST(CPUArithmeticAndLogic, CP_A_U8)
	{
		uint8_t operand1 = 9;
		uint8_t operand2 = 9;

		auto mmu = CreatePresetMemoryManagementUnit(0xFE);
		auto processor = CPU(mmu);
		mmu.SetByte(1, operand2);

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		processor.Cycle();

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}

	void Test8BitAddition(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Add);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2);
	}

	void Test16BitAddition(CPU& processor, Register16& targetRegister)
	{
		uint16_t operand1 = 300;
		uint16_t operand2 = 500;

		processor.GetRegister(CPURegisterID::HL).SetData(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Add);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), operand1 + operand2);
	}

	void Test8BitSubtraction(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 7;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Subtract);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2);
	}

	void Test8BitAND(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::AND);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 & operand2);
	}

	void Test8BitXOR(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::XOR);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 ^ operand2);
	}

	void Test8BitOR(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::OR);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 | operand2);
	}

	void Test8BitAddWithCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 2;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::AddWithCarry);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 + operand2 + carryFlag);
	}

	void Test8BitSubtractWithCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 8;
		uint8_t operand2 = 5;
		uint8_t carryFlag = 1;

		processor.SetCarryFlag((bool)carryFlag);
		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::SubtractWithCarry);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetHighByte(), operand1 - operand2 - carryFlag);
	}

	void Test8BitIncrement(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Increment);
		EXPECT_EQ(prevInstruction.data[0], operand);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(targetRegister.GetData(), operand + 1);
	}

	void Test16BitIncrement(CPU& processor, Register16& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Increment);
		EXPECT_EQ(prevInstruction.data[0], operand);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);

		EXPECT_EQ(targetRegister.GetData(), operand + 1);
	}

	void Test8BitDecrement(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Decrement);
		EXPECT_EQ(prevInstruction.data[0], operand);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::EightBitRegisterData);

		EXPECT_EQ(targetRegister.GetData(), operand - 1);
	}

	void Test16BitDecrement(CPU& processor, Register16& targetRegister)
	{
		uint8_t operand = 5;

		targetRegister.SetData(operand);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Decrement);
		EXPECT_EQ(prevInstruction.data[0], operand);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::SixteenBitRegisterData);

		EXPECT_EQ(targetRegister.GetData(), operand - 1);
	}

	void Test8BitCompare(CPU& processor, Register8& targetRegister)
	{
		uint8_t operand1 = 5;
		uint8_t operand2 = 5;

		processor.GetRegister(CPURegisterID::AF).SetHighByte(operand1);
		targetRegister.SetData(operand2);

		processor.Cycle();

		CPUInstruction prevInstruction = processor.GetPreviouslyExecutedInstruction();

		EXPECT_EQ(prevInstruction.instructionType, CPUInstructionType::Compare);
		EXPECT_EQ(prevInstruction.data[0], operand1);
		EXPECT_EQ(prevInstruction.data[1], operand2);
		EXPECT_EQ(prevInstruction.storageType, CPUInstructionStorageType::None);

		EXPECT_EQ(processor.GetZeroFlag(), 1);
	}
}