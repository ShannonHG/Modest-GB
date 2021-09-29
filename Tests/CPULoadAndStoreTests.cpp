#include "CPU/CPU.hpp"
#include "gtest/gtest.h"
#include "Memory/Memory.hpp"
#include "Common/DataConversions.hpp"
#include "Logger.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	void TestPop(CPU& processor, Memory& memory, Register16& reg)
	{
		uint16_t operand = 320;
		uint16_t address = 450;

		memory.SetByte(address, operand & 0x00FF);
		memory.SetByte(address + 1, operand >> 8);
		processor.GetStackPointer().SetData(address);
		processor.Cycle();

		EXPECT_EQ(reg.GetData(), operand);
		EXPECT_EQ(processor.GetStackPointer().GetData(), address + 2);
	}

	void TestPush(CPU& processor, Memory& memory, Register16& reg)
	{
		uint16_t operand = 320;
		uint16_t address = 450;

		reg.SetData(operand);
		processor.GetStackPointer().SetData(address);
		processor.Cycle();

		uint8_t lower = memory.GetByte(address - 2);
		uint8_t upper = memory.GetByte((address - 2) + 1);

		EXPECT_EQ((upper << 8) | lower, operand);
		EXPECT_EQ(processor.GetStackPointer().GetData(), address - 2);
	}

	// LD B, B
	// Opcode: 0x40
	TEST(CPULoadAndStore, LD_B_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x40);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, B
	// Opcode: 0x50
	TEST(CPULoadAndStore, LD_D_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x50);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, B
	// Opcode: 0x60
	TEST(CPULoadAndStore, LD_H_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x60);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), B
	// Opcode: 0x70
	TEST(CPULoadAndStore, LD_ADDR_HL_B)
	{
		uint8_t value = 7;
		uint16_t address = 320;
		Memory memory = CreatePresetMemory(0x70);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (FF00 + U8), A
	// Opcode: 0xE0
	TEST(CPULoadAndStore, LD_FFOO_U8_A)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		Memory memory = CreatePresetMemory(0xE0);
		memory.SetByte(1, address);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		uint8_t data = memory.GetByte(0xFF00 | address);

		EXPECT_EQ(data, value);
	}

	// LD A, (FF00 + U8)
	// Opcode: 0xF0
	TEST(CPULoadAndStore, LD_A_ADDR_FF00_U8)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		Memory memory = CreatePresetMemory(0xF0);

		memory.SetByte(1, address);
		memory.SetByte(0xFF00 | address, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD BC, U16
	// Opcode: 0x01
	TEST(CPULoadAndStore, LD_BC_U16)
	{
		uint16_t value = 300;
		Memory memory = CreatePresetMemory(0x01);
		memory.SetByte(1, value & 0x00FF);
		memory.SetByte(2, value >> 8);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterBC().GetData(), value);
	}

	// LD DE, U16
	// Opcode: 0x11
	TEST(CPULoadAndStore, LD_DE_U16)
	{
		uint16_t value = 300;
		Memory memory = CreatePresetMemory(0x11);
		memory.SetByte(1, value & 0x00FF);
		memory.SetByte(2, value >> 8);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterDE().GetData(), value);
	}

	// LD HL, U16
	// Opcode: 0x21
	TEST(CPULoadAndStore, LD_HL_U16)
	{
		uint16_t value = 300;
		Memory memory = CreatePresetMemory(0x21);
		memory.SetByte(1, value & 0x00FF);
		memory.SetByte(2, value >> 8);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), value);
	}

	// LD SP, U16
	// Opcode: 0x31
	TEST(CPULoadAndStore, LD_SP_U16)
	{
		uint16_t value = 300;
		Memory memory = CreatePresetMemory(0x31);
		memory.SetByte(1, value & 0x00FF);
		memory.SetByte(2, value >> 8);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetStackPointer().GetData(), value);
	}

	// LD B, C
	// Opcode: 0x41
	TEST(CPULoadAndStore, LD_B_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x41);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, C
	// Opcode: 0x51
	TEST(CPULoadAndStore, LD_D_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x51);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, C
	// Opcode: 0x61
	TEST(CPULoadAndStore, LD_H_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x61);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), C
	// Opcode: 0x71
	TEST(CPULoadAndStore, LD_ADDR_HL_C)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x71);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (BC), A
	// Opcode: 0x02
	TEST(CPULoadAndStore, LD_ADDR_BC_A)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x02);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterBC().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (DE), A
	// Opcode: 0x12
	TEST(CPULoadAndStore, LD_ADDR_DE_A)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x12);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterDE().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (HL+), A
	// Opcode: 0x22
	TEST(CPULoadAndStore, LD_ADDR_INC_HL_A)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x22);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
		EXPECT_EQ(processor.GetRegisterHL().GetData(), address + 1);
	}

	// LD (HL-), A
	// Opcode: 0x32
	TEST(CPULoadAndStore, LD_ADDR_DEC_HL_A)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x32);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
		EXPECT_EQ(processor.GetRegisterHL().GetData(), address - 1);
	}

	// LD B, D
	// Opcode: 0x42
	TEST(CPULoadAndStore, LD_D_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x42);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD H, D
	// Opcode: 0x62
	TEST(CPULoadAndStore, LD_H_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x62);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), D
	// Opcode: 0x72
	TEST(CPULoadAndStore, LD_ADDR_HL_D)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x72);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (FF00 + C), A
	// Opcode: 0xE2
	TEST(CPULoadAndStore, LD_FF00_C_A)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		Memory memory = CreatePresetMemory(0xE2);
		memory.SetByte(1, address);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterC().SetData(address);
		processor.Cycle();

		uint8_t data = memory.GetByte(0xFF00 | address);

		EXPECT_EQ(data, value);
	}

	// LD A, (FF00 + C)
	// Opcode: 0xF2
	TEST(CPULoadAndStore, LD_A_ADDR_FF00_C)
	{
		uint8_t value = 7;
		uint8_t address = 10;
		Memory memory = CreatePresetMemory(0xF2);
		memory.SetByte(0xFF00 | address, 7);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD B, E
	// Opcode: 0x43
	TEST(CPULoadAndStore, LD_B_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x43);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, E
	// Opcode: 0x53
	TEST(CPULoadAndStore, LD_D_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x53);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, E
	// Opcode: 0x63
	TEST(CPULoadAndStore, LD_H_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x63);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), E
	// Opcode: 0x73
	TEST(CPULoadAndStore, LD_ADDR_HL_E)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x73);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD B, H
	// Opcode: 0x44
	TEST(CPULoadAndStore, LD_B_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x44);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, H
	// Opcode: 0x54
	TEST(CPULoadAndStore, LD_D_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x54);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, H
	// Opcode: 0x64
	TEST(CPULoadAndStore, LD_H_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x64);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), H
	// Opcode: 0x74
	TEST(CPULoadAndStore, LD_ADDR_HL_H)
	{
		uint8_t value = 7;
		uint16_t address = value << 8;
		Memory memory = CreatePresetMemory(0x74);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD B, L
	// Opcode: 0x45
	TEST(CPULoadAndStore, LD_B_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x45);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, L
	// Opcode: 0x55
	TEST(CPULoadAndStore, LD_D_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x55);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD, H, L
	// Opcode: 0x65
	TEST(CPULoadAndStore, LD_H_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x65);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), L
	// Opcode: 0x75
	TEST(CPULoadAndStore, LD_ADDR_HL_L)
	{
		uint8_t value = 7;
		uint16_t address = value;
		Memory memory = CreatePresetMemory(0x75);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD B, U8
	// Opcode: 0x06
	TEST(CPULoadAndStore, LD_B_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x06);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, U8
	// Opcode: 0x16
	TEST(CPULoadAndStore, LD_D_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x16);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, U8
	// Opcode: 0x26
	TEST(CPULoadAndStore, LD_H_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x26);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), U8
	// Opcode: 0x36
	TEST(CPULoadAndStore, LD_ADDR_HL_U8)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x36);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD B, (HL)
	// Opcode: 0x46
	TEST(CPULoadAndStore, LD_B_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x46);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, (HL)
	// Opcode: 0x56
	TEST(CPULoadAndStore, LD_D_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x56);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, (HL)
	// Opcode: 0x66
	TEST(CPULoadAndStore, LD_H_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 400;

		Memory memory = CreatePresetMemory(0x66);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD B, A
	// Opcode: 0x47
	TEST(CPULoadAndStore, LD_B_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x47);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterB().GetData(), value);
	}

	// LD D, A
	// Opcode: 0x57
	TEST(CPULoadAndStore, LD_D_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x57);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterD().GetData(), value);
	}

	// LD H, A
	// Opcode: 0x67
	TEST(CPULoadAndStore, LD_H_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x67);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterH().GetData(), value);
	}

	// LD (HL), A
	// Opcode: 0x77
	TEST(CPULoadAndStore, LD_ADDR_HL_A)
	{
		uint8_t value = 7;
		uint16_t address = 400;
		Memory memory = CreatePresetMemory(0x77);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(memory.GetByte(address), value);
	}

	// LD (U16), SP
	// Opcode: 0x08
	TEST(CPULoadAndStore, LD_ADDR_U16_SP)
	{
		uint16_t value = 270;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0x08);
		memory.SetByte(1, address & 0x00FF);
		memory.SetByte(2, address >> 8);

		auto processor = CPU(memory);
		processor.GetStackPointer().SetData(value);
		processor.Cycle();

		uint8_t lowerByte = memory.GetByte(address);
		uint8_t upperByte = memory.GetByte(address + 1);

		EXPECT_EQ((upperByte << 8) | lowerByte, value);
	}

	// LD C, B
	// Opcode: 0x48
	TEST(CPULoadAndStore, LD_C_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x48);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, B
	// Opcode: 0x58
	TEST(CPULoadAndStore, LD_E_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x58);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, B
	// Opcode: 0x68
	TEST(CPULoadAndStore, LD_L_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x68);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, B
	// Opcode: 0x78
	TEST(CPULoadAndStore, LD_A_B)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x78);

		auto processor = CPU(memory);
		processor.GetRegisterB().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, C
	// Opcode: 0x49
	TEST(CPULoadAndStore, LD_C_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x49);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, C
	// Opcode: 0x59
	TEST(CPULoadAndStore, LD_E_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x59);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, C
	// Opcode: 0x69
	TEST(CPULoadAndStore, LD_L_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x69);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, C
	// Opcode: 0x79
	TEST(CPULoadAndStore, LD_A_C)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x79);

		auto processor = CPU(memory);
		processor.GetRegisterC().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD A, (BC)
	// Opcode: 0x0A
	TEST(CPULoadAndStore, LD_A_ADDR_BC)
	{
		uint8_t value = 8;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x0A);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterBC().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD A, (DE)
	// Opcode: 0x1A
	TEST(CPULoadAndStore, LD_A_ADDR_DE)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x1A);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterDE().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD A, (HL+)
	// Opcode: 0x2A
	TEST(CPULoadAndStore, LD_A_ADDR_HL_INC)
	{
		uint8_t value = 9;
		uint16_t address = 400;

		Memory memory = CreatePresetMemory(0x2A);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), address + 1);
		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD A, (HL-)
	// Opcode: 0x3A
	TEST(CPULoadAndStore, LD_A_ADDR_HL_DEC)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x3A);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterHL().GetData(), address - 1);
		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, D
	// Opcode: 0x4A
	TEST(CPULoadAndStore, LD_C_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x4A);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, D
	// Opcode: 0x5A
	TEST(CPULoadAndStore, LD_E_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x5A);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, D
	// Opcode: 0x6A
	TEST(CPULoadAndStore, LD_L_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x6A);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, D
	// Opcode: 0x7A
	TEST(CPULoadAndStore, LD_A_D)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x7A);

		auto processor = CPU(memory);
		processor.GetRegisterD().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD (U16), A
	// Opcode: 0xEA
	TEST(CPULoadAndStore, LD_ADDR_U16_A)
	{
		uint16_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0xFA);
		memory.SetByte(1, address & 0x00FF);
		memory.SetByte(2, address >> 8);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		uint8_t data = memory.GetByte(address);

		EXPECT_EQ(data, value);
	}

	// LD A, (U16)
	// Opcode: 0xFA
	TEST(CPULoadAndStore, LD_A_U16)
	{
		uint16_t value = 7;
		uint16_t address = 300;
		Memory memory = CreatePresetMemory(0xFA);
		memory.SetByte(1, address & 0x00FF);
		memory.SetByte(2, address >> 8);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, E
	// Opcode: 0x4B
	TEST(CPULoadAndStore, LD_C_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x4B);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, E
	// Opcode: 0x5B
	TEST(CPULoadAndStore, LD_E_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x5B);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, E
	// Opcode: 0x6B
	TEST(CPULoadAndStore, LD_L_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x6B);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, E
	// Opcode: 0x7B
	TEST(CPULoadAndStore, LD_A_E)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x7B);

		auto processor = CPU(memory);
		processor.GetRegisterE().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, H
	// Opcode: 0x4C
	TEST(CPULoadAndStore, LD_C_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x4C);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, H
	// Opcode: 0x5C
	TEST(CPULoadAndStore, LD_E_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x5C);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, H
	// Opcode: 0x6C
	TEST(CPULoadAndStore, LD_L_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x6C);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, H
	// Opcode: 0x7C
	TEST(CPULoadAndStore, LD_A_H)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x7C);

		auto processor = CPU(memory);
		processor.GetRegisterH().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, L
	// Opcode: 0x4D
	TEST(CPULoadAndStore, LD_C_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x4D);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, L
	// Opcode: 0x5D
	TEST(CPULoadAndStore, LD_E_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x5D);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, L
	// Opcode: 0x6D
	TEST(CPULoadAndStore, LD_L_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x6D);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, L
	// Opcode: 0x7D
	TEST(CPULoadAndStore, LD_A_L)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x7D);

		auto processor = CPU(memory);
		processor.GetRegisterL().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, U8
	// Opcode: 0x0E
	TEST(CPULoadAndStore, LD_C_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x0E);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, U8
	// Opcode: 0x1E
	TEST(CPULoadAndStore, LD_E_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x1E);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, U8
	// Opcode: 0x2E
	TEST(CPULoadAndStore, LD_L_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x2E);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, U8
	// Opcode: 0x3E
	TEST(CPULoadAndStore, LD_A_U8)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x3E);
		memory.SetByte(1, value);

		auto processor = CPU(memory);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, (HL)
	// Opcode: 0x4E
	TEST(CPULoadAndStore, LD_C_HL)
	{
		uint8_t value = 10;
		uint16_t address = 350;

		Memory memory = CreatePresetMemory(0x4E);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, (HL)
	// Opcode: 0x5E
	TEST(CPULoadAndStore, LD_E_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 400;

		Memory memory = CreatePresetMemory(0x5E);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, (HL)
	// Opcode: 0x6E
	TEST(CPULoadAndStore, LD_L_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 300;

		Memory memory = CreatePresetMemory(0x6E);
		memory.SetByte(address, value);


		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);

		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, (HL)
	// Opcode: 0x7E
	TEST(CPULoadAndStore, LD_A_ADDR_HL)
	{
		uint8_t value = 7;
		uint16_t address = 400;

		Memory memory = CreatePresetMemory(0x7E);
		memory.SetByte(address, value);

		auto processor = CPU(memory);
		processor.GetRegisterHL().SetData(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// LD C, A
	// Opcode: 0x4F
	TEST(CPULoadAndStore, LD_C_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x4F);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterC().GetData(), value);
	}

	// LD E, A
	// Opcode: 0x5F
	TEST(CPULoadAndStore, LD_E_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x5F);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterE().GetData(), value);
	}

	// LD L, A
	// Opcode: 0x6F
	TEST(CPULoadAndStore, LD_L_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x6F);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterL().GetData(), value);
	}

	// LD A, A
	// Opcode: 0x7F
	TEST(CPULoadAndStore, LD_A_A)
	{
		uint8_t value = 7;
		Memory memory = CreatePresetMemory(0x7F);

		auto processor = CPU(memory);
		processor.GetRegisterA().SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegisterA().GetData(), value);
	}

	// POP BC
	// Opcode: 0xC1
	TEST(CPULoadAndStore, POP_BC)
	{
		Memory memory = CreatePresetMemory(0xC1);
		auto processor = CPU(memory);

		TestPop(processor, memory, processor.GetRegisterBC());
	}

	// POP DE
	// Opcode: 0xD1
	TEST(CPULoadAndStore, POP_DE)
	{
		Memory memory = CreatePresetMemory(0xD1);
		auto processor = CPU(memory);

		TestPop(processor, memory, processor.GetRegisterDE());
	}

	// POP HL
	// Opcode: 0xE1
	TEST(CPULoadAndStore, POP_HL)
	{
		Memory memory = CreatePresetMemory(0xE1);
		auto processor = CPU(memory);

		TestPop(processor, memory, processor.GetRegisterHL());
	}

	// POP AF
	// Opcode: 0xF1
	TEST(CPULoadAndStore, POP_AF)
	{
		Memory memory = CreatePresetMemory(0xF1);
		auto processor = CPU(memory);

		TestPop(processor, memory, processor.GetRegisterAF());
	}

	// PUSH BC
	// Opcode: 0xC5
	TEST(CPULoadAndStore, PUSH_BC)
	{
		Memory memory = CreatePresetMemory(0xC5);
		auto processor = CPU(memory);

		TestPush(processor, memory, processor.GetRegisterBC());
	}

	// PUSH DE
	// Opcode: 0xD5
	TEST(CPULoadAndStore, PUSH_DE)
	{
		Memory memory = CreatePresetMemory(0xD5);
		auto processor = CPU(memory);

		TestPush(processor, memory, processor.GetRegisterDE());
	}

	// PUSH HL
	// Opcode: 0xE5
	TEST(CPULoadAndStore, PUSH_HL)
	{
		Memory memory = CreatePresetMemory(0xE5);
		auto processor = CPU(memory);

		TestPush(processor, memory, processor.GetRegisterHL());
	}

	// PUSH AF
	// Opcode: 0xF5
	TEST(CPULoadAndStore, PUSH_AF)
	{
		Memory memory = CreatePresetMemory(0xF5);
		auto processor = CPU(memory);

		TestPush(processor, memory, processor.GetRegisterAF());
	}
}