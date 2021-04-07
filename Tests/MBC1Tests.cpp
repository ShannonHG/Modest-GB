#include <vector>
#include "gtest/gtest.h"
#include "Cartridge.hpp"
#include "MBC1.hpp"
#include "MBCTestGlobals.hpp"

namespace SHG
{
	// A byte from the memory bank controller at an address between 0x0000 and 0x3FFF,
	// should be equal to the byte at that same address in the first bank of ROM.
	// This test ensures that this case is true.
	TEST(MBC1, ContainsFirstROMBankInCorrectRange)
	{
		// Generate ROM with bytes filled at the beginning, middle, and end of each ROM bank
		uint32_t offset = (8 * KiB) - 1;
		std::vector<uint8_t> rom = GenerateRandomTestROM(128 * KiB, offset);

		MBC1 mbc;
		mbc.AttachROM(rom);

		// Compare bytes in the first ROM bank, to bytes retrieved from the 
		// memory bank controller in addresses ranges 0x0000 to 0x3FFF

		uint8_t byte1 = 0;
		EXPECT_EQ(mbc.TryGetByte(0, byte1), true);
		EXPECT_EQ(byte1, rom[0]);

		uint8_t byte2 = 0;
		EXPECT_EQ(mbc.TryGetByte(offset, byte2), true);
		EXPECT_EQ(byte2, rom[offset]);

		uint8_t byte3 = 0;
		EXPECT_EQ(mbc.TryGetByte(offset * 2, byte3), true);
		EXPECT_EQ(byte3, rom[offset * 2]);
	}

	TEST(MBC1, SelectsCorrectROMBank)
	{
		// Generate ROM with bytes filled at the beginning, middle, and end of each ROM bank
		uint32_t offset = (8 * KiB) - 1;
		std::vector<uint8_t> rom = GenerateRandomTestROM(128 * KiB, offset);

		uint16_t targetROMBankNum = 6;

		MBC1 mbc;
		mbc.AttachROM(rom);

		// Set ROM bank number
		EXPECT_EQ(mbc.TrySetByte(0x2050, targetROMBankNum), true);

		// Compare bytes in the first ROM bank, to bytes retrieved from the 
		// memory bank controller in addresses ranges 0x0000 to 0x3FFF

		uint32_t virtualAddressRangeStart = 0x4000;
		uint32_t targetVirtualAddress = 0x4000;
		uint32_t physicalAddress = MemoryBankController::CalculatePhysicalROMAddress(targetROMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte1 = 0;
		EXPECT_EQ(mbc.TryGetByte(virtualAddressRangeStart, byte1), true);
		EXPECT_EQ(byte1, rom[physicalAddress]);

		targetVirtualAddress = virtualAddressRangeStart + offset;
		physicalAddress = MemoryBankController::CalculatePhysicalROMAddress(targetROMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte2 = 0;
		EXPECT_EQ(mbc.TryGetByte(targetVirtualAddress, byte2), true);
		EXPECT_EQ(byte2, rom[physicalAddress]);

		targetVirtualAddress = virtualAddressRangeStart + (offset * 2);
		physicalAddress = MemoryBankController::CalculatePhysicalROMAddress(targetROMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte3 = 0;
		EXPECT_EQ(mbc.TryGetByte(targetVirtualAddress, byte3), true);
		EXPECT_EQ(byte3, rom[physicalAddress]);
	}

	TEST(MBC1, SelectsCorrectRAMBank)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);

		// Generate ROM with bytes filled at the beginning, middle, and end of each RAM bank
		uint32_t offset = (4 * KiB) - 1;
		std::vector<uint8_t> ram = GenerateRandomTestRAM(32 * KiB, offset);

		uint16_t targetRAMBankNum = 2;

		MBC1 mbc;
		mbc.AttachROM(rom);
		mbc.AttachRAM(ram);

		// Set RAM bank number
		EXPECT_EQ(mbc.TrySetByte(0x4005, targetRAMBankNum), true);

		// Enable RAM
		EXPECT_EQ(mbc.TrySetByte(0x0000, 0x0A), true);

		// Compare bytes in the first ROM bank, to bytes retrieved from the 
		// memory bank controller in addresses ranges 0x0000 to 0x3FFF

		uint32_t virtualAddressRangeStart = 0xA000;
		uint32_t targetVirtualAddress = 0xA000;
		uint32_t physicalAddress = MemoryBankController::CalculatePhysicalRAMAddress(targetRAMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte1 = 0;
		EXPECT_EQ(mbc.TryGetByte(virtualAddressRangeStart, byte1), true);
		EXPECT_EQ(byte1, ram[physicalAddress]);

		targetVirtualAddress = virtualAddressRangeStart + offset;
		physicalAddress = MemoryBankController::CalculatePhysicalRAMAddress(targetRAMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte2 = 0;
		EXPECT_EQ(mbc.TryGetByte(targetVirtualAddress, byte2), true);
		EXPECT_EQ(byte2, ram[physicalAddress]);

		targetVirtualAddress = virtualAddressRangeStart + (offset * 2);
		physicalAddress = MemoryBankController::CalculatePhysicalRAMAddress(targetRAMBankNum, virtualAddressRangeStart, targetVirtualAddress);

		uint8_t byte3 = 0;
		EXPECT_EQ(mbc.TryGetByte(targetVirtualAddress, byte3), true);
		EXPECT_EQ(byte3, ram[physicalAddress]);
	}

	// If the value 0x0A is written to any address in the range of 0x0000 to 0x1FFF, 
	// then RAM should be enabled
	TEST(MBC1, EnablesRAM)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);
		std::vector<uint8_t> ram = GenerateEmptyTestRAM(32 * KiB);

		MBC1 mbc;
		mbc.AttachROM(rom);
		mbc.AttachRAM(ram);

		EXPECT_EQ(mbc.TrySetByte(0x0001, 0x0A), true);
		EXPECT_EQ(mbc.IsRAMEnabled(), true);
	}

	// If a value other than 0x0A is written to any address in the range of 0x0000 to 0x1FFF, 
	// then RAM should be disabled
	TEST(MBC1, DisablesRAM)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);

