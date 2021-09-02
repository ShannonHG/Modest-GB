#pragma once
#include <array>
#include "Graphics/FrameBuffer.hpp"
#include "SDL.h"

namespace SHG
{
	class FrameBuffer;
	class Display
	{
	public: 
		Display(int width, int height);
		void Draw(FrameBuffer& buffer);
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