#include <sstream>
#include <iomanip>
#include "Memory/MemoryBankController.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const uint16_t MemoryBankController::ROM_BANK_SIZE = 16 * KiB;
	const uint16_t MemoryBankController::RAM_BANK_SIZE = 8 * KiB;

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

	void MemoryBankController::WriteTovalidROMAccesMessage(uint16_t address)
	{
		std::stringstream stream;
		stream << "Attempted to access invalid ROM address through " << GetName() << ". Address: " << std::hex << std::setfill('0') << std::setw(4) << address << " | ROM size: " << rom->size() / KiB << " KiB";
		Logger::WriteError(stream.str());
	}

	void MemoryBankController::WriteTovalidRAMAccesMessage(uint16_t address)
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

	void MemoryBankController::WriteToRAM(uint32_t address, uint8_t value)
	{
		if (ram == nullptr)
		{
			WriteMissingRAMMessage();
			return;
		}

		if (address >= ram->size())
		{
			WriteTovalidRAMAccesMessage(address);
			return;
		}

		(*ram)[address] = value;
	}

	uint8_t MemoryBankController::ReadFromRAM(uint32_t address)
	{
		if (ram == NULL)
		{
			WriteMissingRAMMessage();
			return false;
		}

		if (address >= ram->size())
		{
			WriteTovalidRAMAccesMessage(address);
			return false;
		}

		return (*ram)[address];
	}

	void MemoryBankController::WriteToROM(uint32_t address, uint8_t value)
	{
		if (rom == nullptr)
		{
			WriteMissingROMMessage();
			return;
		}

		if (address >= rom->size())
		{
			WriteTovalidROMAccesMessage(address);
			return;
		}

		(*rom)[address] = value;
	}

	uint8_t MemoryBankController::ReadFromROM(uint32_t address)
	{
		// TODO: This should probably throw an exception

		if (rom == nullptr)
		{
			WriteMissingROMMessage();
			return 0;
		}

		if (address >= rom->size())
		{
			WriteTovalidROMAccesMessage(address);
			return 0;
		}

		return (*rom)[address];
	}

	void MemoryBankController::Reset()
	{
		if (ram != nullptr) 
			ram->clear();

		if (rom != nullptr) 
			rom->clear();
	}
}