#include <string>
#include "SDL.h"
#include "Input/Joypad.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Interrupts.hpp"

namespace SHG
{
	const std::map<KeyCode, GBButton> DEFAULT_KEYBOARD_MAPPING =
	{
		{KeyCode::D, GBButton::RIGHT},
		{KeyCode::A, GBButton::LEFT},
		{KeyCode::W, GBButton::UP},
		{KeyCode::S, GBButton::DOWN},
		{KeyCode::J, GBButton::A},
		{KeyCode::K, GBButton::B},
		{KeyCode::P, GBButton::START},
		{KeyCode::O, GBButton::Select},
	};

	const std::map<ControllerButtonCode, GBButton> DEFAULT_CONTROLLER_MAPPING =
	{
		{ControllerButtonCode::DPAD_RIGHT, GBButton::RIGHT},
		{ControllerButtonCode::DPAD_LEFT, GBButton::LEFT},
		{ControllerButtonCode::DPAD_UP, GBButton::UP},
		{ControllerButtonCode::DPAD_DOWN, GBButton::DOWN},
		{ControllerButtonCode::ACTION_0, GBButton::A},
		{ControllerButtonCode::ACTION_1, GBButton::B},
		{ControllerButtonCode::MENU, GBButton::START},
		{ControllerButtonCode::SECONDARY_MENU, GBButton::Select}
	};

	// Joypad Register
	// Bit 7 - Unused
	// Bit 6 - Unused
	// Bit 5 - Select Action Buttons (0 = Selected)
	// Bit 4 - Select Direction Buttons (0 = Selected)
	// Bit 3 - Down or Start (0 = Pressed)
	// Bit 2 - Up or Select (0 = Pressed)
	// Bit 1 - Left or B (0 = Pressed)
	// Bit 0 - Right or A (0 = Pressed)

	Joypad::Joypad(InputManager& inputManager, Memory& memoryMap) : memoryMap(&memoryMap)
	{
		ResetButtonStates();

		LoadKeyboardInputMapping(DEFAULT_KEYBOARD_MAPPING);
		LoadControllerInputMapping(DEFAULT_CONTROLLER_MAPPING);

		inputManager.RegisterKeyPressedCallback(std::bind(&Joypad::OnKeyPressed, this, std::placeholders::_1));
		inputManager.RegisterKeyReleasedCallback(std::bind(&Joypad::OnKeyReleased, this, std::placeholders::_1));

		inputManager.RegisterControllerButtonPressedCallback(std::bind(&Joypad::OnControllerButtonPressed, this, std::placeholders::_1));
		inputManager.RegisterControllerButtonReleasedCallback(std::bind(&Joypad::OnControllerButtonReleased, this, std::placeholders::_1));
	}

	void Joypad::LoadKeyboardInputMapping(const std::map<KeyCode, GBButton>& mapping)
	{
		keyboardInputMapping = mapping;
	}

	void Joypad::LoadControllerInputMapping(const std::map<ControllerButtonCode, GBButton>& mapping)
	{
		controllerInputMapping = mapping;
	}

	uint8_t Joypad::Read() const
	{
		uint8_t result = 0xCF;

		// When a button is pressed, the respective bit should be set to 0, 
		// otherwise the bit should be 1.
		if (isDirectionButtonsSelected)
		{
			// Opposing direction buttons cannot be pressed at the same time.
			result = !(buttonStates.at(GBButton::RIGHT) && !buttonStates.at(GBButton::LEFT))
				| (!(buttonStates.at(GBButton::LEFT) && !buttonStates.at(GBButton::RIGHT)) << 1)
				| (!(buttonStates.at(GBButton::UP) && !buttonStates.at(GBButton::DOWN)) << 2)
				| (!(buttonStates.at(GBButton::DOWN) && !buttonStates.at(GBButton::UP)) << 3);
		}
		else if (isActionButtonsSelected)
		{
			result = 
				!buttonStates.at(GBButton::A)
				| (!buttonStates.at(GBButton::B) << 1)
				| (!buttonStates.at(GBButton::Select) << 2)
				| (!buttonStates.at(GBButton::START) << 3);
		}

		result |= !isDirectionButtonsSelected << 4;
		result |= !isActionButtonsSelected << 5;

		// The upper two bits are not used.
		result |= 0b11000000;

		return result;
	}

