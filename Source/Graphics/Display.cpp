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
	}

	void Display::Draw(FrameBuffer& buffer)
	{
		Clear();

		SDL_RenderCopy(renderer, buffer.GetTexture(), nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	SDL_Window* Display::GetWindow()
	{
		return window;
	}

	SDL_Renderer* Display::GetRenderer()
	{
		return renderer;
	}

	void Display::Clear()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}
}