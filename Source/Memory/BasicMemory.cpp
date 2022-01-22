#include <string>
#include "Memory/BasicMemory.hpp"
#include "Logger.hpp"

namespace ModestGB
{
	BasicMemory::BasicMemory(uint32_t size)
	{
		this->size = size;
		data = std::vector<uint8_t>(size);
	}

	uint8_t BasicMemory::Read(uint16_t address) const
	{
		if (!IsAddressAvailable(address))
		{
			Logger::WriteWarning("[Memory] Address is out of range. Address: " + std::to_string(address) + " | Memory Size: " + std::to_string(data.size()));
			return 0;
		}
		else
		{
			return data[address];
		}
	}

	void BasicMemory::Write(uint16_t address, uint8_t value)
	{
		if (IsAddressAvailable(address)) 
			data[address] = value;
	}

	bool BasicMemory::IsAddressAvailable(uint16_t address) const
	{
		return address < size;
	}

	void BasicMemory::Reset()
	{
		std::memset(data.data(), 0, sizeof(uint8_t) * data.size());
	}
}