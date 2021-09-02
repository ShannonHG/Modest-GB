#include "Graphics/FrameBuffer.hpp"
#include "Logger.hpp"

namespace SHG
{
	FrameBuffer::FrameBuffer(Display& display, int width, int height) : width(width), height(height)
	{
		uint32_t format = SDL_GetWindowPixelFormat(display.GetWindow());
		pixelFormat = SDL_AllocFormat(format);
		texture = SDL_CreateTexture(display.GetRenderer(), SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		pixels = std::vector<uint32_t>(width * height);
	}

	void FrameBuffer::UploadData()
	{
		void* lockedPixels;
		int pitch;

		SDL_LockTexture(texture, nullptr, &lockedPixels, &pitch);

		std::memcpy(lockedPixels, pixels.data(), sizeof(uint32_t) * pixels.size());

		SDL_UnlockTexture(texture);
	}

	void FrameBuffer::Clear()
	{
		std::memset(pixels.data(), 0, sizeof(uint32_t) * pixels.size());
		UploadData();
	}

	void FrameBuffer::Destroy()
	{
		SDL_FreeFormat(pixelFormat);
	}

	SDL_Texture* FrameBuffer::GetTexture()
	{
		return texture;
	}

	uint32_t FrameBuffer::GetPixel(int x, int y)
	{
		return pixels[x + y * width];
	}

	void FrameBuffer::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		pixels[x + y * width] = SDL_MapRGBA(pixelFormat, r, g, b, a);
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