#pragma once
#include <vector>

namespace SHG
{
	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height);

		uint8_t GetPixel(int x, int y);
		void SetPixel(int x, int y, uint8_t color);
		int GetWidth();
		int GetHeight();

	private:
		int width;
		int height;
		std::vector<uint8_t> pixels;
	};
}