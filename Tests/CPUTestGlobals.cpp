#include "CPUTestGlobals.hpp"

namespace SHG
{
	Memory CreatePresetMemory(uint8_t value)
	{
		Memory memory = Memory(std::numeric_limits<uint16_t>::max());
		memory.Write(0, value);

		return memory;
	}

	Memory CreatePresetMemory(const std::vector<uint8_t>& values)
	{
		Memory memory = Memory(std::numeric_limits<uint16_t>::max());
		for (int i = 0; i < values.size(); i++) memory.Write(i, values[i]);

		return memory;
	}
}