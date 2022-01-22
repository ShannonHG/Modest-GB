#include <map>
#include <string>
#include "Graphics/PixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/MemoryUtils.hpp"

namespace ModestGB
{
	const Pixel& PixelFetcher::PopPixel()
	{
		if (pixelQueueSize == 0)
			throw std::out_of_range("Attempted to pop pixel from an empty queue.");

		const Pixel& result = pixelQueue[frontPixelIndex];
		frontPixelIndex = (frontPixelIndex + 1) % PIXEL_FETCHER_QUEUE_SIZE;
		pixelQueueSize--;

		return result;
	}

	void PixelFetcher::PushPixel(const Pixel& pixel)
	{
		if (pixelQueueSize == PIXEL_FETCHER_QUEUE_SIZE)
			return;

		pixelQueueSize++;
		pixelQueue[lastPixelIndex] = pixel;
		lastPixelIndex = (lastPixelIndex + 1) % PIXEL_FETCHER_QUEUE_SIZE;
	}

	uint8_t PixelFetcher::GetPixelQueueSize()
	{
		return pixelQueueSize;
	}

	void PixelFetcher::SetX(int16_t x)
	{
		this->x = x;
	}

	void PixelFetcher::SetY(int16_t y)
	{
		this->y = y;
	}

	void PixelFetcher::Reset()
	{
		x = 0;
		y = 0;

		frontPixelIndex = 0;
		lastPixelIndex = 0;
		pixelQueueSize = 0;
	}
}