	void Joypad::Write(uint8_t value)
	{
		// An option is enabled when its respective bit is set to 0, 
		// otherwise the option is disabled.
		isDirectionButtonsSelected = !(value & 0b00010000);
		isActionButtonsSelected = !(value & 0b00100000);
	}

	void Joypad::Reset()
	{
		ResetButtonStates();
		isActionButtonsSelected = false;
		isDirectionButtonsSelected = false;
	}

	bool Joypad::IsActionButtonsSelected() const
	{	
		return isActionButtonsSelected;
	}

	bool Joypad::IsDirectionButtonsSelected() const
	{
		return isDirectionButtonsSelected;
	}

	bool Joypad::IsDownOrStartPressed() const
	{
		return buttonStates.at(GBButton::DOWN) || buttonStates.at(GBButton::START);
	}

	bool Joypad::IsUpOrSelectPressed() const
	{
		return buttonStates.at(GBButton::UP) || buttonStates.at(GBButton::Select);
	}

	bool Joypad::IsLeftOrBPressed() const
	{
		return buttonStates.at(GBButton::LEFT) || buttonStates.at(GBButton::B);
	}

	bool Joypad::IsRightOrAPressed() const
	{
		return buttonStates.at(GBButton::RIGHT) || buttonStates.at(GBButton::A);
	}

	void Joypad::ResetButtonStates()
	{
		buttonStates[GBButton::RIGHT] = false;
		buttonStates[GBButton::LEFT] = false;
		buttonStates[GBButton::UP] = false;
		buttonStates[GBButton::DOWN] = false;
		buttonStates[GBButton::A] = false;
		buttonStates[GBButton::B] = false;
		buttonStates[GBButton::START] = false;
		buttonStates[GBButton::Select] = false;
	}

	void Joypad::OnKeyPressed(KeyCode keyCode)
	{
		SetButtonStateWithKeyCode(keyCode, true);
	}

	void Joypad::OnKeyReleased(KeyCode keyCode)
	{
		SetButtonStateWithKeyCode(keyCode, false);
	}

	void Joypad::OnControllerButtonPressed(ControllerButtonCode buttonCode)
	{
		SetButtonStateWithControllerButtonCode(buttonCode, true);
	}

	void Joypad::OnControllerButtonReleased(ControllerButtonCode buttonCode)
	{
		SetButtonStateWithControllerButtonCode(buttonCode, false);
	}

	void Joypad::SetButtonStateWithKeyCode(KeyCode key, bool isPressed)
	{
		if (keyboardInputMapping.find(key) == keyboardInputMapping.end())
			return;

		GBButton button = keyboardInputMapping[key];
		SetButtonState(button, isPressed);
	}

	void Joypad::SetButtonStateWithControllerButtonCode(ControllerButtonCode buttonCode, bool isPressed)
	{
		if (controllerInputMapping.find(buttonCode) == controllerInputMapping.end())
			return;

		GBButton button = controllerInputMapping[buttonCode];
		SetButtonState(button, isPressed);
	}

	void Joypad::SetButtonState(GBButton button, bool isPressed)
	{
		buttonStates[button] = isPressed;

		if (isPressed)
		{
			if (isDirectionButtonsSelected)
			{
				switch (button)
				{
				case SHG::GBButton::RIGHT:
				case SHG::GBButton::LEFT:
				case SHG::GBButton::UP:
				case SHG::GBButton::DOWN:
					RequestInterrupt(*memoryMap, InterruptType::Joypad);
					break;
				}
			}
			else if (isActionButtonsSelected)
			{
				switch (button)
				{
				case SHG::GBButton::A:
				case SHG::GBButton::B:
				case SHG::GBButton::Select:
				case SHG::GBButton::START:
					RequestInterrupt(*memoryMap, InterruptType::Joypad);
					break;
				}
			}
		}
	}
}