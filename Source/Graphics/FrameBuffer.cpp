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


	void Framebuffer::UploadData()
	{
		void* lockedPixels;
		int pitch;

		SDL_LockTexture(texture, nullptr, &lockedPixels, &pitch);

		std::memcpy(lockedPixels, pixels.data(), sizeof(uint32_t) * pixels.size());

		SDL_UnlockTexture(texture);
	}

	void Framebuffer::Clear(const Color& color)
	{
		std::memset(pixels.data(), SDL_MapRGBA(pixelFormat, color.r, color.g, color.b, color.a), sizeof(uint32_t) * pixels.size());
	}

	void Framebuffer::Destroy()
	{
		SDL_FreeFormat(pixelFormat);
		SDL_DestroyTexture(texture);
	}

	const SDL_Texture* Framebuffer::GetTexture() const
	{
		return texture;
	}

	Color Framebuffer::GetPixel(uint16_t x, uint16_t y)
	{
		Color color;
		SDL_GetRGBA(pixels[x + y * width], pixelFormat, &color.r, &color.g, &color.b, &color.a);

		return color;
	}

	void Framebuffer::SetPixel(uint16_t x, uint16_t y, const Color& color)
	{
		pixels[x + y * width] = SDL_MapRGBA(pixelFormat, color.r, color.g, color.b, color.a);
	}

	uint16_t Framebuffer::GetWidth() const
	{
		return width;
	}

	uint16_t Framebuffer::GetHeight() const
	{
		return height;
	}

	float Framebuffer::GetAspectRatio() const
	{
		return aspectRatio;
	}
}