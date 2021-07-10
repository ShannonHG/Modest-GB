#include "Memory/MemoryMapRange.hpp"

namespace SHG
{
	MemoryMapRange::MemoryMapRange(uint16_t lowerBoundAddress, uint16_t upperBoundAddress)
	{
		this->lowerBoundAddress = lowerBoundAddress;
		this->upperBoundAddress = upperBoundAddress;
	}
}