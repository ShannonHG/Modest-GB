#pragma once
#include <cstdint>

namespace SHG
{
	struct Sprite
	{
		// Horizontal position on the screen plus 8.
		int16_t x = 0;

		// Vertical position on the screen plus 16
		int16_t y = 0;

		// In 8x8 mode, this is the index of this sprite's 8x8 tile (located in $8000-$8FFF).
		// In 8x16 mode, this is the index of the top tile of the sprite, and the least significant bit is ignored.
		uint8_t tileIndex = 0;

		// Is the sprite flipped horizontally?
		bool xFlip = false;

		// Is the sprite flipped vertically?
		bool yFlip = false;

		// Number representing which palette to use.
		uint8_t palette = 0;

		// If false, this sprite will always be drawn over the background and window.
		// Otherwise, background and window pixels with colors 1-3 will be drawn over this sprite.
		bool backgroundOverSprite = false;

		uint8_t oamIndex = 0;
	};
}