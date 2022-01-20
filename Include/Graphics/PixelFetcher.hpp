#pragma once
#include <queue>
#include "Memory/Register8.hpp"
#include "Graphics/Pixel.hpp"

namespace ModestGB
{
	class PixelFetcher
	{
	public:
		virtual void Tick() = 0;
		virtual void Reset();
		// Returns the first pixel in the fetcher's queue.
		Pixel PopPixel();
		uint8_t GetPixelQueueSize();
		void SetX(int16_t x);
		void SetY(int16_t y);

	protected:
		int16_t x = 0;
		int16_t y = 0;
		std::queue<Pixel> queuedPixels;
	};
}