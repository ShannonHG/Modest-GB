#include <sstream>
#include <iomanip>
#include "MemoryBankController.hpp"
#include "Logger.hpp"
#include "Globals.hpp"

namespace SHG
{
	void MemoryBankController::AttachRAM(std::vector<uint8_t>& ram)
	{
		this->ram = &ram;
	}

	void MemoryBankController::AttachROM(std::vector<uint8_t>& rom)
	{
		this->rom = &rom;
	}

	void MemoryBankController::WriteMissingROMMessage()
	{
		Logger::WriteError("Attempted to access ROM data through " + GetName() + " but no ROM is attached");
	}


	void MemoryBankController::WriteMissingRAMMessage()
	{
		Logger::WriteError("Attempted to access RAM data through " + GetName() + " but no RAM is attached");
	}

	void MemoryBankController::WriteInvalidROMAccesMessage(uint16_t address)
	{
		std::stringstream stream;
		stream << "Attempted to access invalid ROM address through " << GetName() << ". Address: " << std::hex << std::setfill('0') << std::setw(4) << address << " | ROM size: " << rom->size() / KiB << " KiB";
		Logger::WriteError(stream.str());
	}

	void MemoryBankController::WriteInvalidRAMAccesMessage(uint16_t address)
	{
		std::stringstream stream;
		stream << "Attempted to access invalid RAM address through " << GetName() << ". Address: " << std::hex << std::setfill('0') << std::setw(4) << address << " | ROM size: " << rom->size() / KiB << " KiB";
		Logger::WriteError(stream.str());
	}

	uint32_t MemoryBankController::CalculatePhysicalROMAddress(uint16_t romBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress)
	{
		uint32_t romBankOffset = romBankNumber * ROM_BANK_SIZE;
		return romBankOffset + (targetVirtualAddress - (virtualAddressRangeStart + romBankOffset));
	}

	uint32_t MemoryBankController::CalculatePhysicalRAMAddress(uint8_t ramBankNumber, uint16_t virtualAddressRangeStart, uint16_t targetVirtualAddress)
	{
		uint32_t ramBankOffset = ramBankNumber * RAM_BANK_SIZE;
		return ramBankOffset + (targetVirtualAddress - (virtualAddressRangeStart + ramBankOffset));
	}

	bool MemoryBankController::TrySetByteInRAM(uint32_t address, uint8_t value)
	{
		if (ram == NULL)
		{
			WriteMissingRAMMessage();
			return false;
		}

		if (address >= ram->size())
		{
			WriteInvalidRAMAccesMessage(address);
			return false;
		}

		(*ram)[address] = value;
		return true;
	}

	bool MemoryBankController::TryGetByteFromRAM(uint32_t address, uint8_t& outValue)
	{
		if (ram == NULL)
		{
			WriteMissingRAMMessage();
			return false;
		}

		if (address >= ram->size())
		{
			WriteInvalidRAMAccesMessage(address);
			return false;
		}

		outValue = (*ram)[address];
		return true;
	}

	bool MemoryBankController::TrySetByteInROM(uint32_t address, uint8_t value)
	{
		if (rom == NULL)
		{
			WriteMissingROMMessage();
			return false;
		}

		if (address >= rom->size())
		{
			WriteInvalidROMAccesMessage(address);
			return false;
		}

		(*rom)[address] = value;
		return true;
	}

	bool MemoryBankController::TryGetByteFromROM(uint32_t address, uint8_t& outValue)
	{
		if (rom == NULL)
		{
			WriteMissingROMMessage();
			return false;
		}

		if (address >= rom->size())
		{
			WriteInvalidROMAccesMessage(address);
			return false;
		}

		outValue = (*rom)[address];
		return true;
	}
}