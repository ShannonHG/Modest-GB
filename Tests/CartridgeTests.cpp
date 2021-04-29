#include <array>
#include "gtest/gtest.h"
#include "Memory/Cartridge.hpp"

namespace SHG
{
	// Mock data that will only contain values in the header area (0x100 - 0x14F)
	std::vector<uint8_t> testROMData(32 * KiB);

	TEST(Cartridge, ReadsRAMTypeFromHeader)
	{
		testROMData[CH_RAM_SIZE_ADDRESS] = 0x05; // 64 KiB

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetRAMSize(), 64 * KiB);
	}

	TEST(Cartridge, ReadsROMTypeFromHeader)
	{
		testROMData[CH_ROM_SIZE_ADDRESS] = 0x00; // 32 KiB

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetROMSize(), 32 * KiB);
	}

	TEST(Cartridge, ReadsMBC1CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x01; // MBC1

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC1);
	}

	TEST(Cartridge, ReadsMBC2CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x05; // MBC2

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC2);
	}

	TEST(Cartridge, ReadsMBC3CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x0F; // MBC3

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC3);
	}

	TEST(Cartridge, ReadsMBC5CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x19; // MBC5

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC5);
	}

	TEST(Cartridge, ReadsMBC6CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x20; // MBC6

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC6);
	}

	TEST(Cartridge, ReadsMBC7CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x22; // MBC7

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MBC7);
	}

	TEST(Cartridge, ReadsMMM01CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0x0B; // MMM01

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::MMM01);
	}

	TEST(Cartridge, ReadsHuC3CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0xFE; // HuC3

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::HuC3);
	}

	TEST(Cartridge, ReadsHuC1CodeFromHeader)
	{
		testROMData[CH_MEMORY_BANK_TYPE_ADDRESS] = 0xFF; // HuC1

		Cartridge cartridge;
		cartridge.LoadFromMemory(testROMData);

		EXPECT_EQ(cartridge.GetMemoryBankControllerType(), MemoryBankControllerType::HuC1);
	}
}