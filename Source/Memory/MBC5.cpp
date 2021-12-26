#include <sstream>
#include "Memory/MBC5.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const uint16_t ROM_BANK_00_START_ADDR = 0x0000;
	const uint16_t ROM_BANK_00_END_ADDR = 0x3FFF;

	const uint16_t ROM_SWITCHABLE_BANK_START_ADDR = 0x4000;
	const uint16_t ROM_SWITCHABLE_BANK_END_ADDR = 0x7FFF;

	const uint16_t RAM_SWITCHABLE_BANK_START_ADDR = 0xA000;
	const uint16_t RAM_SWITCHABLE_BANK_END_ADDR = 0xBFFF;

	const uint16_t RAM_ENABLE_START_ADDR = 0x0000;
	const uint16_t RAM_ENABLE_END_ADDR = 0x1FFF;

	const uint16_t ROM_BANK_NUMBER_LOWER_START_ADDR = 0x2000;
	const uint16_t ROM_BANK_NUMBER_LOWER_END_ADDR = 0x2FFF;

	const uint16_t ROM_BANK_NUMBER_UPPER_START_ADDR = 0x3000;
	const uint16_t ROM_BANK_NUMBER_UPPER_END_ADDR = 0x3FFF;

	const uint16_t RAM_BANK_NUMBER_START_ADDR = 0x4000;
	const uint16_t RAM_BANK_NUMBER_END_ADDR = 0x5FFF;

	MBC5::MBC5()
	{

	}

	std::string MBC5::GetName() const
	{
		return "MBC5";
	}

	uint8_t MBC5::Read(uint16_t address) const
	{
		if (address >= ROM_BANK_00_START_ADDR && address <= ROM_BANK_00_END_ADDR)
		{
			return ReadFromROM(address);
		}
		else if (address >= ROM_SWITCHABLE_BANK_START_ADDR && address <= ROM_SWITCHABLE_BANK_END_ADDR)
		{
			// The actual ROM address is dependent on the current rom bank number.
			return ReadFromROM((address - ROM_SWITCHABLE_BANK_START_ADDR) + (ROM_BANK_SIZE * romBankNumber));
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr || !isRamEnabled)
				return 0;

			return ReadFromRAM((address - RAM_SWITCHABLE_BANK_START_ADDR) + (RAM_BANK_SIZE * ramBankNumber));
		}

		return 0;
	}

	void MBC5::Write(uint16_t address, uint8_t value)
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
		else if (address >= ROM_BANK_NUMBER_LOWER_START_ADDR && address <= ROM_BANK_NUMBER_LOWER_END_ADDR)
		{
			// The lower 8 bits of the ROM bank number are written to this address range.
			romBankNumber = (romBankNumber & 0x100) | value;
		}
		else if (address >= ROM_BANK_NUMBER_UPPER_START_ADDR && address <= ROM_BANK_NUMBER_UPPER_END_ADDR)
		{
			// The 9th bit of the ROM bank number is written to this address range.
			romBankNumber = (romBankNumber & 0x0FF) | (value & 1) << 8;
		}
		else if (address >= RAM_BANK_NUMBER_START_ADDR && address <= RAM_BANK_NUMBER_END_ADDR)
		{
			// Writing a value between $00 and $0F selects the corresponding RAM bank.
			if (value <= 0x0F)
				ramBankNumber = value;
		}
	}

	void MBC5::Reset()
	{
		MemoryBankController::Reset();

		romBankNumber = 0;
		ramBankNumber = 0;
	}
}