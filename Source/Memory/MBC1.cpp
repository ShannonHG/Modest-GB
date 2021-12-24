#include <sstream>
#include "Memory/MBC1.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	// Address range is read only
	const uint16_t ROM_BANK_X0_START_ADDR = 0x0000;
	const uint16_t ROM_BANK_X0_END_ADDR = 0x3FFF;

	// Address range is read only
	const uint16_t ROM_SWITCHABLE_BANK_START_ADDR = 0x4000;
	const uint16_t ROM_SWITCHABLE_BANK_END_ADDR = 0x7FFF;

	// Address range is both readable and writable
	const uint16_t RAM_SWITCHABLE_BANK_START_ADDR = 0xA000;
	const uint16_t RAM_SWITCHABLE_BANK_END_ADDR = 0xBFFF;

	// Address range is write only
	const uint16_t RAM_ENABLE_START_ADDR = 0x0000;
	const uint16_t RAM_ENABLE_END_ADDR = 0x1FFF;

	// Address range is write only
	const uint16_t ROM_BANK_NUMBER_START_ADDR = 0x2000;
	const uint16_t ROM_BANK_NUMBER_END_ADDR = 0x3FFF;

	// Address range is write only
	const uint16_t RAM_BANK_NUMBER_START_ADDR = 0x4000;
	const uint16_t RAM_BANK_NUMBER_END_ADDR = 0x5FFF;

	// Address range is write only
	const uint16_t BANK_MODE_SELECT_START_ADDR = 0x6000;
	const uint16_t BANK_MODE_SELECT_END_ADDR = 0x7FFF;

	// Address to 4-bit register containing the RAM enable flag
	const uint8_t RAM_ENABLE_REGISTER_ADDR = 0;

	// Address to 5-bit register containing the ROM bank number
	const uint8_t ROM_BANK_NUM_REGISTER_ADDR = 1;

	// Address to 2-bit register containing the RAM bank number,
	// or upper bits of ROM bank number
	const uint8_t SECONDARY_BANK_REGISTER_ADDR = 2;

	// Address to 1-bit register containing the current banking mode
	const uint8_t BANK_MODE_SELECT_REGISTER_ADDR = 3;

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
			if (isSimpleBankingModeEnabled)
			{
				//Logger::WriteInfo("Read from ROM bank 0 region", GetMessageHeader());
				return ReadFromROM(address);
			}
			else
			{
				// TODO: Add logic for advanced banking mode
				Logger::WriteInfo("Read from ROM bank 0 region without simple banking", GetMessageHeader());
				return 0;
			}
		}
		else if (address >= ROM_SWITCHABLE_BANK_START_ADDR && address <= ROM_SWITCHABLE_BANK_END_ADDR)
		{
			// The actual ROM address is dependent on the current rom bank number.
			return ReadFromROM(address + (ROM_BANK_SIZE * (romBankNumber - 1)));
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			//Logger::WriteInfo("Read from switchable RAM region", GetMessageHeader());
			if (ram == nullptr || !isRamEnabled)
				return 0;

			// TODO: Add logic for reading from RAM
		}
		else
		{
			Logger::WriteInfo("Read from other region: " + GetHexString16(address));
		}
		return 0;
	}

	void MBC1::Write(uint16_t address, uint8_t value)
	{
		if (address >= RAM_ENABLE_START_ADDR && address <= RAM_ENABLE_END_ADDR)
		{
			//Logger::WriteInfo("Write to RAM enable region", GetMessageHeader());
			// Writing a value with $0A in the lower nibble will enable RAM, any other value will disable RAM.
			isRamEnabled = ((value & 0b1111) == 0x0A);
		}
		else if (address >= RAM_SWITCHABLE_BANK_START_ADDR && address <= RAM_SWITCHABLE_BANK_END_ADDR)
		{
			if (ram == nullptr)
				return;

			// TODO: Add logic for writing to RAM

			//Logger::WriteInfo("Write to other region: " + GetHexString16(address));
		}
		else if (address >= ROM_BANK_NUMBER_START_ADDR && address <= ROM_BANK_NUMBER_END_ADDR)
		{
			// TODO: Add logic for larger cartridges
			if (rom->size() >= MiB)
			{
				Logger::WriteError("Large cart detected (greater than 1 MiB). ROM bank number selection logic will not work properly.");
				return;
			}

			// Determine the number of bits required to address all of the ROM banks. 
			// For example, a 256 KiB cartridge would require 4 bits to address all 16 of its banks.
			uint8_t numBitsRequired = std::log2(rom->size() / ROM_BANK_SIZE);

			uint8_t bitMask = std::pow(2, numBitsRequired) - 1;
			romBankNumber = value & bitMask;
		}
		else if (address >= RAM_BANK_NUMBER_START_ADDR && address <= RAM_BANK_NUMBER_END_ADDR)
		{
			// TODO: Add logic for larger cartridges
			if (rom->size() >= MiB)
			{
				Logger::WriteError("Large cart detected (greater than 1 MiB). RAM bank number selection logic will not work properly.");
				return;
			}

			// The RAM bank register only selects a RAM bank on 32 KiB RAM carts.
			if (ram == nullptr || ram->size() != 32 * KiB)
				return;

			// The RAM bank register is only 2 bits wide.
			ramBankNumber = value & 0b11;
		}
		else
		{
			Logger::WriteInfo("Write to other region: " + GetHexString16(address));
		}
	}

	bool MBC1::IsAddressAvailable(uint16_t address) const
	{
		// TODO: Revisit
		return true;
	}
}