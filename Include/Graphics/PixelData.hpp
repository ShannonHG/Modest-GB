#pragma once
#include <Graphics/PixelColorID.hpp>
#include <cstdint>

namespace SHG
{
	struct PixelData
	{
		int x;
		int y;
		PixelColorID colorID;
		uint8_t palette;
		uint8_t backgroundPriority;
	};
}