#include <string>
#include "SDL.h"
#include "Input/Joypad.hpp"
#include "Logger.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Interrupts.hpp"

namespace ModestGB
{
	const std::map<GBButton, ButtonKeyPair> DEFAULT_INPUT_MAPPING =
	{
		{GBButton::RIGHT,  {ControllerButtonCode::DPAD_RIGHT, KeyCode::D}},
		{GBButton::LEFT,   {ControllerButtonCode::DPAD_LEFT,  KeyCode::A}},
		{GBButton::UP,     {ControllerButtonCode::DPAD_UP,    KeyCode::W}},
		{GBButton::DOWN,   {ControllerButtonCode::DPAD_DOWN,  KeyCode::S}},
		{GBButton::A,      {ControllerButtonCode::ACTION_0,   KeyCode::J}},
		{GBButton::B,      {ControllerButtonCode::ACTION_1,   KeyCode::K}},
		{GBButton::START,  {ControllerButtonCode::START,	  KeyCode::P}},
		{GBButton::SELECT, {ControllerButtonCode::SELECT,     KeyCode::O}}
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

		LoadInputMapping(DEFAULT_INPUT_MAPPING);

		inputManager.RegisterKeyPressedCallback(std::bind(&Joypad::OnKeyPressed, this, std::placeholders::_1));
		inputManager.RegisterKeyReleasedCallback(std::bind(&Joypad::OnKeyReleased, this, std::placeholders::_1));

		inputManager.RegisterControllerButtonPressedCallback(std::bind(&Joypad::OnControllerButtonPressed, this, std::placeholders::_1));
		inputManager.RegisterControllerButtonReleasedCallback(std::bind(&Joypad::OnControllerButtonReleased, this, std::placeholders::_1));
	}

	void Joypad::LoadInputMapping(const std::map<GBButton, ButtonKeyPair>& mapping)
	{
		inputMapping = mapping;
	}

	const std::map<GBButton, ButtonKeyPair>& Joypad::GetInputMapping() const
	{
		return inputMapping;
	}

	ControllerButtonCode Joypad::GetControllerButtonCode(GBButton button) const
	{
		return inputMapping.at(button).button;
	}

	KeyCode Joypad::GetKeyCode(GBButton button) const
	{
		return inputMapping.at(button).key;
	}

	void Joypad::SetControllerButtonCode(GBButton button, ControllerButtonCode buttonCode)
	{
		inputMapping[button].button = buttonCode;
	}

	void Joypad::SetKeyCode(GBButton button, KeyCode keyCode)
	{
		inputMapping[button].key = keyCode;
	}

	uint8_t Joypad::Read() const
	{
		uint8_t result = 0xCF;

		// When a button is pressed, the respective bit should be set to 0, 
		// otherwise the bit should be 1.
		if (isDirectionButtonsSelected)
		{
			// Opposing direction buttons cannot be pressed at the same time.
			result =
				static_cast<int>(!(buttonStates.at(GBButton::RIGHT) && !buttonStates.at(GBButton::LEFT))) |
				static_cast<int>(!(buttonStates.at(GBButton::LEFT) && !buttonStates.at(GBButton::RIGHT)) << 1) |
				static_cast<int>(!(buttonStates.at(GBButton::UP) && !buttonStates.at(GBButton::DOWN)) << 2) |
				static_cast<int>(!(buttonStates.at(GBButton::DOWN) && !buttonStates.at(GBButton::UP)) << 3);
		}
		else if (isActionButtonsSelected)
		{
			result =
				static_cast<int>(!buttonStates.at(GBButton::A)) |
				static_cast<int>(!buttonStates.at(GBButton::B) << 1) |
				static_cast<int>(!buttonStates.at(GBButton::SELECT) << 2) |
				static_cast<int>(!buttonStates.at(GBButton::START) << 3);
		}

		result |= static_cast<int>(!isDirectionButtonsSelected) << 4;
		result |= static_cast<int>(!isActionButtonsSelected) << 5;

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
		return buttonStates.at(GBButton::UP) || buttonStates.at(GBButton::SELECT);
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
		buttonStates[GBButton::SELECT] = false;
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

	void Joypad::SetButtonStateWithKeyCode(KeyCode keyCode, bool isPressed)
	{
		for (const std::pair<GBButton, ButtonKeyPair> entry : inputMapping)
		{
			if (entry.second.key == keyCode)
			{
				SetButtonState(entry.first, isPressed);
			}
		}
	}

	void Joypad::SetButtonStateWithControllerButtonCode(ControllerButtonCode buttonCode, bool isPressed)
	{
		for (const std::pair<GBButton, ButtonKeyPair> entry : inputMapping)
		{
			if (entry.second.button == buttonCode)
			{
				SetButtonState(entry.first, isPressed);
			}
		}
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
				case GBButton::RIGHT:
				case GBButton::LEFT:
				case GBButton::UP:
				case GBButton::DOWN:
					Interrupts::RequestInterrupt(*memoryMap, Interrupts::InterruptType::Joypad);
					break;
				}
			}
			else if (isActionButtonsSelected)
			{
				switch (button)
				{
				case GBButton::A:
				case GBButton::B:
				case GBButton::SELECT:
				case GBButton::START:
					Interrupts::RequestInterrupt(*memoryMap, Interrupts::InterruptType::Joypad);
					break;
				}
			}
		}
	}
}