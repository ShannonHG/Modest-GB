#pragma once
#include <cstdint>

namespace SHG
{
	struct MemoryMapRange
	{
		MemoryMapRange(uint16_t lowerBoundAddress, uint16_t upperBoundAddress);
		uint16_t lowerBoundAddress = 0;
		uint16_t upperBoundAddress = 0;
	};
}