#pragma once
#include "Memory/Memory.hpp"

namespace SHG
{
	Memory CreatePresetMemory(uint8_t value);
	Memory CreatePresetMemory(const std::vector<uint8_t>& values);
}