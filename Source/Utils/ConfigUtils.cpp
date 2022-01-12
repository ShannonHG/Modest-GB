#include <string>
#include <filesystem>
#include "Utils/ConfigUtils.hpp"
#include "Utils/MemoryUtils.hpp"
#include "Logger.hpp"
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/emitter.h"

namespace SHG::Config
{
	const std::string CPU_WINDOW_CONFIG_ITEM_NAME = "CPU Debug Window Open";
	const std::string TILES_WINDOW_CONFIG_ITEM_NAME = "Tiles Debug Window Open";
	const std::string BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME = "Background Map Debug Window Open";
	const std::string WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME = "Window Map Debug Window Open";
	const std::string SPRITES_WINDOW_CONFIG_ITEM_NAME = "Sprites Debug Window Open";
	const std::string LOG_WINDOW_CONFIG_ITEM_NAME = "Log Window Open";
	const std::string SETTINGS_WINDOW_CONFIG_ITEM_NAME = "Settings Window Open";
	const std::string SOUND_WINDOW_CONFIG_ITEM_NAME = "Sound Debug Window Open";
	const std::string TIMER_WINDOW_CONFIG_ITEM_NAME = "Timer Debug Window Open";
	const std::string VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME = "Video Registers Debug Window Open";
	const std::string JOYPAD_WINDOW_CONFIG_ITEM_NAME = "Joypad Debug Window Open";

	const std::string PALETTES_NODE_NAME = "Palettes";
	const std::string MUTED_NODE_NAME = "Muted";
	const std::string VOLUME_NODE_NAME = "Volume";
	const std::string OUTPUT_DEVICE_NODE_NAME = "Output Device";
	const std::string JOYPAD_NODE_NAME = "Joypad";
	const std::string CONTROLLER_BUTTON_NODE_NAME = "Ctrl Button";
	const std::string KEYCODE_NODE_NAME = "Key";
	const std::string SAVED_DATA_NODE_NAME = "Saved Data Location";

	const std::map<SavedDataSearchType, std::string> SAVED_DATA_LOCATION_STRINGS =
	{
		{ SavedDataSearchType::ROM_DIRECTORY, "ROM Directory" },
		{ SavedDataSearchType::EMULATOR_DIRECTORY, "Emulator Directory" }
	};

	const std::map<uint16_t, std::string> PALETTE_NAMES =
	{
		{ GB_BACKGROUND_PALETTE_ADDRESS, "BGP" },
		{ GB_SPRITE_PALETTE_0_ADDRESS, "OBP0" },
		{ GB_SPRITE_PALETTE_1_ADDRESS, "OBP1" }
	};

	std::array<std::string, 4> COLOR_INDEX_NAMES = { "0", "1", "2", "3" };

