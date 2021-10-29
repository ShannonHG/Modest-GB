#include <string>
#include "SDL.h"
#include "Input/Joypad.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const std::map<KeyCode, GBButton> DEFAULT_CONTROLLER_MAPPING =
	{
		{KeyCode::D, GBButton::Right},
		{KeyCode::A, GBButton::Left},
		{KeyCode::W, GBButton::Up},
		{KeyCode::S, GBButton::Down},
		{KeyCode::J, GBButton::A},
		{KeyCode::K, GBButton::B},
		{KeyCode::P, GBButton::Start},
		{KeyCode::O, GBButton::Select},
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

	Joypad::Joypad(InputManager& inputManager)
	{
		ResetButtonStates();
		LoadControllerMapping(DEFAULT_CONTROLLER_MAPPING);

		inputManager.RegisterKeyPressedCallback(std::bind(&Joypad::OnKeyPressed, this, std::placeholders::_1));
		inputManager.RegisterKeyReleasedCallback(std::bind(&Joypad::OnKeyReleased, this, std::placeholders::_1));
	}

	void Joypad::LoadControllerMapping(const std::map<KeyCode, GBButton>& mapping)
	{
		controllerMapping = mapping;
	}

	uint8_t Joypad::Read(uint16_t address)
	{
		uint8_t result = 0;

		// When a button is pressed, the respective bit should be set to 0, 
		// otherwise the bit should be 1.
		if (isDirectionButtonsSelected)
		{
			result = !buttonStates[GBButton::Right]
				| (!buttonStates[GBButton::Left] << 1)
				| (!buttonStates[GBButton::Up] << 2)
				| (!buttonStates[GBButton::Down] << 3);
		}
		else if (isActionButtonsSelected)
		{
			result = !buttonStates[GBButton::A]
				| (!buttonStates[GBButton::B] << 1)
				| (!buttonStates[GBButton::Select] << 2)
				| (!buttonStates[GBButton::Start] << 3);
		}

		result |= isDirectionButtonsSelected << 4;
		result |= isActionButtonsSelected << 5;

		return result;
	}

	void Joypad::Write(uint16_t address, uint8_t value)
	{
		// An option is enabled when its respective bit is set to 0, 
		// otherwise the option is disabled.
		isDirectionButtonsSelected = !(value & 0b00010000);
		isActionButtonsSelected = !(value & 0b00100000);
	}

	bool Joypad::IsAddressAvailable(uint16_t address)
	{
		// This register only contains a single byte.
		return address == 0;
	}

	void Joypad::Reset()
	{
		ResetButtonStates();
		isDirectionButtonsSelected = false;
		isActionButtonsSelected = false;
	}

	void Joypad::ResetButtonStates()
	{
		buttonStates[GBButton::Right] = false;
		buttonStates[GBButton::Left] = false;
		buttonStates[GBButton::Up] = false;
		buttonStates[GBButton::Down] = false;
		buttonStates[GBButton::A] = false;
		buttonStates[GBButton::B] = false;
		buttonStates[GBButton::Start] = false;
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

	void Joypad::SetButtonStateWithKeyCode(KeyCode key, bool isPressed)
	{
		if (controllerMapping.find(key) != controllerMapping.end())
			buttonStates[controllerMapping[key]] = isPressed;
	}
}