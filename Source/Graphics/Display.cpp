#include "Graphics/Display.hpp"
#include "Logger.hpp"

namespace SHG
{
	Display::Display() {}

	Display::Display(const std::string& title, int x, int y, int width, int height)
	{
		screenWidth = width;
		screenHeight = height;

		this->x = x;
		this->y = y;

		window = SDL_CreateWindow(title.c_str(), x, y, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, 0, 0);
	}

	void Display::SetWindowTitle(const std::string& title)
	{
		SDL_SetWindowTitle(window, title.c_str());
	}

	void Display::Draw(Framebuffer& buffer)
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

	int Display::GetWindowX()
	{
		return x;
	}

	int Display::GetWindowY()
	{
		return y;
	}

	int Display::GetWidth()
	{
		return screenWidth;
	}

	int Display::GetHeight()
	{
		return screenHeight;
	}
}