#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void Test8BitRotateLeftThroughCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 2;
		uint8_t carryFlag = 1;
		uint8_t previousHighBit = data >> 7;

		// Shift left and set the least significant bit equal to the carry flag.
		uint8_t expectedResult = (data << 1) | (carryFlag);

		targetRegister.SetData(data);
		processor.ChangeCarryFlag(carryFlag);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousHighBit);
	}

	void Test8BitRotateRightThroughCarry(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 2;
		uint8_t carryFlag = 1;
		uint8_t previousLowBit = (data & 1);

		// Shift right and set the most significant bit equal to the carry flag.
		uint8_t expectedResult = (data >> 1) | (carryFlag << 7);

		targetRegister.SetData(data);
		processor.ChangeCarryFlag(carryFlag);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousLowBit);
	}

	void Test8BitRotateLeft(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 8;

		// Shift left and set the least significant bit equal to the previous most significant bit.
		uint8_t expectedResult = (data << 1) | (data >> 7);

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	void Test8BitRotateRight(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 4;

		// Shift right and set the most significant bit equal to the previous least significant bit.
		uint8_t expectedResult = (data >> 1) | (data << 7);

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	void Test8BitArithmeticLeftShift(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 7;
		uint8_t expectedResult = data << 1;

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	void Test8BitArithmeticRightShift(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 7;
		uint8_t expectedResult = (data >> 1) | (data & 0b10000000);

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	void Test8BitLogicalRightShift(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 7;
		uint8_t expectedResult = data >> 1;

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	void Test8BitSwap(CPU& processor, Register8& targetRegister)
	{
		uint8_t data = 245;
		uint8_t expectedResult = ((data & 0x0F) << 4) | ((data & 0xF0) >> 4) ;

		targetRegister.SetData(data);
		processor.Step();

		EXPECT_EQ(targetRegister.GetData(), expectedResult);
	}

	// RLC B
	// Opcode 0xCB00
	TEST(CPURotateAndShiftInstructions, RLC_B)
	{
		Memory memory = CreatePresetMemory({0xCB, 0x00});
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterB());
	}

	// RLC C
	// Opcode 0xCB01
	TEST(CPURotateAndShiftInstructions, RLC_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x01 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterC());
	}

	// RLC D
	// Opcode 0xCB02
	TEST(CPURotateAndShiftInstructions, RLC_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x02 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterD());
	}

	// RLC E
	// Opcode 0xCB03
	TEST(CPURotateAndShiftInstructions, RLC_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x03 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterE());
	}

	// RLC H
	// Opcode 0xCB04
	TEST(CPURotateAndShiftInstructions, RLC_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x04 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterH());
	}

	// RLC L
	// Opcode 0xCB05
	TEST(CPURotateAndShiftInstructions, RLC_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x05 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterL());
	}

	// RLC (HL)
	// Opcode 0xCB06
	TEST(CPURotateAndShiftInstructions, RLC_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = (data << 1) | (data >> 7);

		Memory memory = CreatePresetMemory({ 0xCB, 0x06 });
		memory.SetByte(address, data);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// RLCA
	// Opcode 0x07
	TEST(CPURotateAndShiftInstructions, RLCA)
	{
		Memory memory = CreatePresetMemory(0x07);
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterA());
	}

	// RLC A
	// Opcode 0xCB07
	TEST(CPURotateAndShiftInstructions, RLC_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x07 });
		auto processor = CPU(memory);

		Test8BitRotateLeft(processor, processor.GetRegisterA());
	}

	// RRC B
	// Opcode 0xCB08
	TEST(CPURotateAndShiftInstructions, RRC_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x08 });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterB());
	}

	// RRC C
	// Opcode 0xCB09
	TEST(CPURotateAndShiftInstructions, RRC_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x09 });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterC());
	}

	// RRC D
	// Opcode 0xCB0A
	TEST(CPURotateAndShiftInstructions, RRC_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x0A });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterD());
	}

	// RRC E
	// Opcode 0xCB0B
	TEST(CPURotateAndShiftInstructions, RRC_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x0B });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterE());
	}

	// RRC H
	// Opcode 0xCB0C
	TEST(CPURotateAndShiftInstructions, RRC_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x0C });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterH());
	}

	// RRC L
	// Opcode 0xCB0D
	TEST(CPURotateAndShiftInstructions, RRC_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x0D });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterL());
	}

	// RRC (HL)
	// Opcode 0xCB0E
	TEST(CPURotateAndShiftInstructions, RRC_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = (data >> 1) | (data << 7);

		Memory memory = CreatePresetMemory({ 0xCB, 0x0E });
		memory.SetByte(address, data);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// RRCA
	// Opcode 0x0F
	TEST(CPURotateAndShiftInstructions, RRCA)
	{
		Memory memory = CreatePresetMemory(0x0F);
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterA());
	}

	// RRC A
	// Opcode 0xCB0F
	TEST(CPURotateAndShiftInstructions, RRC_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x0F });
		auto processor = CPU(memory);

		Test8BitRotateRight(processor, processor.GetRegisterA());
	}

	// RL B
	// Opcode 0xCB10
	TEST(CPURotateAndShiftInstructions, RL_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x10 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterB());
	}

	// RL C
	// Opcode 0xCB11
	TEST(CPURotateAndShiftInstructions, RL_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x11 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterC());
	}

	// RL D
	// Opcode 0xCB12
	TEST(CPURotateAndShiftInstructions, RL_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x12 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterD());
	}

	// RL E
	// Opcode 0xCB13
	TEST(CPURotateAndShiftInstructions, RL_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x13 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterE());
	}

	// RL H
	// Opcode 0xCB14
	TEST(CPURotateAndShiftInstructions, RL_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x14 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterH());
	}

	// RL L
	// Opcode 0xCB15
	TEST(CPURotateAndShiftInstructions, RL_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x15 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterL());
	}

	// RL (HL)
	// Opcode 0xCB16
	TEST(CPURotateAndShiftInstructions, RL_ADDR_HL)
	{
		uint8_t data = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;
		uint8_t expectedResult = (data << 1) | (carryFlag);
		uint8_t previousHighBit = (data & 0b10000000) >> 7;

		Memory memory = CreatePresetMemory({ 0xCB, 0x16 });
		memory.SetByte(address, data);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.ChangeCarryFlag(carryFlag);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousHighBit);
	}

	// RLA
	// Opcode 0x17
	TEST(CPURotateAndShiftInstructions, RLA)
	{
		Memory memory = CreatePresetMemory(0x17);
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterA());
	}

	// RL A
	// Opcode 0xCB17
	TEST(CPURotateAndShiftInstructions, RL_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x17 });
		auto processor = CPU(memory);

		Test8BitRotateLeftThroughCarry(processor, processor.GetRegisterA());
	}

	// RR B
	// Opcode 0xCB18
	TEST(CPURotateAndShiftInstructions, RR_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x18 });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterB());
	}

	// RR C
	// Opcode 0xCB19
	TEST(CPURotateAndShiftInstructions, RR_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x19 });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterC());
	}

	// RR D
	// Opcode 0xCB1A
	TEST(CPURotateAndShiftInstructions, RR_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x1A });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterD());
	}

	// RR E
	// Opcode 0xCB1B
	TEST(CPURotateAndShiftInstructions, RR_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x1B });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterE());
	}

	// RR H
	// Opcode 0xCB1C
	TEST(CPURotateAndShiftInstructions, RR_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x1C });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterH());
	}

	// RR L
	// Opcode 0xCB1D
	TEST(CPURotateAndShiftInstructions, RR_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x1D });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterL());
	}

	// RR (HL)
	// Opcode 0xCB1E
	TEST(CPURotateAndShiftInstructions, RR_ADDR_HL)
	{
		uint8_t data = 2;
		uint16_t address = 300;
		uint8_t carryFlag = 1;
		uint8_t expectedResult = (data >> 1) | (carryFlag << 7);
		uint8_t previousLowBit = (data & 1);

		Memory memory = CreatePresetMemory({ 0xCB, 0x1E });
		memory.SetByte(address, data);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.ChangeCarryFlag(carryFlag);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
		EXPECT_EQ(processor.GetCarryFlag(), previousLowBit);
	}

	// RRA
	// Opcode 0x1F
	TEST(CPURotateAndShiftInstructions, RRA)
	{
		Memory memory = CreatePresetMemory(0x1F);
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterA());
	}

	// RR A
	// Opcode 0xCB1F
	TEST(CPURotateAndShiftInstructions, RR_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x1F });
		auto processor = CPU(memory);

		Test8BitRotateRightThroughCarry(processor, processor.GetRegisterA());
	}

	// SLA B
	// 0xCB20
	TEST(CPURotateAndShiftInstructions, SLA_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x20 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterB());
	}

	// SLA C
	// 0xCB21
	TEST(CPURotateAndShiftInstructions, SLA_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x21 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterC());
	}

	// SLA D
	// 0xCB22
	TEST(CPURotateAndShiftInstructions, SLA_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x22 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterD());
	}

	// SLA E
	// 0xCB23
	TEST(CPURotateAndShiftInstructions, SLA_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x23 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterE());
	}

	// SLA H
	// 0xCB24
	TEST(CPURotateAndShiftInstructions, SLA_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x24 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterH());
	}

	// SLA L
	// 0xCB25
	TEST(CPURotateAndShiftInstructions, SLA_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x25 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterL());
	}

	// SLA (HL)
	// 0xCB26
	TEST(CPURotateAndShiftInstructions, SLA_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = data << 1;

		Memory memory = CreatePresetMemory({ 0xCB, 0x26 });
		auto processor = CPU(memory);

		memory.SetByte(address, data);
		processor.GetRegisterHL().SetData(address);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// SLA A
	// 0xCB27
	TEST(CPURotateAndShiftInstructions, SLA_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x27 });
		auto processor = CPU(memory);

		Test8BitArithmeticLeftShift(processor, processor.GetRegisterA());
	}

	// SRA B
	// 0xCB28
	TEST(CPURotateAndShiftInstructions, SRA_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x28 });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterB());
	}

	// SRA C
	// 0xCB29
	TEST(CPURotateAndShiftInstructions, SRA_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x29 });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterC());
	}

	// SRA D
	// 0xCB2A
	TEST(CPURotateAndShiftInstructions, SRA_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x2A });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterD());
	}

	// SRA E
	// 0xCB2B
	TEST(CPURotateAndShiftInstructions, SRA_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x2B });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterE());
	}

	// SRA H
	// 0xCB2C
	TEST(CPURotateAndShiftInstructions, SRA_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x2C });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterH());
	}

	// SRA L
	// 0xCB2D
	TEST(CPURotateAndShiftInstructions, SRA_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x2D });
		auto processor = CPU(memory);

		Test8BitArithmeticRightShift(processor, processor.GetRegisterL());
	}

	// SRA (HL)
	// 0xCB2E
	TEST(CPURotateAndShiftInstructions, SRA_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = (data >> 1) & (data & 0b10000000);

		Memory memory = CreatePresetMemory({ 0xCB, 0x3E });
		auto processor = CPU(memory);

		processor.GetRegisterHL().SetData(address);

		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// SRA A
	// 0xCB2F
	TEST(CPURotateAndShiftInstructions, SRA_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x2F });
		auto processor = CPU(memory);


		Test8BitArithmeticRightShift(processor, processor.GetRegisterA());
	}

	// SWAP B
	// 0xCB30
	TEST(CPURotateAndShiftInstructions, SWAP_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x30 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterB());
	}

	// SWAP C
	// 0xCB31
	TEST(CPURotateAndShiftInstructions, SWAP_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x31 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterC());
	}

	// SWAP D
	// 0xCB32
	TEST(CPURotateAndShiftInstructions, SWAP_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x32 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterD());
	}

	// SWAP E
	// 0xCB33
	TEST(CPURotateAndShiftInstructions, SWAP_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x33 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterE());
	}

	// SWAP H
	// 0xCB34
	TEST(CPURotateAndShiftInstructions, SWAP_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x34 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterH());
	}

	// SWAP L
	// 0xCB35
	TEST(CPURotateAndShiftInstructions, SWAP_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x35 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterL());
	}

	// SWAP (HL)
	// 0xCB36
	TEST(CPURotateAndShiftInstructions, SWAP_ADDR_HL)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x36 });
		auto processor = CPU(memory);

		uint8_t data = 245;
		uint16_t address = 400;
		uint8_t expectedResult = ((data & 0x0F) << 4) | ((data & 0xF0) >> 4);

		memory.SetByte(address, data);
		processor.GetRegisterHL().SetData(address);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// SWAP A
	// 0xCB37
	TEST(CPURotateAndShiftInstructions, SWAP_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x37 });
		auto processor = CPU(memory);

		Test8BitSwap(processor, processor.GetRegisterA());
	}

	// SRL B
	// 0xCB38
	TEST(CPURotateAndShiftInstructions, SRL_B)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x38 });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterB());
	}

	// SRL C
	// 0xCB39
	TEST(CPURotateAndShiftInstructions, SRL_C)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x39 });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterC());
	}

	// SRL D
	// 0xCB3A
	TEST(CPURotateAndShiftInstructions, SRL_D)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x3A });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterD());
	}

	// SRL E
	// 0xCB3B
	TEST(CPURotateAndShiftInstructions, SRL_E)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x3B });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterE());
	}

	// SRL H
	// 0xCB3C
	TEST(CPURotateAndShiftInstructions, SRL_H)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x3C });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterH());
	}

	// SRL L
	// 0xCB3D
	TEST(CPURotateAndShiftInstructions, SRL_L)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x3D });
		auto processor = CPU(memory);

		Test8BitLogicalRightShift(processor, processor.GetRegisterL());
	}

	// SRL (HL)
	// 0xCB3E
	TEST(CPURotateAndShiftInstructions, SRL_ADDR_HL)
	{
		uint8_t data = 7;
		uint16_t address = 300;
		uint8_t expectedResult = data >> 1;

		Memory memory = CreatePresetMemory({ 0xCB, 0x3E });
		auto processor = CPU(memory);

		memory.SetByte(address, data);
		processor.GetRegisterHL().SetData(address);
		processor.Step();

		EXPECT_EQ(memory.GetByte(address), expectedResult);
	}

	// SRL A
	// 0xCB3F
	TEST(CPURotateAndShiftInstructions, SRL_A)
	{
		Memory memory = CreatePresetMemory({ 0xCB, 0x3F });
		auto processor = CPU(memory);


		Test8BitLogicalRightShift(processor, processor.GetRegisterA());
	}
}