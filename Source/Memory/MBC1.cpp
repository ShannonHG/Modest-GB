#include <sstream>
#include "Memory/MBC1.hpp"
#include "Logger.hpp"
#include "Globals.hpp"

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

	std::string MBC1::GetName()
	{
		return "MBC1";
	}

	uint8_t MBC1::GetByte(uint16_t address)
	{
		if (address <= ROM_BANK_X0_END_ADDR && address >= ROM_BANK_X0_START_ADDR)
		{
			return GetByteFromROM(address);
		}
		else if (address <= ROM_SWITCHABLE_BANK_END_ADDR && address >= ROM_SWITCHABLE_BANK_START_ADDR)
		{
			/*uint16_t offset = CalculatePhysicalROMAddress(GetROMBankNumber(), ROM_SWITCHABLE_BANK_START_ADDR, address);*/

			return GetByteFromROM(address);
		}
		else if (address <= RAM_SWITCHABLE_BANK_END_ADDR && address >= RAM_SWITCHABLE_BANK_START_ADDR)
		{
			if (!IsRAMEnabled()) return false;
			else
			{
				uint16_t offset = CalculatePhysicalRAMAddress(GetRAMBankNumber(), RAM_SWITCHABLE_BANK_START_ADDR, address);

				return GetByteFromRAM(offset);
			}
		}

		return true;
	}

	void MBC1::SetByte(uint16_t address, uint8_t value)
	{
		if (address <= RAM_ENABLE_END_ADDR && address >= RAM_ENABLE_START_ADDR)
		{
			// Only the lower 4 bits matter
			value &= 0b1111;

			// RAM is enabled if 0x0A is written to this address range, otherwise RAM is disabled
			switch (value)
			{
			case 0x0A:
				registers[RAM_ENABLE_REGISTER_ADDR] = 1;
				break;
			default:
				registers[RAM_ENABLE_REGISTER_ADDR] = 0;
				break;
			}
		}
		else if (address <= ROM_BANK_NUMBER_END_ADDR && address >= ROM_BANK_NUMBER_START_ADDR)
		{
			uint8_t bitMask = 0;

			// MBC1 automatically translates 0x00 to 0x01
			if (value == 0)
			{
				registers[ROM_BANK_NUM_REGISTER_ADDR] = 1;
			}
			// if the ROM size is less than 1 MiB, then mask the bank number to the required number of bits,
			// otherwise, the upper bits will be specified in a secondary register
			else if (rom != NULL && rom->size() < MiB)
			{
				// Determine the number of bits required to represent addresses for all of the available banks 
				uint8_t bitsRequired = log2(rom->size() / ROM_BANK_SIZE);

				// Create a bit mask that has each one of the required bits enabled
				for (int i = 0; i < bitsRequired; i++) bitMask |= (1 << i);
			}
			else
			{
				// Default to 5 bits and let the upper bits be specified in a secondary register.
				// This is to emulate the 5 bit register that's used in the Game Boy
				bitMask = 0b00011111;
			}

			// Ignore any bits that are not enabled in the bitmask
			registers[ROM_BANK_NUM_REGISTER_ADDR] = value & bitMask;
		}
		else if (address <= RAM_BANK_NUMBER_END_ADDR && address >= RAM_BANK_NUMBER_START_ADDR)
		{
			// Only the 2 least significant bits matter
			value &= 0b11;

			// If the attached ROM is 1 MiB or larger, provided value should be used to set 
			// 5-6 of the ROM bank number
			if (rom != NULL && rom->size() >= MiB)
			{
				registers[SECONDARY_BANK_REGISTER_ADDR] = value;
			}
			else if (ram != NULL && ram->size() >= 32 * KiB)
			{
				registers[SECONDARY_BANK_REGISTER_ADDR] = value;
			}
		}
		else if (address <= RAM_SWITCHABLE_BANK_END_ADDR && address >= RAM_SWITCHABLE_BANK_START_ADDR)
		{
			if (!IsRAMEnabled()) return;
			else
			{
				uint16_t offset = CalculatePhysicalRAMAddress(GetRAMBankNumber(), RAM_SWITCHABLE_BANK_START_ADDR, address);

				return SetByteInRAM(offset, value);
			}
		}
		else if (address <= BANK_MODE_SELECT_END_ADDR && address >= BANK_MODE_SELECT_START_ADDR)
		{
			registers[BANK_MODE_SELECT_REGISTER_ADDR] = value & 1;
		}
	}

	bool MBC1::IsAddressAvailable(uint16_t address)
	{
		return address >= ROM_BANK_X0_START_ADDR && address <= BANK_MODE_SELECT_END_ADDR;
	}

	bool MBC1::IsRAMEnabled()
	{
		return registers[RAM_ENABLE_REGISTER_ADDR] == 1;
	}

	uint8_t MBC1::GetROMBankNumber()
	{
		// If the banking mode is 0, the ROM bank number register defines the full bank number.
		// If the banking mode is 1, the 2 bits in the secondary bank register will be used as bits 
		// 5 and 6 of the ROM bank number, while the lower bits are defined by the ROM bank number register.
		if (GetBankingMode() == 0)
		{
			return registers[ROM_BANK_NUM_REGISTER_ADDR];
		}
		else
		{
			return (registers[SECONDARY_BANK_REGISTER_ADDR] << 5) | registers[ROM_BANK_NUM_REGISTER_ADDR];
		}
	}

	uint8_t MBC1::GetRAMBankNumber()
	{
		return registers[SECONDARY_BANK_REGISTER_ADDR];
	}

	uint8_t MBC1::GetBankingMode()
	{
		return registers[BANK_MODE_SELECT_REGISTER_ADDR];
	}
}