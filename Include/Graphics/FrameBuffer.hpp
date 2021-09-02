#pragma once
#include <vector>
#include "SDL.h"
#include "Graphics/Display.hpp"

namespace SHG
{
	class Display;
	class FrameBuffer
	{
	public:
		FrameBuffer(Display& display, int width, int height);

		SDL_Texture* texture;

		uint32_t GetPixel(int x, int y);
		void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		int GetWidth();
		int GetHeight();
		void UploadData();
		void Clear();
		void Destroy();
		SDL_Texture* GetTexture();

	private:
		SDL_PixelFormat* pixelFormat;
		int width;
		int height;
		std::vector<uint32_t> pixels;
	};
}