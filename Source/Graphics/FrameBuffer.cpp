#include "Graphics/Framebuffer.hpp"
#include "Logger.hpp"

namespace SHG
{
	Framebuffer::Framebuffer() {}

	Framebuffer::Framebuffer(SDL_Window* window, uint16_t width, uint16_t height)
	{
		Initialize(window, width, height);
	}

	void Framebuffer::Initialize(SDL_Window* window, uint16_t width, uint16_t height)
	{
		this->width = width;
		this->height = height;
		aspectRatio = width / (float)height;
		pixelFormat = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
		texture = SDL_CreateTexture(SDL_GetRenderer(window), SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		pixels = std::vector<uint32_t>(width * height);
	}

	// TODO: Handle case where the framebuffer is uninitialized

	void Framebuffer::UploadData()
	{
		void* lockedPixels;
		int pitch;

		SDL_LockTexture(texture, nullptr, &lockedPixels, &pitch);

		std::memcpy(lockedPixels, pixels.data(), sizeof(uint32_t) * pixels.size());

		SDL_UnlockTexture(texture);
	}

	void Framebuffer::Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		std::memset(pixels.data(), SDL_MapRGBA(pixelFormat, r, g, b, a), sizeof(uint32_t) * pixels.size());
		UploadData();
	}

	void Framebuffer::Destroy()
	{
		SDL_FreeFormat(pixelFormat);
		SDL_DestroyTexture(texture);
	}

	SDL_Texture* Framebuffer::GetTexture()
	{
		return texture;
	}

	uint32_t Framebuffer::GetPixel(uint16_t x, uint16_t y)
	{
		return pixels[x + y * width];
	}

	void Framebuffer::SetPixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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

	float Framebuffer::GetAspectRatio()
	{
		return aspectRatio;
	}
}