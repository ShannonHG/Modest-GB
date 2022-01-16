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
		{ SDLK_RIGHT, KeyCode::RIGHT_ARROW },
		{ SDLK_LEFT, KeyCode::LEFT_ARROW },
		{ SDLK_UP, KeyCode::UP_ARROW },
		{ SDLK_DOWN, KeyCode::DOWN_ARROW },
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
		{6, ControllerButtonCode::START},
		{4, ControllerButtonCode::MENU},
		{10, ControllerButtonCode::RIGHT_SHOULDER},
		{9, ControllerButtonCode::LEFT_SHOULDER},
	};

	void InputManager::Initialize()
	{
		RefreshControllers();
	}

	const std::string& InputManager::GetCurrentControllerName() const
	{
		return currentControllerName;
	}

	const std::vector<std::string>& InputManager::GetAllControllerNames() const
	{
		return controllerNames;
	}

	void InputManager::RefreshControllers()
	{
		if (joystickCount != SDL_NumJoysticks())
		{
			bool isCurrentControllerFound = false;
			int controllerCount = 0;

			controllerIndexes.clear();
			controllerNames.clear();

			for (int i = 0; i < SDL_NumJoysticks(); i++)
			{
				if (SDL_IsGameController(i))
				{
					std::string controllerName = SDL_GameControllerNameForIndex(i);
					controllerIndexes[controllerName] = i;
					controllerNames.push_back(controllerName);

					if (currentControllerName == controllerName)
						isCurrentControllerFound = true;
				}
			}

			if (controllerIndexes.size() > 0 && !isCurrentControllerFound)
				SetController(controllerIndexes.begin()->first);
		}
	}

	void InputManager::SetController(const std::string& controllerName)
	{
		if (controllerName.empty())
			return;

		if (controllerIndexes.find(controllerName) != controllerIndexes.end())
		{
			if (SDL_GameControllerOpen(controllerIndexes[controllerName]) == nullptr)
			{
				Logger::WriteWarning("Failed to select controller: " + controllerName + ". Error: " + SDL_GetError());
			}
			else
			{
				// Close the existing controller, if any.
				if (!currentControllerName.empty())
					SDL_GameControllerClose(SDL_GameControllerFromInstanceID(controllerIndexes[currentControllerName]));

				currentControllerName = controllerName;
				Logger::WriteInfo("Selected controller: " + currentControllerName);
			}
		}
		else
		{
			Logger::WriteWarning("Failed to select controller: " + controllerName);
		}
	}

	void InputManager::Update()
	{
		RefreshControllers();

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			for (GenericInputEventCallback callback : genericInputEventCallbacks)
				callback(e);

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

	void InputManager::RegisterKeyPressedCallback(KeyInputEventCallback callback)
	{
		keyPressedCallbacks.push_back(callback);
	}

	void InputManager::RegisterKeyReleasedCallback(KeyInputEventCallback callback)
	{
		keyReleasedCallbacks.push_back(callback);
	}

	void InputManager::RegisterControllerButtonPressedCallback(ControllerInputEventCallback callback)
	{
		controllerButtonPressedCallbacks.push_back(callback);
	}

	void InputManager::RegisterControllerButtonReleasedCallback(ControllerInputEventCallback callback)
	{
		controllerButtonReleasedCallbacks.push_back(callback);
	}

	void InputManager::RegisterGenericInputEventCallback(GenericInputEventCallback callback)
	{
		genericInputEventCallbacks.push_back(callback);
	}

	void InputManager::InvokeKeyInputCallbacks(std::vector<KeyInputEventCallback>& callbacks, KeyCode keyCode)
	{
		for (KeyInputEventCallback& callback : callbacks)
			callback(keyCode);
	}

	void InputManager::InvokeControllerInputCallbacks(std::vector<ControllerInputEventCallback>& callbacks, ControllerButtonCode buttonCode)
	{
		for (ControllerInputEventCallback& callback : callbacks)
			callback(buttonCode);
	}
}