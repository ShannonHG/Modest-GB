#include <sstream>
#include "Memory/MBC1.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const uint16_t ROM_BANK_X0_START_ADDR = 0x0000;
	const uint16_t ROM_BANK_X0_END_ADDR = 0x3FFF;

	const uint16_t ROM_SWITCHABLE_BANK_START_ADDR = 0x4000;
	const uint16_t ROM_SWITCHABLE_BANK_END_ADDR = 0x7FFF;

	const uint16_t RAM_SWITCHABLE_BANK_START_ADDR = 0xA000;
	const uint16_t RAM_SWITCHABLE_BANK_END_ADDR = 0xBFFF;

	const uint16_t RAM_ENABLE_START_ADDR = 0x0000;
	const uint16_t RAM_ENABLE_END_ADDR = 0x1FFF;

	const uint16_t ROM_BANK_NUMBER_START_ADDR = 0x2000;
	const uint16_t ROM_BANK_NUMBER_END_ADDR = 0x3FFF;

	const uint16_t RAM_BANK_NUMBER_START_ADDR = 0x4000;
	const uint16_t RAM_BANK_NUMBER_END_ADDR = 0x5FFF;

	const uint16_t BANK_MODE_SELECT_START_ADDR = 0x6000;
	const uint16_t BANK_MODE_SELECT_END_ADDR = 0x7FFF;

	// TODO: Does not handle 1MB multi-game carts.
	MBC1::MBC1()
	{

	}

	std::string MBC1::GetName() const
	{
		return "MBC1";
	}

	uint8_t MBC1::Read(uint16_t address) const
	{
		if (address >= ROM_BANK_X0_START_ADDR && address <= ROM_BANK_X0_END_ADDR)
		{
			// For 1 MiB cartridges, the RAM bank number is used as the upper 2 bits of the ROM bank number.
			uint8_t selectedROMBank = GetAdjustedROMBankNumber(0);

			return ReadFromROM(address + (ROM_BANK_SIZE * selectedROMBank));
		}
		else if (address >= ROM_SWITCHABLE_BANK_START_ADDR && address <= ROM_SWITCHABLE_BANK_END_ADDR)
		{
			// For 1 MiB cartridges, the RAM bank number is used as the upper 2 bits of the ROM bank number.
			uint8_t selectedROMBank = GetAdjustedROMBankNumber(romBankNumber);

			// The actual ROM address is dependent on the current rom bank number.
			return ReadFromROM((address - ROM_SWITCHABLE_BANK_START_ADDR) + (ROM_BANK_SIZE * selectedROMBank));
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRamEnabled)
				return 0;

			return ReadFromRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + (RAM_BANK_SIZE * ramBankNumber));
		}

		return 0;
	}

	void MBC1::Write(uint16_t address, uint8_t value)
	{
		if (address >= RAM_ENABLE_START_ADDR && address <= RAM_ENABLE_END_ADDR)
		{
			// Writing a value with $0A in the lower nibble will enable RAM, any other value will disable RAM.
			isRamEnabled = ((value & 0b1111) == 0x0A);
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRamEnabled)
				return;

			WriteToRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + (RAM_BANK_SIZE * ramBankNumber), value);
		}
		else if (address >= ROM_BANK_NUMBER_START_ADDR && address <= ROM_BANK_NUMBER_END_ADDR)
		{
			// Determine the number of bits required to address all of the ROM banks. 
			// For example, a 256 KiB cartridge would require 4 bits to address all 16 of its banks.
			uint8_t numBitsRequired = std::log2(rom->size() / ROM_BANK_SIZE);

			uint8_t bitMask = std::pow(2, numBitsRequired) - 1;
			romBankNumber = value & bitMask;

			// When 0 is written to this address space, the ROM bank number should be incremented to 1 
			// since ROM bank 0 cannot be selected.
			if (romBankNumber == 0)
				romBankNumber++;
		}
		else if (address >= RAM_BANK_NUMBER_START_ADDR && address <= RAM_BANK_NUMBER_END_ADDR)
		{
			// The RAM bank register is only 2 bits wide.
			ramBankNumber = value & 0b11;
		}
		else if (address >= BANK_MODE_SELECT_START_ADDR && address <= BANK_MODE_SELECT_END_ADDR)
		{
			// The banking mode select register is only 1 bit wide.
			isSimpleBankingModeEnabled = value & 1;
		}
	}

	uint8_t MBC1::GetAdjustedROMBankNumber(uint8_t romBankNumber) const
	{
		return isSimpleBankingModeEnabled ? romBankNumber : romBankNumber | (ramBankNumber << 5);
	}

	void MBC1::Reset()
	{
		MemoryBankController::Reset();

		// The ROM bank number defaults to 1;
		romBankNumber = 1;
		ramBankNumber = 0;
		isSimpleBankingModeEnabled = false;
	}
}