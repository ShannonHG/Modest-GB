#pragma once
#include <cstdint>
#include "Graphics/Color.hpp"

namespace SHG
{
	struct PixelData
	{
		int x;
		int y;
		Color color;
		uint8_t palette;
		uint8_t priority;
	};
}