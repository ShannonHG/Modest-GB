#pragma once
#include <cstdint>

namespace SHG
{
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	const Color RGBA_WHITE = Color{ 255, 255, 255, 255 };
	const Color RGBA_BLACK = Color{ 0, 0, 0, 255 };
	const Color RGBA_LIGHT_GRAY = Color{ 175, 175, 175, 255 };
	const Color RGBA_DARK_GRAY = Color{ 100, 100, 100, 255 };

	bool operator==(const Color& c1, const Color& c2);
}