	void SaveVideoConfiguration(YAML::Node& node, const PPU& ppu)
	{
		for (const std::pair<uint16_t, std::string>& pair : PALETTE_NAMES)
		{
			// Write the color of all 4 color indexes in the palette to the yaml node.
			for (uint8_t i = 0; i < 4; i++)
			{
				Color color = ppu.GetPaletteTint(pair.first, i);

				node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]].push_back(static_cast<int>(color.r));
				node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]].push_back(static_cast<int>(color.g));
				node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]].push_back(static_cast<int>(color.b));
			}
		}
	}

	bool LoadVideoConfiguration(const YAML::Node& node, PPU& ppu)
	{
		try
		{
			for (const std::pair<uint16_t, std::string>& pair : PALETTE_NAMES)
			{
				// Attempt to read the color of all 4 color indexes in the palette from the yaml node.
				for (uint8_t i = 0; i < 4; i++)
				{
					Color color =
					{
						.r = node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]][0].as<uint8_t>(),
						.g = node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]][1].as<uint8_t>(),
						.b = node[PALETTES_NODE_NAME][pair.second][COLOR_INDEX_NAMES[i]][2].as<uint8_t>()
					};

					ppu.SetPaletteTint(pair.first, i, color);
				}
			}
		}
		catch (std::exception e)
		{
			Logger::WriteError("Failed to parse video configuration. Error: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	void SaveAudioConfiguration(YAML::Node& node, const APU& apu)
	{
		node[MUTED_NODE_NAME] = apu.IsMuted();
		node[VOLUME_NODE_NAME] = apu.GetMasterVolume();
		node[OUTPUT_DEVICE_NODE_NAME] = apu.GetCurrentOutputDeviceName();
	}

	bool LoadAudioConfiguration(YAML::Node& node, APU& apu)
	{
		try
		{
			apu.Mute(node[MUTED_NODE_NAME].as<bool>());
			apu.SetMasterVolume(node[VOLUME_NODE_NAME].as<float>());
			apu.SetOutputDevice(node[OUTPUT_DEVICE_NODE_NAME].as<std::string>());
		}
		catch (std::exception e)
		{
			Logger::WriteError("Failed to load audio configuration. Error: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	void SaveJoypadConfiguration(YAML::Node& node, const Joypad& joypad)
	{
		std::map<GBButton, ButtonKeyPair> inputMapping = joypad.GetInputMapping();

		for (const std::pair<GBButton, ButtonKeyPair>& pair : inputMapping)
		{
			node[JOYPAD_NODE_NAME][GBBUTTON_STRINGS.at(pair.first)][CONTROLLER_BUTTON_NODE_NAME] = CONTROLLER_BUTTON_STRINGS.at(pair.second.button);
			node[JOYPAD_NODE_NAME][GBBUTTON_STRINGS.at(pair.first)][KEYCODE_NODE_NAME] = KEYCODE_STRINGS.at(pair.second.key);
		}
	}

	bool LoadJoypadConfiguration(YAML::Node& node, Joypad& joypad)
	{
		try
		{
			for (const std::pair<GBButton, std::string>& pair : GBBUTTON_STRINGS)
			{
				std::string controllerNodeValue = node[JOYPAD_NODE_NAME][GBBUTTON_STRINGS.at(pair.first)][CONTROLLER_BUTTON_NODE_NAME].Scalar();

				// Find the ControllerButtonCode whose string representation matches the value retrieved from the yaml node.
				for (const std::pair<ControllerButtonCode, std::string>& ctrlPair : CONTROLLER_BUTTON_STRINGS)
				{
					if (controllerNodeValue == ctrlPair.second)
					{
						joypad.SetControllerButtonCode(pair.first, ctrlPair.first);
						break;
					}
				}

				std::string keyNodeValue = node[JOYPAD_NODE_NAME][GBBUTTON_STRINGS.at(pair.first)][KEYCODE_NODE_NAME].Scalar();

				// Find the KeyCode whose string representation matches the value retrieved from the yaml node.
				for (const std::pair<KeyCode, std::string>& keyPair : KEYCODE_STRINGS)
				{
					if (keyNodeValue == keyPair.second)
					{
						joypad.SetKeyCode(pair.first, keyPair.first);
						break;
					}
				}
			}
		}
		catch (std::exception e)
		{
			Logger::WriteError("Failed to load joypad configuration. Error: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	void SaveCartridgeConfiguration(YAML::Node& node, const Cartridge& cartridge)
	{
		node[SAVED_DATA_NODE_NAME] = SAVED_DATA_LOCATION_STRINGS.at(cartridge.GetSavedDataSearchType());
	}

	bool LoadCartridgeConfiguration(YAML::Node& node, Cartridge& cartridge)
	{
		try
		{
			auto nodeValue = node[SAVED_DATA_NODE_NAME].as<std::string>();

			// Find the SavedDataSearchType whose string representation matches the value retrieved from the yaml node.
			for (const std::pair<SavedDataSearchType, std::string>& pair : SAVED_DATA_LOCATION_STRINGS)
			{
				if (nodeValue == pair.second)
				{
					cartridge.SetSavedDataSearchType(pair.first);
					return true;
				}
			}
			
		}
		catch (std::exception e)
		{
			Logger::WriteError("Failed to load cartridge configuration. Error: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	void SaveWindowConfiguration(YAML::Node node, EmulatorWindow& window)
	{
		node[CPU_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderCPUDebugWindow;
		node[TILES_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderTilesDebugWindow;
		node[BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderBackgroundTileMapDebugWindow;
		node[WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderWindowTileMapDebugWindow;
		node[SPRITES_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderSpritesDebugWindow;
		node[LOG_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderLogWindow;
		node[SETTINGS_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderSettingsWindow;
		node[SOUND_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderSoundDebugWindow;
		node[TIMER_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderTimerDebugWindow;
		node[VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderVideoRegistersDebugWindow;
		node[JOYPAD_WINDOW_CONFIG_ITEM_NAME] = window.shouldRenderJoypadDebugWindow;
	}

	bool LoadWindowConfiguration(YAML::Node node, EmulatorWindow& window)
	{
		try
		{
			window.shouldRenderCPUDebugWindow = node[CPU_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderTilesDebugWindow = node[TILES_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderBackgroundTileMapDebugWindow = node[BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderWindowTileMapDebugWindow = node[WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderSpritesDebugWindow = node[SPRITES_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderLogWindow = node[LOG_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderSettingsWindow = node[SETTINGS_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderSoundDebugWindow = node[SOUND_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderTimerDebugWindow = node[TIMER_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderVideoRegistersDebugWindow = node[VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME].as<bool>();
			window.shouldRenderJoypadDebugWindow = node[JOYPAD_WINDOW_CONFIG_ITEM_NAME].as<bool>();
		}
		catch (std::exception e)
		{
			Logger::WriteError("Failed to load window configuration. Error: " + std::string(e.what()));
			return false;
		}

		return true;
	}

	bool Config::SaveConfiguration(const std::string& configFilePath, EmulatorWindow& window, const APU& apu, const PPU& ppu, const Joypad& joypad, const Cartridge& cartridge)
	{
		YAML::Node node;

		SaveAudioConfiguration(node, apu);
		SaveCartridgeConfiguration(node, cartridge);
		SaveWindowConfiguration(node, window);
		SaveVideoConfiguration(node, ppu);
		SaveJoypadConfiguration(node, joypad);

		auto stream = std::ofstream(configFilePath);

		if (!stream.is_open())
			return false;

		stream << node;
		return true;
	}

	bool Config::LoadConfiguration(const std::string& configFilePath, EmulatorWindow& window, APU& apu, PPU& ppu, Joypad& joypad, Cartridge& cartridge)
	{
		if (!std::filesystem::exists(configFilePath))
			return false;

		YAML::Node node = YAML::LoadFile(configFilePath);

		if (node.size() == 0)
			return false;

		bool success = true;
		if (!LoadAudioConfiguration(node, apu))
			success = false;

		if (!LoadCartridgeConfiguration(node, cartridge))
			success = false;

		if (!LoadWindowConfiguration(node, window))
			success = false;

		if (!LoadVideoConfiguration(node, ppu))
			success = false;

		if (!LoadJoypadConfiguration(node, joypad))
			success = false;

		return success;
	}
}