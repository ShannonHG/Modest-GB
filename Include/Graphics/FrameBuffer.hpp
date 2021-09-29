#pragma once
#include <vector>
#include "SDL.h"
#include "Graphics/Display.hpp"

namespace SHG
{
	class Display;
	class Framebuffer
	{
	public:
		Framebuffer();
		Framebuffer(Display& display, uint16_t width, uint16_t height);

		SDL_Texture* texture = nullptr;

		uint32_t GetPixel(uint16_t x, uint16_t y);
		void SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		int GetWidth();
		int GetHeight();
		void UploadData();
		void Clear();
		void Destroy();
		SDL_Texture* GetTexture();

	private:
		SDL_PixelFormat* pixelFormat = nullptr;
		uint16_t width = 0;
		uint16_t height = 0;
		std::vector<uint32_t> pixels;
	};
}