#include "Graphics/Display.hpp"
#include "Logger.hpp"

namespace SHG
{
	Display::Display(int width, int height)
	{
		screenWidth = width;
		screenHeight = height;

		window = SDL_CreateWindow("GameBoy Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, 0, 0);
		surface = SDL_GetWindowSurface(window);
	}

	void Display::Draw(FrameBuffer buffer)
	{
		Clear();

		for (int x = 0; x < buffer.GetWidth(); x++)
		{
			for (int y = 0; y < buffer.GetHeight(); y++)
			{
				uint8_t color = buffer.GetPixel(x, y);

				SDL_Rect rect;
				rect.x = x;
				rect.y = y;
				rect.w = 1;
				rect.h = 1;

				SDL_SetRenderDrawColor(renderer, color, color, color, 255);
				SDL_RenderFillRect(renderer, &rect);
				SDL_RenderPresent(renderer);
			}
		}
	}

	void Display::Clear()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}
}