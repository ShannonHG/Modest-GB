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
		Display(const std::string& title, int width, int height);
		void Draw(Framebuffer& buffer);
		SDL_Window* GetWindow();
		SDL_Renderer* GetRenderer();
	private:
		int screenWidth = 0;
		int screenHeight = 0;
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;

		void Clear();
	};
}