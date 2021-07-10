#pragma once
#include <Graphics/PixelColorID.hpp>
#include <cstdint>

namespace SHG
{
	struct PixelData
	{
		PixelColorID colorID;
		uint8_t palette;
		uint8_t spritePriority;
		uint8_t backgroundPriority;
	};
}