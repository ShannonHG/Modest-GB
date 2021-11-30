#pragma once
#include <vector>
#include "SDL.h"
#include "Graphics/Color.hpp"

namespace SHG
{
	class Framebuffer
	{
	public:
		Framebuffer();
		Framebuffer(SDL_Window* window, uint16_t width, uint16_t height);
		void Initialize(SDL_Window* window, uint16_t width, uint16_t height);
		Color GetPixel(uint16_t x, uint16_t y);
		void SetPixel(uint16_t x, uint16_t y, const Color& color);
		uint16_t GetWidth() const;
		uint16_t GetHeight() const;
		float GetAspectRatio();
		void UploadData();
		void Clear(const Color& color = RGBA_WHITE);
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