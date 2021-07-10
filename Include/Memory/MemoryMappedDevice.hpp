#pragma once
#include <vector>
#include "Memory/DataStorageDevice.hpp"
#include "Memory/MemoryMapRange.hpp"

namespace SHG
{
	struct MemoryMappedDevice
	{
		MemoryMappedDevice(DataStorageDevice& device, std::vector<MemoryMapRange> ranges);
		DataStorageDevice& device;
		std::vector<MemoryMapRange> ranges;
		uint16_t GetLowestBound();
		uint16_t GetHighestBound();
	};
}