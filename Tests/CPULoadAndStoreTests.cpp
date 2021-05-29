#include "CPU/CPU.hpp"
#include "gtest/gtest.h"
#include "Memory/MemoryManagementUnit.hpp"
#include "MockMemoryManagementUnit.hpp"
#include "Globals.hpp"
#include "Logger.hpp"

namespace SHG
{
	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(uint8_t value)
	{
		auto mmu = MockMemoryManagementUnit(std::numeric_limits<uint16_t>::max());
		mmu.SetByte(0, value);

		return mmu;
	}

	// LD B, B
	// Opcode: 0x40
	TEST(CPU, LD_B_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x40);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, B
	// Opcode: 0x50
	TEST(CPU, LD_D_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x50);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, B
	// Opcode: 0x60
	TEST(CPU, LD_H_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x60);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), B
	// Opcode: 0x70
	TEST(CPU, LD_HL_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x70);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD (FF00 + U8), A
	// Opcode: 0xE0
	TEST(CPU, LD_FFOO_U8_A)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		auto mmu = CreatePresetMemoryManagementUnit(0xE0);
		mmu.SetByte(1, address);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		uint8_t data = mmu.GetByte(0xFF00 | address);

		EXPECT_EQ(data, value);
	}

	// LD A, (FF00 + U8)
	// Opcode: 0xF0
	TEST(CPU, LD_A_FF00_U8)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		auto mmu = CreatePresetMemoryManagementUnit(0xF0);

		mmu.SetByte(1, address);
		mmu.SetByte(0xFF00 | address, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD BC, U16
	// Opcode: 0x01
	TEST(CPU, LD_BC_U16)
	{
		uint16_t value = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0x01);
		mmu.SetByte(1, value & 0x00FF);
		mmu.SetByte(2, value >> 8);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetData(), value);
	}

	// LD DE, U16
	// Opcode: 0x11
	TEST(CPU, LD_DE_U16)
	{
		uint16_t value = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0x11);
		mmu.SetByte(1, value & 0x00FF);
		mmu.SetByte(2, value >> 8);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetData(), value);
	}

	// LD HL, U16
	// Opcode: 0x21
	TEST(CPU, LD_HL_U16)
	{
		uint16_t value = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0x21);
		mmu.SetByte(1, value & 0x00FF);
		mmu.SetByte(2, value >> 8);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD SP, U16
	// Opcode: 0x31
	TEST(CPU, LD_SP_U16)
	{
		uint16_t value = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0x31);
		mmu.SetByte(1, value & 0x00FF);
		mmu.SetByte(2, value >> 8);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::SP).GetData(), value);
	}

	// LD B, C
	// Opcode: 0x41
	TEST(CPU, LD_B_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x41);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, C
	// Opcode: 0x51
	TEST(CPU, LD_D_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x51);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, C
	// Opcode: 0x61
	TEST(CPU, LD_H_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x61);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), C
	// Opcode: 0x71
	TEST(CPU, LD_HL_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x71);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD (BC), A
	// Opcode: 0x02
	TEST(CPU, LD_BC_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x02);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetData(), value);
	}

	// LD (DE), A
	// Opcode: 0x12
	TEST(CPU, LD_DE_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x12);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetData(), value);
	}

	// LD (HL+), A
	// Opcode: 0x22
	TEST(CPU, LD_HL_INC_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x22);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value + 1);
	}

	// LD (HL-), A
	// Opcode: 0x32
	TEST(CPU, LD_HL_DEC_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x32);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value - 1);
	}

	// LD B, D
	// Opcode: 0x42
	TEST(CPU, LD_D_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x42);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD H, D
	// Opcode: 0x62
	TEST(CPU, LD_H_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x62);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), D
	// Opcode: 0x72
	TEST(CPU, LD_HL_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x72);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD (FF00 + C), A
	// Opcode: 0xE2
	TEST(CPU, LD_FF00_C_A)
	{
		uint8_t value = 7;
		uint8_t address = 200;
		auto mmu = CreatePresetMemoryManagementUnit(0xE2);
		mmu.SetByte(1, address);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(address);
		processor.Cycle();

		uint8_t data = mmu.GetByte(0xFF00 | address);

		EXPECT_EQ(data, value);
	}

	// LD A, (FF00 + C)
	// Opcode: 0xF2
	TEST(CPU, LD_A_FF00_C)
	{
		uint8_t value = 7;
		uint8_t address = 10;
		auto mmu = CreatePresetMemoryManagementUnit(0xF2);
		mmu.SetByte(0xFF00 | address, 7);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(address);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD B, E
	// Opcode: 0x43
	TEST(CPU, LD_B_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x43);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, E
	// Opcode: 0x53
	TEST(CPU, LD_D_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x53);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, E
	// Opcode: 0x63
	TEST(CPU, LD_H_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x63);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), E
	// Opcode: 0x73
	TEST(CPU, LD_HL_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x73);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD B, H
	// Opcode: 0x44
	TEST(CPU, LD_B_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x44);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, H
	// Opcode: 0x54
	TEST(CPU, LD_D_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x54);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, H
	// Opcode: 0x64
	TEST(CPU, LD_H_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x64);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), H
	// Opcode: 0x74
	TEST(CPU, LD_HL_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x74);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD B, L
	// Opcode: 0x45
	TEST(CPU, LD_B_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x45);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, L
	// Opcode: 0x55
	TEST(CPU, LD_D_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x55);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD, H, L
	// Opcode: 0x65
	TEST(CPU, LD_H_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x65);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), L
	// Opcode: 0x75
	TEST(CPU, LD_HL_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x75);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD B, U8
	// Opcode: 0x06
	TEST(CPU, LD_B_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x06);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, U8
	// Opcode: 0x16
	TEST(CPU, LD_D_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x16);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, U8
	// Opcode: 0x26
	TEST(CPU, LD_H_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x26);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), U8
	// Opcode: 0x36
	TEST(CPU, LD_HL_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x36);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD B, (HL)
	// Opcode: 0x46
	TEST(CPU, LD_B_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x46);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, (HL)
	// Opcode: 0x56
	TEST(CPU, LD_D_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x56);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, (HL)
	// Opcode: 0x66
	TEST(CPU, LD_H_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x66);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD B, A
	// Opcode: 0x47
	TEST(CPU, LD_B_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x47);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetUpperByte(), value);
	}

	// LD D, A
	// Opcode: 0x57
	TEST(CPU, LD_D_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x57);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetUpperByte(), value);
	}

	// LD H, A
	// Opcode: 0x67
	TEST(CPU, LD_H_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x67);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetUpperByte(), value);
	}

	// LD (HL), A
	// Opcode: 0x77
	TEST(CPU, LD_HL_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x77);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value);
	}

	// LD (U16), SP
	// Opcode: 0x08
	TEST(CPU, LD_U16_SP)
	{
		uint16_t value = 270;
		uint16_t address = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0x08);
		mmu.SetByte(1, address & 0x00FF);
		mmu.SetByte(2, address >> 8);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::SP).SetData(value);
		processor.Cycle();

		uint8_t lowerByte = mmu.GetByte(address);

		uint8_t upperByte = mmu.GetByte(address + 1);

		EXPECT_EQ((upperByte << 8) | lowerByte, value);
	}

	// LD C, B
	// Opcode: 0x48
	TEST(CPU, LD_C_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x48);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, B
	// Opcode: 0x58
	TEST(CPU, LD_E_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x58);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, B
	// Opcode: 0x68
	TEST(CPU, LD_L_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x68);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, B
	// Opcode: 0x78
	TEST(CPU, LD_A_B)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x78);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, C
	// Opcode: 0x49
	TEST(CPU, LD_C_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x49);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, C
	// Opcode: 0x59
	TEST(CPU, LD_E_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x59);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, C
	// Opcode: 0x69
	TEST(CPU, LD_L_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x69);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, C
	// Opcode: 0x79
	TEST(CPU, LD_A_C)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x79);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD A, (BC)
	// Opcode: 0x0A
	TEST(CPU, LD_A_BC)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x0A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::BC).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD A, (DE)
	// Opcode: 0x1A
	TEST(CPU, LD_A_DE)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x1A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD A, (HL+)
	// Opcode: 0x2A
	TEST(CPU, LD_A_HL_INC)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x2A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value + 1);
		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD A, (HL-)
	// Opcode: 0x3A
	TEST(CPU, LD_A_HL_DEC)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x3A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetData(), value - 1);
		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, D
	// Opcode: 0x4A
	TEST(CPU, LD_C_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, D
	// Opcode: 0x5A
	TEST(CPU, LD_E_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, D
	// Opcode: 0x6A
	TEST(CPU, LD_L_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, D
	// Opcode: 0x7A
	TEST(CPU, LD_A_D)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7A);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD (U16), A
	// Opcode: 0xEA
	TEST(CPU, LD_U16_A)
	{
		uint16_t value = 7;
		uint16_t address = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0xFA);
		mmu.SetByte(1, address & 0x00FF);
		mmu.SetByte(2, address >> 8);
		mmu.SetByte(address, value);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		uint8_t data = mmu.GetByte(address);

		EXPECT_EQ(data, value);
	}

	// LD A, (U16)
	// Opcode: 0xFA
	TEST(CPU, LD_A_U16)
	{
		uint16_t value = 7;
		uint16_t address = 300;
		auto mmu = CreatePresetMemoryManagementUnit(0xFA);
		mmu.SetByte(1, address & 0x00FF);
		mmu.SetByte(2, address >> 8);
		mmu.SetByte(address, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, E
	// Opcode: 0x4B
	TEST(CPU, LD_C_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4B);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, E
	// Opcode: 0x5B
	TEST(CPU, LD_E_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5B);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, E
	// Opcode: 0x6B
	TEST(CPU, LD_L_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6B);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, E
	// Opcode: 0x7B
	TEST(CPU, LD_A_E)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7B);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::DE).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, H
	// Opcode: 0x4C
	TEST(CPU, LD_C_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4C);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, H
	// Opcode: 0x5C
	TEST(CPU, LD_E_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5C);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, H
	// Opcode: 0x6C
	TEST(CPU, LD_L_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6C);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, H
	// Opcode: 0x7C
	TEST(CPU, LD_A_H)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7C);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, L
	// Opcode: 0x4D
	TEST(CPU, LD_C_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4D);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, L
	// Opcode: 0x5D
	TEST(CPU, LD_E_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5D);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, L
	// Opcode: 0x6D
	TEST(CPU, LD_L_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6D);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, L
	// Opcode: 0x7D
	TEST(CPU, LD_A_L)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7D);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetLowerByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, U8
	// Opcode: 0x0E
	TEST(CPU, LD_C_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x0E);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, U8
	// Opcode: 0x1E
	TEST(CPU, LD_E_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x1E);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, U8
	// Opcode: 0x2E
	TEST(CPU, LD_L_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x2E);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, U8
	// Opcode: 0x3E
	TEST(CPU, LD_A_U8)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x3E);
		mmu.SetByte(1, value);

		auto processor = CPU(mmu);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, (HL)
	// Opcode: 0x4E
	TEST(CPU, LD_C_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4E);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, (HL)
	// Opcode: 0x5E
	TEST(CPU, LD_E_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5E);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, (HL)
	// Opcode: 0x6E
	TEST(CPU, LD_L_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6E);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, (HL)
	// Opcode: 0x7E
	TEST(CPU, LD_A_HL)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7E);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::HL).SetData(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}

	// LD C, A
	// Opcode: 0x4F
	TEST(CPU, LD_C_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x4F);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::BC).GetLowerByte(), value);
	}

	// LD E, A
	// Opcode: 0x5F
	TEST(CPU, LD_E_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x5F);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::DE).GetLowerByte(), value);
	}

	// LD L, A
	// Opcode: 0x6F
	TEST(CPU, LD_L_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x6F);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::HL).GetLowerByte(), value);
	}

	// LD A, A
	// Opcode: 0x7F
	TEST(CPU, LD_A_A)
	{
		uint8_t value = 7;
		auto mmu = CreatePresetMemoryManagementUnit(0x7F);

		auto processor = CPU(mmu);
		processor.GetRegister(CPURegisterID::AF).SetUpperByte(value);
		processor.Cycle();

		EXPECT_EQ(processor.GetRegister(CPURegisterID::AF).GetUpperByte(), value);
	}
}