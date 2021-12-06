#include <filesystem>
#include <fstream>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Emulator.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"

namespace SHG
{
	const std::string CONFIG_FILE_NAME = "GBConfig.ini";
	const std::string CPU_WINDOW_CONFIG_ITEM_NAME = "IsCPUWindowOpen";
	const std::string TILES_WINDOW_CONFIG_ITEM_NAME = "IsTilesWindowOpen";
	const std::string BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME = "IsBackgroundMapWindowOpen";
	const std::string WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME = "IsWindowMapWindowOpen";
	const std::string SPRITES_WINDOW_CONFIG_ITEM_NAME = "IsSpritesWindowOpen";
	const std::string LOG_WINDOW_CONFIG_ITEM_NAME = "IsLogWindowOpen";
	const std::string VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME = "IsVideoRegisterWindowOpen";
	const std::string IO_REGISTERS_WINDOW_CONFIG_ITEM_NAME = "IsIOWindowOpen";
	const std::string WINDOW_WIDTH_CONFIG_ITEM_NAME = "Width";
	const std::string WINDOW_HEIGHT_CONFIG_ITEM_NAME = "Height";
	const std::string WINDOW_X_CONFIG_ITEM_NAME = "X";
	const std::string WINDOW_Y_CONFIG_ITEM_NAME = "Y";

	const uint32_t MAX_LOG_ENTRY_STRING_SIZE = 50000;

	bool Emulator::Run()
	{
		Logger::RegisterLogEntryAddedCallback(std::bind(&Emulator::AddLogEntry, this, std::placeholders::_1, std::placeholders::_2));

		if (!window.Initialize())
		{
			Logger::WriteError("Failed to initialize the window.");
			return false;
		}

		LoadConfigurationFile();

		window.RegisterFileSelectionCallback(std::bind(&Emulator::OnFileSelected, this, std::placeholders::_1));
		window.RegisterPauseButtonCallback(std::bind(&Emulator::OnPauseButtonPressed, this));
		window.RegisterStepButtonCallback(std::bind(&Emulator::OnStepButtonPressed, this));
		window.RegisterClearButtonCallback(std::bind(&Emulator::OnClearButtonPressed, this));
		inputManager.RegisterExitEventCallback(std::bind(&Emulator::OnQuit, this));

		ppu.InitializeFramebuffer(window.GetSDLWindow());
		SetupMemoryMap();
		memoryMap.Reset();

		isRunning = true;
		double timeSinceLastFrame = 0;
		double timeSinceLastCycleCount = 0;
		uint32_t cyclesSinceLastFrame = 0;
		uint32_t cyclesSinceLastCount = 0;

		auto prevTime = std::chrono::system_clock::now();

		std::function<void()> onQuitCallback = std::bind(&Emulator::OnQuit, this);

		while (isRunning)
		{
			auto currentTime = std::chrono::system_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime).count();
			prevTime = currentTime;

			Logger::IsSystemEventLoggingEnabled = window.IsTraceEnabled();

			if (cartridge.IsROMLoaded() && (!isPaused || isStepRequested) && cyclesSinceLastFrame < GB_CYCLES_PER_FRAME)
			{
				uint32_t cycles = processor.Step();

				cyclesSinceLastFrame += cycles;
				cyclesSinceLastCount += cycles;

				timer.Step(cycles);
				ppu.Step(cycles);

				if (window.shouldRenderTilesWindow)
					ppu.DebugDrawTiles();

				if (window.shouldRenderSpritesWindow)
					ppu.DebugDrawSprites();

				if (window.shouldRenderBackgroundTileMapWindow)
					ppu.DebugDrawBackgroundTileMap();

				if (window.shouldRenderWindowTileMapWindow)
					ppu.DebugDrawWindowTileMap();

				if (cycles > 0)
					processor.HandleInterrupts();

				isStepRequested = false;
			}

			timeSinceLastFrame += deltaTime;
			timeSinceLastCycleCount += deltaTime;

			if (timeSinceLastCycleCount >= 1)
			{
				cyclesPerSecond = cyclesSinceLastCount;
				cyclesSinceLastCount = 0;
				timeSinceLastCycleCount = 0;
			}

			if (timeSinceLastFrame >= GB_DURATION_PER_FRAME)
			{
				inputManager.Update();
				window.Render(memoryMap, ppu, processor, cyclesPerSecond, logEntries);
				timeSinceLastFrame = 0;
				cyclesSinceLastFrame = 0;
			}
		}

