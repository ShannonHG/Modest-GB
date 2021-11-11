#include <array>
#include "gtest/gtest.h"
#include "Memory/Cartridge.hpp"

namespace SHG
{
	// Mock data that will only contain values in the header area (0x100 - 0x14F)
	std::vector<uint8_t> testROMData(32 * KiB);

	TEST(Cartridge, ReadsRAMTypeFromHeader)
	{
		// RAM Size Address = 0x0149
		// 64 KiB RAM Code = 0x05
		testROMData[0x0149] = 0x05; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetRAMSize(), 64 * KiB);
	}

	TEST(Cartridge, ReadsROMTypeFromHeader)
	{
		// ROM Size Address = 0x0148
		// 32 KiB ROM Code = 0x00
		testROMData[0x0148] = 0x00; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetROMSize(), 32 * KiB);
	}

	TEST(Cartridge, ReadsMBC1CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC1 Code = 0x01
		testROMData[0x147] = 0x01; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC1);
	}

	TEST(Cartridge, ReadsMBC2CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC2 Code = 0x05
		testROMData[0x147] = 0x05; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC2);
	}

	TEST(Cartridge, ReadsMBC3CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC3 Code = 0x0F
		testROMData[0x147] = 0x0F; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC3);
	}

	TEST(Cartridge, ReadsMBC5CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC5 Code = 0x19
		testROMData[0x147] = 0x19; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC5);
	}

	TEST(Cartridge, ReadsMBC6CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC6 Code = 0x20
		testROMData[0x147] = 0x20; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC6);
	}

	TEST(Cartridge, ReadsMBC7CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MBC7 Code = 0x22
		testROMData[0x147] = 0x22; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC7);
	}

	TEST(Cartridge, ReadsMMM01CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// MMM01 Code = 0x0B
		testROMData[0x147] = 0x0B; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MMM01);
	}

	TEST(Cartridge, ReadsHuC3CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// HuC3 Code = 0xFE
		testROMData[0x147] = 0xFE; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::HuC3);
	}

	TEST(Cartridge, ReadsHuC1CodeFromHeader)
	{
		// Cartridge Type Address = 0x0147
		// HuC1 Code = 0xFF
		testROMData[0x147] = 0xFF; 

		Cartridge cartridge;
		cartridge.Load(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::HuC1);
	}
}