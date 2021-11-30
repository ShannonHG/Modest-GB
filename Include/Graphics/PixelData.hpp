#pragma once
#include <cstdint>
#include "Graphics/Color.hpp"

namespace SHG
{
	struct PixelData
	{
		int x = 0;
		int y = 0;
		Color color;
		uint8_t palette = 0;
		uint8_t priority = 0;
		uint16_t oamPriority = 0;
		uint16_t spriteX = 0;
		bool isSpritePixel = false;
		bool isValid = false;
		uint8_t colorIndex = 0;
	};
}