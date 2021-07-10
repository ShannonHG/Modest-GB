#include "Graphics/FrameBuffer.hpp"

namespace SHG
{
	FrameBuffer::FrameBuffer(int width, int height) : width(width), height(height)
	{
		pixels = std::vector<uint8_t>(width * height);
	}

	uint8_t FrameBuffer::GetPixel(int x, int y)
	{
		return pixels[x * y + width];
	}

	void FrameBuffer::SetPixel(int x, int y, uint8_t color)
	{
		pixels[x * y + width] = color;
	}

	int FrameBuffer::GetWidth()
	{
		return width;
	}

	int FrameBuffer::GetHeight()
	{
		return height;
	}
}