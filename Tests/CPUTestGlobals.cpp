#include "CPUTestGlobals.hpp"

namespace SHG
{
	BasicMemory CreatePresetMemory(uint8_t value)
	{
		BasicMemory memory = BasicMemory(std::numeric_limits<uint16_t>::max());
		memory.Write(0, value);

		return memory;
	}

	BasicMemory CreatePresetMemory(const std::vector<uint8_t>& values)
	{
		BasicMemory memory = BasicMemory(std::numeric_limits<uint16_t>::max());
		for (int i = 0; i < values.size(); i++) memory.Write(i, values[i]);

		return memory;
	}
}