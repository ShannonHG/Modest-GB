#include "Memory/MemoryMapEntry.hpp"

namespace SHG
{
	MemoryMapEntry::MemoryMapEntry(DataStorageDevice* device, const std::vector<MemoryMapRange>& addressRanges)
	{
		this->device = device;
		this->addressRanges = addressRanges;

		lowestBoundAddress = std::numeric_limits<uint16_t>::max();
		highestBoundAddress = std::numeric_limits<uint16_t>::min();

		RecalculateLimits();
	}

	void MemoryMapEntry::AddAddressRange(uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		addressRanges.push_back({ lowerBoundAddress, upperBoundAddress });

		RecalculateLimits();
	}

	uint16_t MemoryMapEntry::GetLowestBoundAddress()
	{
		return lowestBoundAddress;
	}

	uint16_t MemoryMapEntry::GetHighestBoundAddress()
	{
		return highestBoundAddress;
	}

	const std::vector<MemoryMapRange>& MemoryMapEntry::GetAddressRanges() const
	{
		return addressRanges;
	}

	DataStorageDevice* MemoryMapEntry::GetDevice() const
	{
		return device;
	}

	void MemoryMapEntry::RecalculateLimits()
	{
		for (const MemoryMapRange& range : addressRanges)
		{
			if (range.lowerBoundAddress < lowestBoundAddress)
				lowestBoundAddress = range.lowerBoundAddress;

			if (range.upperBoundAddress > highestBoundAddress)
				highestBoundAddress = range.upperBoundAddress;
		}
	}
}