#include "MemoryBankController.hpp"

namespace SHG
{
	bool MemoryBankController::IsRAMEnabled()
	{
		return isRAMEnabled;
	}

	uint8_t MemoryBankController::GetROMBankNumber()
	{
		return romBankNumber;
	}

	void MemoryBankController::AttachRAM(std::vector<uint8_t>& ram)
	{
		this->ram = &ram;
	}

	void MemoryBankController::AttachROM(std::vector<uint8_t>& rom)
	{
		this->rom = &rom;
	}
}