		MBC1 mbc;
		mbc.AttachROM(rom);

		EXPECT_EQ(mbc.TrySetByte(0x0001, 0x0D), true);
		EXPECT_EQ(mbc.IsRAMEnabled(), false);
	}

	// This test confirms that writing to the address range 0x6000 - 0x7FFF sets the ROM bank number.
	TEST(MBC1, SetsBankingModeTo0)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);
		std::vector<uint8_t> ram = GenerateEmptyTestRAM(32 * KiB);

		// RAM with 32 KiB of space will only have 2 bits available to address its ROM banks (4 banks).
		// So this number should be less than or equal to 0x03 since that's the maximum value that can be represented with 2 bits.
		uint8_t expectedBankingMode = 0x00;

		MBC1 mbc;
		mbc.AttachROM(rom);
		mbc.AttachRAM(ram);

		EXPECT_EQ(mbc.TrySetByte(0x6112, expectedBankingMode), true);
		EXPECT_EQ(mbc.GetBankingMode() , expectedBankingMode);
	}

	// This test confirms that writing to the address range 0x2000 - 0x3FFF sets the ROM bank number.
	// For ROMs that are 1 MiB or larger, the upper bits of the bank number will be written to a different address space, 
	// so that case is handle in a different test
	TEST(MBC1, SetsROMBankNumberForLessThan1MiB)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);

		// A ROM with 128 KiB will only have 3 bits available to address its ROM banks (8 banks).
		// So this number should be less than or equal to 0x07 since that's the maximum value that can be represented with 3 bits.
		uint8_t expectedBankNum = 0x04;

		MBC1 mbc;
		mbc.AttachROM(rom);

		EXPECT_EQ(mbc.TrySetByte(0x2005, expectedBankNum), true);
		EXPECT_EQ(mbc.GetROMBankNumber(), expectedBankNum);
	}

	// This test confirms that writing to the address range 0x4000 - 0x5FFF sets the RAM bank number.
	// This address space is also used to set the upper bits of the ROM bank number, but that case is handled
	// in a different test.
	TEST(MBC1, SetsRAMBankNumber)
	{
		std::vector<uint8_t> rom = GenerateEmptyTestROM(128 * KiB);
		std::vector<uint8_t> ram = GenerateEmptyTestRAM(32 * KiB);

		// RAM with 32 KiB of space will only have 2 bits available to address its ROM banks (4 banks).
		// So this number should be less than or equal to 0x03 since that's the maximum value that can be represented with 2 bits.
		uint8_t expectedBankNum = 0x01;

		MBC1 mbc;
		mbc.AttachROM(rom);
		mbc.AttachRAM(ram);

		EXPECT_EQ(mbc.TrySetByte(0x4011, expectedBankNum), true);
		EXPECT_EQ(mbc.GetRAMBankNumber(), expectedBankNum);
	}
}