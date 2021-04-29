#include "CPU/CPU.hpp"
#include "gtest/gtest.h"

namespace SHG
{
	// LD B, B
	// Opcode: 0x40
	TEST(CPU, LD_B_B)
	{
		GTEST_FAIL();
	}

	// LD D, B
	// Opcode: 0x50
	TEST(CPU, LD_D_B)
	{
		GTEST_FAIL();
	}

	// LD H, B
	// Opcode: 0x60
	TEST(CPU, LD_H_B)
	{
		GTEST_FAIL();
	}

	// LD (HL), B
	// Opcode: 0x70
	TEST(CPU, LD_HL_B)
	{
		GTEST_FAIL();
	}

	// LD (FF00 + U8), A
	// Opcode: 0xE0
	TEST(CPU, LD_FFOO_U8_A)
	{
		GTEST_FAIL();
	}

	// LD A, (FF00 + U8)
	// Opcode: 0xF0
	TEST(CPU, LD_A_FF00_U8)
	{
		GTEST_FAIL();
	}

	// LD BC, U16
	// Opcode: 0x01
	TEST(CPU, LD_BC_U16)
	{
		GTEST_FAIL();
	}

	// LD DE, U16
	// Opcode: 0x11
	TEST(CPU, LD_DE_U16)
	{
		GTEST_FAIL();
	}

	// LD HL, U16
	// Opcode: 0x21
	TEST(CPU, LD_HL_U16)
	{
		GTEST_FAIL();
	}

	// LD SP, U16
	// Opcode: 0x31
	TEST(CPU, LD_SP_U16)
	{
		GTEST_FAIL();
	}

	// LD B, C
	// Opcode: 0x41
	TEST(CPU, LD_B_C)
	{
		GTEST_FAIL();
	}

	// LD D, C
	// Opcode: 0x51
	TEST(CPU, LD_D_C)
	{
		GTEST_FAIL();
	}

	// LD H, C
	// Opcode: 0x61
	TEST(CPU, LD_H_C)
	{
		GTEST_FAIL();
	}

	// LD (HL), C
	// Opcode: 0x71
	TEST(CPU, LD_HL_C)
	{
		GTEST_FAIL();
	}

	// LD (BC), A
	// Opcode: 0x02
	TEST(CPU, LD_BC_A)
	{
		GTEST_FAIL();
	}

	// LD (DE), A
	// Opcode: 0x12
	TEST(CPU, LD_DE_A)
	{
		GTEST_FAIL();
	}

	// LD (HL+), A
	// Opcode: 0x22
	TEST(CPU, LD_HL_INC_A)
	{
		GTEST_FAIL();
	}

	// LD (HL-), A
	// Opcode: 0x32
	TEST(CPU, LD_HL_DEC_A)
	{
		GTEST_FAIL();
	}

	// LD B, D
	// Opcode: 0x42
	TEST(CPU, LD_D_D)
	{
		GTEST_FAIL();
	}

	// LD H, D
	// Opcode: 0x62
	TEST(CPU, LD_H_D)
	{
		GTEST_FAIL();
	}

	// LD (HL), D
	// Opcode: 0x72
	TEST(CPU, LD_HL_D)
	{
		GTEST_FAIL();
	}

	// LD (FF00 + C), A
	// Opcode: 0xE2
	TEST(CPU, LD_FF00_C_A)
	{
		GTEST_FAIL();
	}

	// LD A, (FF00 + C)
	// Opcode: 0xF2
	TEST(CPU, LD_A_FF00_C)
	{
		GTEST_FAIL();
	}

	// LD B, E
	// Opcode: 0x43
	TEST(CPU, LD_B_E)
	{
		GTEST_FAIL();
	}

	// LD D, E
	// Opcode: 0x53
	TEST(CPU, LD_D_E)
	{
		GTEST_FAIL();
	}

	// LD H, E
	// Opcode: 0x63
	TEST(CPU, LD_H_E)
	{
		GTEST_FAIL();
	}

	// LD (HL), E
	// Opcode: 0x73
	TEST(CPU, LD_HL_E)
	{
		GTEST_FAIL();
	}

	// LD B, H
	// Opcode: 0x44
	TEST(CPU, LD_B_H)
	{
		GTEST_FAIL();
	}

	// LD D, H
	// Opcode: 0x54
	TEST(CPU, LD_D_H)
	{
		GTEST_FAIL();
	}

