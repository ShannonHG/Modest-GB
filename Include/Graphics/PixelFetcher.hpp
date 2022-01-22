#pragma once
#include <array>
#include "Memory/Register8.hpp"
#include "Graphics/Pixel.hpp"

namespace ModestGB
{
	const uint8_t PIXEL_FETCHER_QUEUE_SIZE = 16;
	class PixelFetcher
	{
	public:
		virtual void Tick() = 0;
		virtual void Reset();
		// Returns, and removes, the first pixel in the fetcher's queue.
		const Pixel& PopPixel();
		uint8_t GetPixelQueueSize();
		void SetX(int16_t x);
		void SetY(int16_t y);

	protected:
		int16_t x = 0;
		int16_t y = 0;

		// A fixed-size array is used instead of an actual queue to avoid
		// dealing with the queue being resized whenever a pixel is pushed or popped.
		std::array<Pixel, PIXEL_FETCHER_QUEUE_SIZE> pixelQueue;

		void PushPixel(const Pixel& pixel);

	private:
		uint8_t frontPixelIndex = 0;
		uint8_t lastPixelIndex = 0;
		uint8_t pixelQueueSize = 0;
	};
}