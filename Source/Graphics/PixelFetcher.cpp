#include <map>
#include <string>
#include "Graphics/PixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/MemoryUtils.hpp"

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

		if (!queuedPixels.empty())
			queuedPixels = std::queue<Pixel>();
	}
}