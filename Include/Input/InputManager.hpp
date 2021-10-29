#pragma once
#include "Utils/Events.hpp"

namespace SHG
{
	enum class KeyCode
	{
		Num_0,
		Num_1,
		Num_2,
		Num_3,
		Num_4,
		Num_5,
		Num_6,
		Num_7,
		Num_8,
		Num_9,
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
		Right, 
		Left,
		Up, 
		Down
	};

	using KeyInputEvent = std::function<void(KeyCode)>;

	class InputManager
	{
	public:
		void Update();

		void RegisterKeyPressedCallback(KeyInputEvent callback);
		void RegisterKeyReleasedCallback(KeyInputEvent callback);
		void RegisterExitEventCallback(SimpleEvent callback);
	private:
		std::vector<KeyInputEvent> keyPressedCallbacks;
		std::vector<KeyInputEvent> keyReleasedCallbacks;
		std::vector<SimpleEvent> exitEventCallbacks;

		void InvokeKeyInputCallbacks(std::vector<KeyInputEvent>& callbacks, KeyCode keyCode);
	};
}