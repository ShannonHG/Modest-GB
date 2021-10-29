#pragma once
#include <vector>
#include "Memory/DataStorageDevice.hpp"
#include "Memory/MemoryMapRange.hpp"

namespace SHG
{
	class MemoryMapEntry
	{
	public:
		MemoryMapEntry(DataStorageDevice* device, const std::vector<MemoryMapRange>& addressRanges);

		uint16_t GetLowestBoundAddress();
		uint16_t GetHighestBoundAddress();
		void AddAddressRange(uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		const std::vector<MemoryMapRange>& GetAddressRanges() const;
		DataStorageDevice* GetDevice() const;
	private:
		DataStorageDevice* device;
		std::vector<MemoryMapRange> addressRanges;

		uint16_t lowestBoundAddress = 0;
		uint16_t highestBoundAddress = 0;

		void RecalculateLimits();
	};
}