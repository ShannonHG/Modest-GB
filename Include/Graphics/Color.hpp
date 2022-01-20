#pragma once
#include <cstdint>

namespace ModestGB
{
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a = 255;
	};

	const Color RGBA_WHITE = { 255, 255, 255, 255 };
	const Color RGBA_BLACK = { 0, 0, 0, 255 };

	bool operator==(const Color & c1, const Color & c2);
	bool operator!=(const Color& c1, const Color& c2);
}