	// LD H, H
	// Opcode: 0x64
	TEST(CPU, LD_H_H)
	{
		GTEST_FAIL();
	}

	// LD (HL), H
	// Opcode: 0x74
	TEST(CPU, LD_HL_H)
	{
		GTEST_FAIL();
	}

	// LD B, L
	// Opcode: 0x45
	TEST(CPU, LD_B_L)
	{
		GTEST_FAIL();
	}

	// LD D, L
	// Opcode: 0x55
	TEST(CPU, LD_D_L)
	{
		GTEST_FAIL();
	}

	// LD, H, L
	// Opcode: 0x65
	TEST(CPU, LD_H_L)
	{
		GTEST_FAIL();
	}

	// LD (HL), L
	// Opcode: 0x75
	TEST(CPU, LD_HL_L)
	{
		GTEST_FAIL();
	}

	// LD B, U8
	// Opcode: 0x06
	TEST(CPU, LD_B_U8)
	{
		GTEST_FAIL();
	}

	// LD D, U8
	// Opcode: 0x16
	TEST(CPU, LD_D_U8)
	{
		GTEST_FAIL();
	}

	// LD H, U8
	// Opcode: 0x26
	TEST(CPU, LD_H_U8)
	{
		GTEST_FAIL();
	}

	// LD (HL), U8
	// Opcode: 0x36
	TEST(CPU, LD_HL_U8)
	{
		GTEST_FAIL();
	}

	// LD B, (HL)
	// Opcode: 0x46
	TEST(CPU, LD_B_HL)
	{
		GTEST_FAIL();
	}

	// LD D, (HL)
	// Opcode: 0x56
	TEST(CPU, LD_D_HL)
	{
		GTEST_FAIL();
	}

	// LD H, (HL)
	// Opcode: 0x66
	TEST(CPU, LD_H_HL)
	{
		GTEST_FAIL();
	}

	// LD B, A
	// Opcode: 0x47
	TEST(CPU, LD_B_A)
	{
		GTEST_FAIL();
	}

	// LD D, A
	// Opcode: 0x57
	TEST(CPU, LD_D_A)
	{
		GTEST_FAIL();
	}

	// LD H, A
	// Opcode: 0x67
	TEST(CPU, LD_H_A)
	{
		GTEST_FAIL();
	}

	// LD (HL), A
	// Opcode: 0x77
	TEST(CPU, LD_HL_A)
	{
		GTEST_FAIL();
	}

	// LD (U16), SP
	// Opcode: 0x08
	TEST(CPU, LD_U16_SP)
	{
		GTEST_FAIL();
	}

	// LD C, B
	// Opcode: 0x48
	TEST(CPU, LD_C_B)
	{
		GTEST_FAIL();
	}

	// LD E, B
	// Opcode: 0x58
	TEST(CPU, LD_E_B)
	{
		GTEST_FAIL();
	}

	// LD L, B
	// Opcode: 0x68
	TEST(CPU, LD_L_B)
	{
		GTEST_FAIL();
	}

	// LD A, B
	// Opcode: 0x78
	TEST(CPU, LD_A_B)
	{
		GTEST_FAIL();
	}

	// LD C, C
	// Opcode: 0x49
	TEST(CPU, LD_C_C)
	{
		GTEST_FAIL();
	}

	// LD E, C
	// Opcode: 0x59
	TEST(CPU, LD_E_C)
	{
		GTEST_FAIL();
	}

	// LD L, C
	// Opcode: 0x69
	TEST(CPU, LD_L_C)
	{
		GTEST_FAIL();
	}

	// LD A, C
	// Opcode: 0x79
	TEST(CPU, LD_A_C)
	{
		GTEST_FAIL();
	}

	// LD A, (BC)
	// Opcode: 0x0A
	TEST(CPU, LD_A_BC)
	{
		GTEST_FAIL();
	}

	// LD A, (DE)
	// Opcode: 0x1A
	TEST(CPU, LD_A_DE)
	{
		GTEST_FAIL();
	}

	// LD A, (HL+)
	// Opcode: 0x2A
	TEST(CPU, LD_A_HL_INC)
	{
		GTEST_FAIL();
	}

	// LD A, (HL-)
	// Opcode: 0x3A
	TEST(CPU, LD_A_HL_DEC)
	{
		GTEST_FAIL();
	}

