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
		RIGHT, 
		LEFT,
		UP, 
		DOWN
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

	using KeyInputEvent = std::function<void(KeyCode)>;
	using ControllerInputEvent = std::function<void(ControllerButtonCode)>;

	class InputManager
	{
	public:
		void Initialize();
		void Update();

		void RegisterKeyPressedCallback(KeyInputEvent callback);
		void RegisterKeyReleasedCallback(KeyInputEvent callback);
		void RegisterControllerButtonPressedCallback(ControllerInputEvent callback);
		void RegisterControllerButtonReleasedCallback(ControllerInputEvent callback);
		void RegisterExitEventCallback(SimpleEvent callback);
	private:
		std::vector<KeyInputEvent> keyPressedCallbacks;
		std::vector<KeyInputEvent> keyReleasedCallbacks;
		std::vector<ControllerInputEvent> controllerButtonPressedCallbacks;
		std::vector<ControllerInputEvent> controllerButtonReleasedCallbacks;
		std::vector<SimpleEvent> exitEventCallbacks;

		void InvokeKeyInputCallbacks(std::vector<KeyInputEvent>& callbacks, KeyCode keyCode);
		void InvokeControllerInputCallbacks(std::vector<ControllerInputEvent>& callbacks, ControllerButtonCode buttonCode);
	};
}