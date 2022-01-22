#include <map>
#include <string>
#include "Graphics/PixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/MemoryUtils.hpp"

namespace ModestGB
{
	Pixel PixelFetcher::PopPixel()
	{
		const Pixel& p = queuedPixels.front();
		queuedPixels.pop();
		return p;
	}

	uint8_t PixelFetcher::GetPixelQueueSize()
	{
		return static_cast<uint8_t>(queuedPixels.size());
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

		// Clear the pixel queue.
		std::queue<Pixel>().swap(queuedPixels);
	}
}