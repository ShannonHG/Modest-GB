#pragma once
#include <vector>
#include <map>
#include "Memory/DataStorageDevice.hpp"
#include "Input/InputManager.hpp"
#include "Input/GBButtons.hpp"

namespace SHG
{
	class Joypad : public DataStorageDevice
	{
	public:
		Joypad(InputManager& inputManager);
		void LoadControllerMapping(const std::map<KeyCode, GBButton>& mapping);

		uint8_t Read(uint16_t address) override;
		void Write(uint16_t address, uint8_t value) override;
		bool IsAddressAvailable(uint16_t address) override;
		void Reset() override;

	private:
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