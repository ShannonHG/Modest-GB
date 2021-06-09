#pragma once
#include "MockMemoryManagementUnit.hpp"

namespace SHG
{
	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(uint8_t value);
	MockMemoryManagementUnit CreatePresetMemoryManagementUnit(const std::vector<uint8_t>& values);
}