#pragma once
#include "Memory/BasicMemory.hpp"

namespace SHG
{
	BasicMemory CreatePresetMemory(uint8_t value);
	BasicMemory CreatePresetMemory(const std::vector<uint8_t>& values);
}