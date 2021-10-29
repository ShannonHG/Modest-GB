#include <functional>
#include <map>
#include "SDL.h"
#include "imgui_impl_sdl.h"
#include "Input/InputManager.hpp"

namespace SHG
{
	const std::map<SDL_Keycode, KeyCode> KEY_MAP =
	{
		{ SDLK_0, KeyCode::Num_0 },
		{ SDLK_1, KeyCode::Num_1 },
		{ SDLK_2, KeyCode::Num_2 },
		{ SDLK_3, KeyCode::Num_3 },
		{ SDLK_4, KeyCode::Num_4 },
		{ SDLK_5, KeyCode::Num_5 },
		{ SDLK_6, KeyCode::Num_6 },
		{ SDLK_7, KeyCode::Num_7 },
		{ SDLK_8, KeyCode::Num_8 },
		{ SDLK_9, KeyCode::Num_9 },
		{ SDLK_a, KeyCode::A },
		{ SDLK_b, KeyCode::B },
		{ SDLK_c, KeyCode::C },
		{ SDLK_d, KeyCode::D },
		{ SDLK_e, KeyCode::E },
		{ SDLK_f, KeyCode::F },
		{ SDLK_g, KeyCode::G },
		{ SDLK_h, KeyCode::H },
		{ SDLK_i, KeyCode::I },
		{ SDLK_j, KeyCode::J },
		{ SDLK_k, KeyCode::K },
		{ SDLK_l, KeyCode::L },
		{ SDLK_m, KeyCode::M },
		{ SDLK_n, KeyCode::N },
		{ SDLK_o, KeyCode::O },
		{ SDLK_p, KeyCode::P },
		{ SDLK_q, KeyCode::Q },
		{ SDLK_r, KeyCode::R },
		{ SDLK_s, KeyCode::S },
		{ SDLK_t, KeyCode::T },
		{ SDLK_u, KeyCode::U },
		{ SDLK_v, KeyCode::V },
		{ SDLK_w, KeyCode::W },
		{ SDLK_x, KeyCode::X },
		{ SDLK_y, KeyCode::Y },
		{ SDLK_z, KeyCode::Z },
		{ SDLK_RIGHT, KeyCode::Right },
		{ SDLK_LEFT, KeyCode::Left },
		{ SDLK_UP, KeyCode::Up },
		{ SDLK_DOWN, KeyCode::Down },
	};

	void InputManager::Update()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT)
			{
				for (SimpleEvent callback : exitEventCallbacks)
					callback();

				return;
			}

			SDL_Keycode keycode = e.key.keysym.sym;

			if (KEY_MAP.find(keycode) != KEY_MAP.end())
			{
				switch (e.type)
				{
				case SDL_KEYDOWN:
					InvokeKeyInputCallbacks(keyPressedCallbacks, KEY_MAP.at(keycode));
					break;
				case SDL_KEYUP:
					InvokeKeyInputCallbacks(keyReleasedCallbacks, KEY_MAP.at(keycode));
					break;
				}
			}
		}
	}


	void InputManager::RegisterKeyPressedCallback(KeyInputEvent callback)
	{
		keyPressedCallbacks.push_back(callback);
	}

	void InputManager::RegisterKeyReleasedCallback(KeyInputEvent callback)
	{
		keyReleasedCallbacks.push_back(callback);
	}

	void InputManager::RegisterExitEventCallback(SimpleEvent callback)
	{
		exitEventCallbacks.push_back(callback);
	}

	void InputManager::InvokeKeyInputCallbacks(std::vector<KeyInputEvent>& callbacks, KeyCode keyCode)
	{
		for (KeyInputEvent& callback : callbacks)
			callback(keyCode);
	}
}