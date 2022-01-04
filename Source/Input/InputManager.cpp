#include <functional>
#include <string>
#include <map>
#include "SDL.h"
#include "imgui_impl_sdl.h"
#include "Input/InputManager.hpp"
#include "Logger.hpp"

namespace SHG
{
	const std::map<SDL_Keycode, KeyCode> KEY_MAP =
	{
		{ SDLK_0, KeyCode::NUM_0 },
		{ SDLK_1, KeyCode::NUM_1 },
		{ SDLK_2, KeyCode::NUM_2 },
		{ SDLK_3, KeyCode::NUM_3 },
		{ SDLK_4, KeyCode::NUM_4 },
		{ SDLK_5, KeyCode::NUM_5 },
		{ SDLK_6, KeyCode::NUM_6 },
		{ SDLK_7, KeyCode::NUM_7 },
		{ SDLK_8, KeyCode::NUM_8 },
		{ SDLK_9, KeyCode::NUM_9 },
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
		{ SDLK_RIGHT, KeyCode::RIGHT },
		{ SDLK_LEFT, KeyCode::LEFT },
		{ SDLK_UP, KeyCode::UP },
		{ SDLK_DOWN, KeyCode::DOWN },
	};

	const std::map<uint8_t, ControllerButtonCode> CONTROLLER_BUTTON_MAP =
	{
		{0, ControllerButtonCode::ACTION_0},
		{1, ControllerButtonCode::ACTION_1},
		{2, ControllerButtonCode::ACTION_2},
		{3, ControllerButtonCode::ACTION_3},
		{11, ControllerButtonCode::DPAD_UP},
		{12, ControllerButtonCode::DPAD_DOWN},
		{14, ControllerButtonCode::DPAD_RIGHT},
		{13, ControllerButtonCode::DPAD_LEFT},
		{6, ControllerButtonCode::MENU},
		{4, ControllerButtonCode::SECONDARY_MENU},
		{10, ControllerButtonCode::RIGHT_SHOULDER},
		{9, ControllerButtonCode::LEFT_SHOULDER},
	};

	void InputManager::Initialize()
	{
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			if (SDL_IsGameController(i))
			{
				// TODO: Add support for selecting controller
				SDL_GameControllerOpen(i);
				return;
			}
		}
	}

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

			// Detect keyboard input.
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

			// Detect controller button presses.
			uint8_t controllerButton = e.cbutton.button;
			if (CONTROLLER_BUTTON_MAP.find(controllerButton) != CONTROLLER_BUTTON_MAP.end())
			{
				switch (e.type)
				{
				case SDL_CONTROLLERBUTTONDOWN:
					InvokeControllerInputCallbacks(controllerButtonPressedCallbacks, CONTROLLER_BUTTON_MAP.at(controllerButton));
					break;
				case SDL_CONTROLLERBUTTONUP:
					InvokeControllerInputCallbacks(controllerButtonReleasedCallbacks, CONTROLLER_BUTTON_MAP.at(controllerButton));
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

	void InputManager::RegisterControllerButtonPressedCallback(ControllerInputEvent callback)
	{
		controllerButtonPressedCallbacks.push_back(callback);
	}

	void InputManager::RegisterControllerButtonReleasedCallback(ControllerInputEvent callback)
	{
		controllerButtonReleasedCallbacks.push_back(callback);
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

	void InputManager::InvokeControllerInputCallbacks(std::vector<ControllerInputEvent>& callbacks, ControllerButtonCode buttonCode)
	{
		for (ControllerInputEvent& callback : callbacks)
			callback(buttonCode);
	}
}