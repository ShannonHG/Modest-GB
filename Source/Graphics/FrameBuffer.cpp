#include "Graphics/Framebuffer.hpp"
#include "Logger.hpp"

namespace SHG
{
	Framebuffer::Framebuffer(Display& display, int width, int height) : width(width), height(height)
	{
		uint32_t format = SDL_GetWindowPixelFormat(display.GetWindow());
		pixelFormat = SDL_AllocFormat(format);
		texture = SDL_CreateTexture(display.GetRenderer(), SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		pixels = std::vector<uint32_t>(width * height);
	}

	void Framebuffer::UploadData()
	{
		void* lockedPixels;
		int pitch;

		SDL_LockTexture(texture, nullptr, &lockedPixels, &pitch);

		std::memcpy(lockedPixels, pixels.data(), sizeof(uint32_t) * pixels.size());

		SDL_UnlockTexture(texture);
	}

	void Framebuffer::Clear()
	{
		std::memset(pixels.data(), 0, sizeof(uint32_t) * pixels.size());
		UploadData();
	}

	void Framebuffer::Destroy()
	{
		SDL_FreeFormat(pixelFormat);
	}

	SDL_Texture* Framebuffer::GetTexture()
	{
		return texture;
	}

	uint32_t Framebuffer::GetPixel(int x, int y)
	{
		return pixels[x + y * width];
	}

	void Framebuffer::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		pixels[x + y * width] = SDL_MapRGBA(pixelFormat, r, g, b, a);
	}

	int Framebuffer::GetWidth()
	{
		return width;
	}

	int Framebuffer::GetHeight()
	{
		return height;
	}
}