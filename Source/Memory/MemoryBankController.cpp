#include <sstream>
#include <iomanip>
#include "Memory/MemoryBankController.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace ModestGB
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

	void MemoryBankController::Reset()
	{
		if (ram != nullptr)
			ram->clear();

		if (rom != nullptr)
			rom->clear();
	}

	void MemoryBankController::SetRAMWriteCallback(RAMWriteCallback callback)
	{
		ramWriteCallback = callback;
	}

	void MemoryBankController::PrintMissingROMMessage() const
	{
		Logger::WriteError("Attempted to access ROM data through, but no ROM is attached", GetMessageHeader());
	}

	void MemoryBankController::PrintMissingRAMMessage() const
	{
		Logger::WriteError("Attempted to access RAM data through " + GetName() + " but no RAM is attached", GetMessageHeader());
	}

	void MemoryBankController::PrintInvalidROMAccessMessage(uint32_t address) const
	{
		Logger::WriteError("Attempted to access invalid ROM Address: " + Convert::GetHexString32(address));
	}

	void MemoryBankController::PrintInvalidRAMAccesMessage(uint32_t address) const
	{
		Logger::WriteError("Attempted to access invalid RAM Address: " + Convert::GetHexString32(address));
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
			PrintMissingRAMMessage();
			return;
		}

		if (address >= ram->size())
		{
			PrintInvalidRAMAccesMessage(address);
			return;
		}

		(*ram)[address] = value;

		if (ramWriteCallback)
			ramWriteCallback(address, value);
	}

	uint8_t MemoryBankController::ReadFromRAM(uint32_t address) const
	{
		if (ram == nullptr)
		{
			PrintMissingRAMMessage();
			return false;
		}

		if (address >= ram->size())
		{
			PrintInvalidRAMAccesMessage(address);
			return false;
		}

		return (*ram)[address];
	}

	void MemoryBankController::WriteToROM(uint32_t address, uint8_t value)
	{
		if (rom == nullptr)
		{
			PrintMissingROMMessage();
			return;
		}

		if (address >= rom->size())
		{
			PrintInvalidROMAccessMessage(address);
			return;
		}

		(*rom)[address] = value;
	}

	uint8_t MemoryBankController::ReadFromROM(uint32_t address) const
	{
		if (rom == nullptr)
		{
			PrintMissingROMMessage();
			return 0;
		}

		if (address >= rom->size())
		{
			PrintInvalidROMAccessMessage(address);
			return 0;
		}

		return (*rom)[address];
	}

	std::string MemoryBankController::GetMessageHeader() const
	{
		return "[" + GetName() + "]";
	}
}