#pragma once
#include <vector>
#include "SDL.h"

namespace SHG
{
	class Framebuffer
	{
	public:
		Framebuffer();
		Framebuffer(SDL_Window* window, uint16_t width, uint16_t height);
		void Initialize(SDL_Window* window, uint16_t width, uint16_t height);
		uint32_t GetPixel(uint16_t x, uint16_t y);
		void SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		int GetWidth();
		int GetHeight();
		float GetAspectRatio();
		void UploadData();
		void Clear();
		void Destroy();
		SDL_Texture* GetTexture();

	private:
		SDL_Texture* texture = nullptr;
		SDL_PixelFormat* pixelFormat = nullptr;
		uint16_t width = 0;
		uint16_t height = 0;
		float aspectRatio = 0;
		std::vector<uint32_t> pixels;
	};
}