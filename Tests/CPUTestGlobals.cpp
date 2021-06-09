#include "CPUTestGlobals.hpp"

namespace SHG
{
	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(uint8_t value)
	{
		auto mmu = MockMemoryManagementUnit(std::numeric_limits<uint16_t>::max());
		mmu.SetByte(0, value);

		return mmu;
	}

	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(const std::vector<uint8_t>& values)
	{
		auto mmu = MockMemoryManagementUnit(std::numeric_limits<uint16_t>::max());
		for (int i = 0; i < values.size(); i++) mmu.SetByte(i, values[i]);

		return mmu;
	}
}