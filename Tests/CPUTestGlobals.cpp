#include "CPUTestGlobals.hpp"

namespace SHG
{
	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(uint8_t value)
	{
		auto mmu = MockMemoryManagementUnit(std::numeric_limits<uint16_t>::max());
		mmu.SetByte(0, value);

		return mmu;
	}
}