#include <map>
#include <string>
#include "Graphics/PixelFetcher.hpp"
#include "Graphics/Graphics.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/GBMemoryMapAddresses.hpp"

namespace SHG
{
	Pixel PixelFetcher::PopPixel()
	{
		Pixel p = queuedPixels.front();
		queuedPixels.pop();
		return p;
	}

	uint8_t PixelFetcher::GetPixelQueueSize()
	{
		return queuedPixels.size();
	}

	void PixelFetcher::SetX(uint8_t x)
	{
		this->x = x;
	}

	void PixelFetcher::SetY(uint8_t y)
	{
		this->y = y;
	}

	void PixelFetcher::Reset()
	{
		x = 0;
		y = 0;
		queuedPixels = std::queue<Pixel>();
	}
}