#include "MBCTestGlobals.hpp"
#include "Globals.hpp"
#include "Memory/MemoryBankController.hpp"

namespace SHG
{
	std::vector<uint8_t> GenerateRandomTestROM(uint32_t size, uint32_t offset)
	{
		std::vector<uint8_t> rom(size);
		uint8_t randMax = 255;
		uint16_t numOfBanks = size / MemoryBankController::ROM_BANK_SIZE;
		uint16_t addrOffset2 = offset;
		uint16_t addrOffset3 = offset * 2;

		// Store random values in different sections of each bank
		for (int i = 0; i < numOfBanks; i++)
		{
			uint32_t bankStartAddr = i * MemoryBankController::ROM_BANK_SIZE;

			rom[bankStartAddr] = rand() % randMax;
			rom[bankStartAddr + addrOffset2] = rand() % randMax;
			rom[bankStartAddr + addrOffset3] = rand() % randMax;
		}

		return rom;
	}

	std::vector<uint8_t> GenerateRandomTestRAM(uint32_t size, uint32_t offset)
	{
		std::vector<uint8_t> ram(size);
		uint8_t randMax = 255;
		uint16_t numOfBanks = size / MemoryBankController::RAM_BANK_SIZE;
		uint16_t addrOffset2 = offset;
		uint16_t addrOffset3 = offset * 2;

		// Store random values in different sections of each bank
		for (int i = 0; i < numOfBanks; i++)
		{
			uint32_t bankStartAddr = i * MemoryBankController::RAM_BANK_SIZE;

			ram[bankStartAddr] = rand() % randMax;
			ram[bankStartAddr + addrOffset2] = rand() % randMax;
			ram[bankStartAddr + addrOffset3] = rand() % randMax;
		}

		return ram;
	}

	std::vector<uint8_t> GenerateEmptyTestRAM(uint32_t size)
	{
		std::vector<uint8_t> ram(size);
		return ram;
	}

	std::vector<uint8_t> GenerateEmptyTestROM(uint32_t size)
	{
		std::vector<uint8_t> rom(size);
		return rom;
	}
}