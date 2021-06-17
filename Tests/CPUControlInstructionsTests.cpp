#include "gtest/gtest.h"
#include "CPU/CPU.hpp"
#include "CPUTestGlobals.hpp"

namespace SHG
{
	// NOP
	// Opcode: 0x00
	TEST(CPUControlInstructions, NOP)
	{
		GTEST_FAIL();
	}

	// STOP
	// Opcode: 0x10
	TEST(CPUControlInstructions, STOP)
	{
		GTEST_FAIL();
	}

	// HALT
	// Opcode: 0x76
	TEST(CPUControlInstructions, HALT)
	{
		GTEST_FAIL();
	}

	// DI
	// Opcode: 0xF3
	TEST(CPUControlInstructions, DI)
	{
		GTEST_FAIL();
	}

	// EI
	// Opcode: 0xFB
	TEST(CPUControlInstructions, EI)
	{
		GTEST_FAIL();
	}
}