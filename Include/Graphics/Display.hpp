#pragma once
#include <array>
#include "Graphics/FrameBuffer.hpp"
#include "SDL.h"

namespace SHG
{
	class Display
	{
	public: 
		Display(int width, int height);
		void Draw(FrameBuffer buffer);

	private:
		int screenWidth = 0;
		int screenHeight = 0;
		SDL_Window* window = NULL;
		SDL_Surface* surface = NULL;
		SDL_Renderer* renderer = NULL;

		void Clear();
	};
}