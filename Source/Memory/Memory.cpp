#include <string>
#include "Memory/Memory.hpp"
#include "Logger.hpp"

namespace SHG
{
	Memory::Memory(uint32_t memorySize)
	{
		memory = std::vector<uint8_t>(memorySize);
	}

	uint8_t Memory::Read(uint16_t address)
	{
		if (!IsAddressAvailable(address))
		{
			Logger::WriteWarning("[Memory] Address is out of range. Address: " + std::to_string(address) + " | Memory Size: " + std::to_string(memory.size()));
			return 0;
		}
		else
		{
			return memory[address];
		}
	}

	void Memory::Write(uint16_t address, uint8_t value)
	{
		if (IsAddressAvailable(address)) memory[address] = value;
	}

	bool Memory::IsAddressAvailable(uint16_t address)
	{
		return address < memory.size();
	}

	void Memory::Reset()
	{
		std::memset(memory.data(), 0, sizeof(uint8_t) * memory.size());
	}
}