#pragma once
#include <array>
#include "Graphics/FrameBuffer.hpp"
#include "SDL.h"
#include <string>

namespace SHG
{
	class Framebuffer;
	class Display
	{
	public: 
		Display();
		Display(const std::string& title, int x, int y, int width, int height);
		void Draw(Framebuffer& buffer);
		SDL_Window* GetWindow();
		SDL_Renderer* GetRenderer();
		void SetWindowTitle(const std::string& title);
		int GetWindowX();
		int GetWindowY();
		int GetWidth();
		int GetHeight();
	private:
		int x = 0;
		int y = 0;
		int screenWidth = 0;
		int screenHeight = 0;
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;

		void Clear();
	};
}