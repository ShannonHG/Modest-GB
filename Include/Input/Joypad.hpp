#pragma once
#include <vector>
#include <map>
#include "Input/InputManager.hpp"
#include "Input/GBButtons.hpp"
#include "Memory/Memory.hpp"

namespace SHG
{
	class Joypad
	{
	public:
		Joypad(InputManager& inputManager, Memory& memoryMap);
		void LoadControllerMapping(const std::map<KeyCode, GBButton>& mapping);

		uint8_t Read() const;
		void Write(uint8_t value);
		void Reset();

		bool IsActionButtonsSelected();
		bool IsDirectionButtonsSelected();
		bool IsDownOrStartPressed();
		bool IsUpOrSelectPressed();
		bool IsLeftOrBPressed();
		bool IsRightOrAPressed();

	private:
		Memory& memoryMap;

		std::map<KeyCode, GBButton> controllerMapping;
		std::map<GBButton, bool> buttonStates;
		bool isActionButtonsSelected = false;
		bool isDirectionButtonsSelected = false;

		void OnKeyPressed(KeyCode keyCode);
		void OnKeyReleased(KeyCode keyCode);
		void SetButtonStateWithKeyCode(KeyCode keyCode, bool isPressed);
		void ResetButtonStates();
	};
}