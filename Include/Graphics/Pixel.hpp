#pragma once
#include <cstdint>
#include "Utils/GBSpecs.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Utils/MemoryUtils.hpp"

namespace SHG
{
	struct Pixel
	{
		uint8_t colorIndex = 0;
		uint16_t paletteAddress = GB_BACKGROUND_PALETTE_ADDRESS;
		// Only used for sprite pixels. If false, this pixel will always be drawn over background and window pixels.
		// Otherwise, background and window pixels with colors 1-3 will be drawn over this pixel.
		bool backgroundOverSprite = true;
		// Use to determine priority of sprite pixels. When comparing sprite pixels, 
		// the pixel that belongs to the sprite with a smaller X position has priority.
		int16_t spriteX = GB_SCREEN_WIDTH;
		// Use to determine priority of sprite pixels. When comparing sprite pixels, 
		// the pixel that belongs to the sprite that appears first in OAM has priority.
		uint8_t spriteOAMIndex = MAX_SPRITE_COUNT;
	};
}