#include <string>
#include "MockMemoryManagementUnit.hpp"
#include "Logger.hpp"

namespace SHG
{
	MockMemoryManagementUnit::MockMemoryManagementUnit(uint16_t memorySize)
	{
		memory = std::vector<uint8_t>(memorySize);
	}

	uint8_t MockMemoryManagementUnit::GetByte(uint16_t address)
	{
		if (!IsAddressAvailable(address))
		{
			Logger::WriteWarning("[MockMemoryManagementUnit] Address is out of range. Address: " + std::to_string(address) + " | Memory Size: " + std::to_string(memory.size()));
			return 0;
		}
		else
		{
			return memory[address];
		}
	}

	void MockMemoryManagementUnit::SetByte(uint16_t address, uint8_t value)
	{
		if (IsAddressAvailable(address)) memory[address] = value;
	}

	bool MockMemoryManagementUnit::IsAddressAvailable(uint16_t address)
	{
		return address < memory.size();
	}
}