#include <sstream>
#include "Memory/MBC1.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace ModestGB
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
			// If simple ROM banking mode is enabled, then only bank $00 can be accessed here, 
			// otherwise, banks $00, $29, $40, and $60 can be accessed.
			uint8_t selectedROMBank = isSimpleBankingModeEnabled ? 0 : GetAdjustedROMBankNumber(0);
			return ReadFromROM(address + (ROM_BANK_SIZE * selectedROMBank));
		}
		else if (address >= ROM_SWITCHABLE_BANK_START_ADDR && address <= ROM_SWITCHABLE_BANK_END_ADDR)
		{
			// The actual ROM address is dependent on the current rom bank number.
			return ReadFromROM((address - ROM_SWITCHABLE_BANK_START_ADDR) + (ROM_BANK_SIZE * GetAdjustedROMBankNumber(romBankNumber)));
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRamEnabled)
				return 0xFF;

			uint16_t offset = IsRAMBankingEnabled() ? (RAM_BANK_SIZE * ramBankNumber) : 0;
			return ReadFromRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + offset);
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

			uint16_t offset = IsRAMBankingEnabled() ? (RAM_BANK_SIZE * ramBankNumber) : 0;
			WriteToRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + offset, value);
		}
		else if (address >= ROM_BANK_NUMBER_START_ADDR && address <= ROM_BANK_NUMBER_END_ADDR)
		{
			// The ROM bank register is 5 bits wide. When 0b00000 is written to this address space, 
			// the ROM bank number is automatically set to 1.
			romBankNumber = std::max(value & 0b11111, 1);
		}
		else if (address >= RAM_BANK_NUMBER_START_ADDR && address <= RAM_BANK_NUMBER_END_ADDR)
		{
			// The RAM bank register is only 2 bits wide.
			ramBankNumber = value & 0b11;
		}
		else if (address >= BANK_MODE_SELECT_START_ADDR && address <= BANK_MODE_SELECT_END_ADDR)
		{
			// The banking mode select register is only 1 bit wide.
			// Simple ROM banking mode is enabled when a 0 is written, and 
			// RAM banking / Advanced ROM banking mode is enable when a 1 is written.
			isSimpleBankingModeEnabled = (value & 1) == 0;
		}
	}

	uint8_t MBC1::GetAdjustedROMBankNumber(uint8_t romBankNumber) const
	{
		// For 1 MiB or greater cartridges, the RAM bank number is used as the upper 2 bits of the ROM bank number.
		if (rom->size() >= MiB)
			romBankNumber |= (ramBankNumber << 5);

		// Determine the number of bits required to address all of the ROM banks. 
		// For example, a 256 KiB cartridge would require 4 bits to address all 16 of its banks.
		uint8_t numBitsRequired = static_cast<uint8_t>(std::log2(rom->size() / ROM_BANK_SIZE));

		uint8_t bitMask = static_cast<uint8_t>(std::pow(2, numBitsRequired) - 1);
		return romBankNumber &= bitMask;
	}

	bool MBC1::IsRAMBankingEnabled() const
	{
		return ram->size() == 32 * KiB && !isSimpleBankingModeEnabled;
	}

	void MBC1::Reset()
	{
		MemoryBankController::Reset();

		// The ROM bank number defaults to 1;
		romBankNumber = 1;
		ramBankNumber = 0;
		isSimpleBankingModeEnabled = true;
		isRamEnabled = false;
	}
}