	// LD C, D
	// Opcode: 0x4A
	TEST(CPU, LD_C_D)
	{
		GTEST_FAIL();
	}

	// LD E, D
	// Opcode: 0x5A
	TEST(CPU, LD_E_D)
	{
		GTEST_FAIL();
	}

	// LD L, D
	// Opcode: 0x6A
	TEST(CPU, LD_L_D)
	{
		GTEST_FAIL();
	}

	// LD A, D
	// Opcode: 0x7A
	TEST(CPU, LD_A_D)
	{
		GTEST_FAIL();
	}

	// LD (U16), A
	// Opcode: 0xEA
	TEST(CPU, LD_U16_A)
	{
		GTEST_FAIL();
	}

	// LD A, (U16)
	// Opcode: 0xFA
	TEST(CPU, LD_A_U16)
	{
		GTEST_FAIL();
	}

	// LD C, E
	// Opcode: 0x4B
	TEST(CPU, LD_C_E)
	{
		GTEST_FAIL();
	}

	// LD_E_E
	// Opcode: 0x5B
	TEST(CPU, LD_E_E)
	{
		GTEST_FAIL();
	}

	// LD L, E
	// Opcode: 0x6B
	TEST(CPU, LD_L_E)
	{
		GTEST_FAIL();
	}

	// LD A, E
	// Opcode: 0x7B
	TEST(CPU, LD_A_E)
	{
		GTEST_FAIL();
	}

	// LD C, H
	// Opcode: 0x4C
	TEST(CPU, LD_C_H)
	{
		GTEST_FAIL();
	}

	// LD E, H
	// Opcode: 0x5C
	TEST(CPU, LD_E_H)
	{
		GTEST_FAIL();
	}

	// LD L, H
	// Opcode: 0x6C
	TEST(CPU, LD_L_H)
	{
		GTEST_FAIL();
	}

	// LD A, H
	// Opcode: 0x7C
	TEST(CPU, LD_A_H)
	{
		GTEST_FAIL();
	}

	// LD C, L
	// Opcode: 0x4D
	TEST(CPU, LD_C_L)
	{
		GTEST_FAIL();
	}

	// LD E, L
	// Opcode: 0x5D
	TEST(CPU, LD_E_L)
	{
		GTEST_FAIL();
	}

	// LD L, L
	// Opcode: 0x6D
	TEST(CPU, LD_L_L)
	{
		GTEST_FAIL();
	}

	// LD A, L
	// Opcode: 0x7D
	TEST(CPU, LD_A_L)
	{
		GTEST_FAIL();
	}

	// LD C, U8
	// Opcode: 0x0E
	TEST(CPU, LD_C_U8)
	{
		GTEST_FAIL();
	}

	// LD E, U8
	// Opcode: 0x1E
	TEST(CPU, LD_E_U8)
	{
		GTEST_FAIL();
	}

	// LD L, U8
	// Opcode: 0x2E
	TEST(CPU, LD_L_U8)
	{
		GTEST_FAIL();
	}

	// LD A, U8
	// Opcode: 0x3E
	TEST(CPU, LD_A_U8)
	{
		GTEST_FAIL();
	}

	// LD C, (HL)
	// Opcode: 0x4E
	TEST(CPU, LD_C_HL)
	{
		GTEST_FAIL();
	}

	// LD E, (HL)
	// Opcode: 0x5E
	TEST(CPU, LD_E_HL)
	{
		GTEST_FAIL();
	}

	// LD L, (HL)
	// Opcode: 0x6E
	TEST(CPU, LD_L_HL)
	{
		GTEST_FAIL();
	}

	// LD A, (HL)
	// Opcode: 0x7E
	TEST(CPU, LD_A_HL)
	{
		GTEST_FAIL();
	}

	// LD C, A
	// Opcode: 0x4F
	TEST(CPU, LD_C_A)
	{
		GTEST_FAIL();
	}

	// LD E, A
	// Opcode: 0x5F
	TEST(CPU, LD_E_A)
	{
		GTEST_FAIL();
	}

	// LD L, A
	// Opcode: 0x6F
	TEST(CPU, LD_L_A)
	{
		GTEST_FAIL();
	}

	// LD A, A
	// Opcode: 0x7F
	TEST(CPU, LD_A_A)
	{
		GTEST_FAIL();
	}
}