		return true;
	}

	void Emulator::SetupMemoryMap()
	{
		memoryMap.AttachPPU(&ppu);
		memoryMap.AttachCartridge(&cartridge);
		memoryMap.AttachEchoRAM(&echoRam);
		memoryMap.AttachGenericIO(&ioRegisters);
		memoryMap.AttachHRAM(&hram);
		memoryMap.AttachInterruptEnableRegister(&interruptEnableRegister);
		memoryMap.AttachInterruptFlagRegister(&interruptFlagRegister);
		memoryMap.AttachOAM(&oam);
		memoryMap.AttachJoypadRegister(&joypad);
		memoryMap.AttachTimer(&timer);
		memoryMap.AttachVRAM(&vram);
		memoryMap.AttachWRAM(&wram);
		memoryMap.AttachRestrictedMemory(&restrictedMemory);
	}

	void Emulator::AddLogEntry(std::string logEntry, LogMessageType messageType)
	{
		if (!window.shouldRenderLogWindow)
			return;

		if (messageType != LogMessageType::SystemEvent || window.IsTraceEnabled())
			logEntries += logEntry + '\n';

		if (logEntries.size() >= MAX_LOG_ENTRY_STRING_SIZE)
		{
			std::string trimmed = logEntries.substr(logEntries.size() - MAX_LOG_ENTRY_STRING_SIZE);
			size_t pos = trimmed.find_first_of('\n');

			// Start on the first complete line
			logEntries = trimmed.substr(pos + 1);
		}
	}

	void Emulator::OnQuit()
	{
		isRunning = false;
		SaveConfigurationFile();
	}

	void Emulator::OnFileSelected(std::string path)
	{
		if (path.empty())
			return;

		LoadROM(path);
	}

	void Emulator::OnPauseButtonPressed()
	{
		isPaused = !isPaused;
		window.SetPauseButtonLabel(isPaused ? "Resume" : "Pause");
	}

	void Emulator::OnStepButtonPressed()
	{
		isStepRequested = true;
	}

	void Emulator::OnClearButtonPressed()
	{
		logEntries.clear();
	}

	bool Emulator::LoadROM(std::string& romFilePath)
	{
		memoryMap.Reset();

		if (!cartridge.Load(romFilePath))
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		};

		// Temporarily clear the read-only bit masks so that default values
		// can be written to the memory map.
		processor.Reset();
		ppu.Reset();
		timer.Reset();
		memoryMap.Reset();

		return true;
	}

	void Emulator::SaveConfigurationFile()
	{
		auto fileStream = std::ofstream(std::filesystem::current_path().string() + "/" + CONFIG_FILE_NAME);

		SaveBoolConfigurationItem(fileStream, CPU_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderCPUWindow);
		SaveBoolConfigurationItem(fileStream, TILES_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderTilesWindow);
		SaveBoolConfigurationItem(fileStream, BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderBackgroundTileMapWindow);
		SaveBoolConfigurationItem(fileStream, WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderWindowTileMapWindow);
		SaveBoolConfigurationItem(fileStream, SPRITES_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderSpritesWindow);
		SaveBoolConfigurationItem(fileStream, LOG_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderLogWindow);
		SaveBoolConfigurationItem(fileStream, VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderVideoRegistersWindow);
		SaveBoolConfigurationItem(fileStream, IO_REGISTERS_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderIOWindow);
	/*	SaveIntConfigurationItem(fileStream, WINDOW_WIDTH_CONFIG_ITEM_NAME, window.GetWidth());
		SaveIntConfigurationItem(fileStream, WINDOW_HEIGHT_CONFIG_ITEM_NAME, window.GetHeight());
		SaveIntConfigurationItem(fileStream, WINDOW_X_CONFIG_ITEM_NAME, window.GetX());
		SaveIntConfigurationItem(fileStream, WINDOW_Y_CONFIG_ITEM_NAME, window.GetY());*/

		fileStream.close();
	}

	void Emulator::LoadConfigurationFile()
	{
		auto fileStream = std::ifstream(std::filesystem::current_path().string() + "/" + CONFIG_FILE_NAME);

		if (!fileStream.is_open())
			return;

		LoadConfigurationItemAsBool(fileStream, CPU_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderCPUWindow);
		LoadConfigurationItemAsBool(fileStream, TILES_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderTilesWindow);
		LoadConfigurationItemAsBool(fileStream, BACKGROUND_MAP_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderBackgroundTileMapWindow);
		LoadConfigurationItemAsBool(fileStream, WINDOW_MAP_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderWindowTileMapWindow);
		LoadConfigurationItemAsBool(fileStream, SPRITES_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderSpritesWindow);
		LoadConfigurationItemAsBool(fileStream, LOG_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderLogWindow);
		LoadConfigurationItemAsBool(fileStream, VIDEO_REGISTERS_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderVideoRegistersWindow);
		LoadConfigurationItemAsBool(fileStream, IO_REGISTERS_WINDOW_CONFIG_ITEM_NAME, window.shouldRenderIOWindow);

	/*	int width = 0;
		int height = 0;
		int x = 0;
		int y = 0;

		LoadConfigurationItemAsInt(fileStream, WINDOW_WIDTH_CONFIG_ITEM_NAME, width);
		LoadConfigurationItemAsInt(fileStream, WINDOW_HEIGHT_CONFIG_ITEM_NAME, height);
		LoadConfigurationItemAsInt(fileStream, WINDOW_X_CONFIG_ITEM_NAME, x);
		LoadConfigurationItemAsInt(fileStream, WINDOW_Y_CONFIG_ITEM_NAME, y);

		window.SetSize(width, height);
		window.SetPosition(x, y);*/

		fileStream.close();
	}

	void Emulator::LoadConfigurationItem(std::ifstream& stream, const std::string& key, std::string& value)
	{
		std::string line;
		while (std::getline(stream, line))
		{
			size_t pos = line.find(key);

			if (pos != std::string::npos)
			{
				// The item key and value are separated by a whitespace, 
				// so the position of the first whitespace + 1 will gives us the position of the value string.
				pos = line.find_first_of(' ');

				if (pos != std::string::npos)
				{
					value = line.substr(pos + 1);
					break;
				}
			}
		}

		// Clears the EOF bit, and moves the stream back to the beginning of the file.
		stream.clear();
		stream.seekg(0, std::ios::beg);
	}

	void Emulator::LoadConfigurationItemAsBool(std::ifstream& stream, const std::string& key, bool& value)
	{
		std::string stringValue;
		LoadConfigurationItem(stream, key, stringValue);

		std::stringstream(stringValue) >> std::boolalpha >> value;
	}

	void Emulator::LoadConfigurationItemAsInt(std::ifstream& stream, const std::string& key, int& value)
	{
		std::string stringValue;
		LoadConfigurationItem(stream, key, stringValue);

		if (stringValue.size() > 0)
			value = std::stoi(stringValue);
	}

	void Emulator::SaveConfigurationItem(std::ofstream& stream, const std::string& key, const std::string& value)
	{
		stream << key << " " << value << std::endl;
	}

	void Emulator::SaveBoolConfigurationItem(std::ofstream& stream, const std::string& key, bool value)
	{
		std::stringstream stringStream;
		stringStream << std::boolalpha << value;

		SaveConfigurationItem(stream, key, stringStream.str());
	}

	void Emulator::SaveIntConfigurationItem(std::ofstream& stream, const std::string& key, int value)
	{
		SaveConfigurationItem(stream, key, std::to_string(value));
	}
}