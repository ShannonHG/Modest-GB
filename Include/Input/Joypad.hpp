#pragma once
#include <vector>
#include <map>
#include "Input/InputManager.hpp"
#include "Input/GBButtons.hpp"
#include "Memory/Memory.hpp"

namespace ModestGB
{
	struct ButtonKeyPair
	{
		ControllerButtonCode button;
		KeyCode key;
	};

	class Joypad
	{
	public:
		Joypad(InputManager& inputManager, Memory& memoryMap);
		void LoadInputMapping(const std::map<GBButton, ButtonKeyPair>& mapping);
		const std::map<GBButton, ButtonKeyPair>& GetInputMapping() const;
		ControllerButtonCode GetControllerButtonCode(GBButton button) const;
		KeyCode GetKeyCode(GBButton button)const;
		void SetControllerButtonCode(GBButton button, ControllerButtonCode buttonCode);
		void SetKeyCode(GBButton button, KeyCode keyCode);

		uint8_t Read() const;
		void Write(uint8_t value);
		void Reset();

		bool IsActionButtonsSelected() const;
		bool IsDirectionButtonsSelected() const;
		bool IsDownOrStartPressed() const;
		bool IsUpOrSelectPressed() const;
		bool IsLeftOrBPressed() const;
		bool IsRightOrAPressed() const;

	private:
		Memory* memoryMap;

		std::map<GBButton, ButtonKeyPair> inputMapping;

		std::map<GBButton, bool> buttonStates;
		bool isActionButtonsSelected = false;
		bool isDirectionButtonsSelected = false;

		void OnKeyPressed(KeyCode keyCode);
		void OnKeyReleased(KeyCode keyCode);
		void SetButtonStateWithKeyCode(KeyCode keyCode, bool isPressed);

		void OnControllerButtonPressed(ControllerButtonCode buttonCode);
		void OnControllerButtonReleased(ControllerButtonCode buttonCode);
		void SetButtonStateWithControllerButtonCode(ControllerButtonCode buttonCode, bool isPressed);

		void SetButtonState(GBButton button, bool isPressed);

		void ResetButtonStates();
	};
}