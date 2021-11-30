#pragma once
#include <queue>
#include "Memory/Register8.hpp"
#include "Graphics/Pixel.hpp"
#include "Memory/MemoryMap.hpp"

namespace SHG
{
	class PixelFetcher
	{
	public:
		virtual void Step() = 0;
		// Returns the first pixel in the fetcher's queue.
		Pixel PopPixel();
		uint8_t GetPixelQueueSize();
		void SetX(uint8_t x);
		void SetY(uint8_t y);
		virtual void Reset();

	protected:
		uint8_t x = 0;
		uint8_t y = 0;
		std::queue<Pixel> queuedPixels;
	};
}