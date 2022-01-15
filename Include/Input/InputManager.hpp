#pragma once
#include "Utils/Events.hpp"

namespace SHG
{
	enum class KeyCode
	{
		NUM_0,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		RIGHT_ARROW, 
		LEFT_ARROW,
		UP_ARROW, 
		DOWN_ARROW
	};

	const std::map<KeyCode, std::string> KEYCODE_STRINGS = 
	{
		{ KeyCode::NUM_0, "0" },
		{ KeyCode::NUM_1, "1" },
		{ KeyCode::NUM_2, "2" },
		{ KeyCode::NUM_3, "3" },
		{ KeyCode::NUM_4, "4" },
		{ KeyCode::NUM_5, "5" },
		{ KeyCode::NUM_6, "6" },
		{ KeyCode::NUM_7, "7" },
		{ KeyCode::NUM_8, "8" },
		{ KeyCode::NUM_9, "9" },
		{ KeyCode::A, "A" },
		{ KeyCode::B, "B" },
		{ KeyCode::C, "C" },
		{ KeyCode::D, "D" },
		{ KeyCode::E, "E" },
		{ KeyCode::F, "F" },
		{ KeyCode::G, "G" },
		{ KeyCode::H, "H" },
		{ KeyCode::I, "I" },
		{ KeyCode::J, "J" },
		{ KeyCode::K, "K" },
		{ KeyCode::L, "L" },
		{ KeyCode::M, "M" },
		{ KeyCode::N, "N" },
		{ KeyCode::O, "O" },
		{ KeyCode::P, "P" },
		{ KeyCode::Q, "Q" },
		{ KeyCode::R, "R" },
		{ KeyCode::S, "S" },
		{ KeyCode::T, "T" },
		{ KeyCode::U, "U" },
		{ KeyCode::V, "V" },
		{ KeyCode::W, "W" },
		{ KeyCode::X, "X" },
		{ KeyCode::Y, "Y" },
		{ KeyCode::Z, "Z" },
		{ KeyCode::RIGHT_ARROW, "RIGHT ARROW" },
		{ KeyCode::LEFT_ARROW, "LEFT ARROW" },
		{ KeyCode::UP_ARROW, "UP ARROW" },
		{ KeyCode::DOWN_ARROW, "DOWN ARROW" }
	};

	enum class ControllerButtonCode
	{
		ACTION_0, 
		ACTION_1,
		ACTION_2,
		ACTION_3,
		DPAD_RIGHT,
		DPAD_LEFT,
		DPAD_UP,
		DPAD_DOWN,
		RIGHT_SHOULDER,
		LEFT_SHOULDER,
		START, 
		MENU,
	};

	const std::map<ControllerButtonCode, std::string> CONTROLLER_BUTTON_STRINGS
	{
		{ ControllerButtonCode::ACTION_0, "A" },
		{ ControllerButtonCode::ACTION_1, "B" },
		{ ControllerButtonCode::ACTION_2, "X" },
		{ ControllerButtonCode::ACTION_3, "Y" },
		{ ControllerButtonCode::DPAD_RIGHT, "DPAD RIGHT" },
		{ ControllerButtonCode::DPAD_LEFT, "DPAD LEFT" },
		{ ControllerButtonCode::DPAD_UP, "DPAD UP" },
		{ ControllerButtonCode::DPAD_DOWN, "DPAD DOWN" },
		{ ControllerButtonCode::RIGHT_SHOULDER, "RIGHT SHOULDER" },
		{ ControllerButtonCode::LEFT_SHOULDER, "LEFT SHOULDER" },
		{ ControllerButtonCode::START, "START" },
		{ ControllerButtonCode::MENU, "MENU" }
	};

	using KeyInputEventCallback = std::function<void(KeyCode)>;
	using ControllerInputEventCallback = std::function<void(ControllerButtonCode)>;
	using GenericInputEventCallback = std::function<void(SDL_Event)>;

	class InputManager
	{
	public:
		void Initialize();
		void Update();

		void RegisterKeyPressedCallback(KeyInputEventCallback callback);
		void RegisterKeyReleasedCallback(KeyInputEventCallback callback);
		void RegisterControllerButtonPressedCallback(ControllerInputEventCallback callback);
		void RegisterControllerButtonReleasedCallback(ControllerInputEventCallback callback);
		void RegisterGenericInputEventCallback(GenericInputEventCallback callback);
	private:
		std::vector<KeyInputEventCallback> keyPressedCallbacks;
		std::vector<KeyInputEventCallback> keyReleasedCallbacks;
		std::vector<ControllerInputEventCallback> controllerButtonPressedCallbacks;
		std::vector<ControllerInputEventCallback> controllerButtonReleasedCallbacks;
		std::vector<GenericInputEventCallback> genericInputEventCallbacks;

		void InvokeKeyInputCallbacks(std::vector<KeyInputEventCallback>& callbacks, KeyCode keyCode);
		void InvokeControllerInputCallbacks(std::vector<ControllerInputEventCallback>& callbacks, ControllerButtonCode buttonCode);
	};
}