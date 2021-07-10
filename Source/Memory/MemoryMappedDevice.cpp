#include "Memory/MemoryMappedDevice.hpp"

namespace SHG
{
	MemoryMappedDevice::MemoryMappedDevice(DataStorageDevice& device, std::vector<MemoryMapRange> ranges) : device(device)
	{
		this->ranges = ranges;
	}

	uint16_t MemoryMappedDevice::GetLowestBound()
	{
		uint16_t lowest = std::numeric_limits<uint16_t>::max();
		for (MemoryMapRange range : ranges)
		{
			if (range.lowerBoundAddress < lowest)
			{
				lowest = range.lowerBoundAddress;
			}
		}

		return lowest;
	}

	uint16_t MemoryMappedDevice::GetHighestBound()
	{
		uint16_t highest = 0;
		for (MemoryMapRange range : ranges)
		{
			if (range.upperBoundAddress > highest)
			{
				highest = range.upperBoundAddress;
			}
		}

		return highest;
	}
}