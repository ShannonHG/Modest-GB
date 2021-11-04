#include <filesystem>
#include <fstream>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Emulator.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/GBMemoryMapAddresses.hpp"

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

	const uint32_t MAX_LOG_ENTRY_STRING_SIZE = 50000;

	bool Emulator::Start()
	{
		Logger::RegisterLogEntryAddedCallback(std::bind(&Emulator::AddLogEntry, this, std::placeholders::_1, std::placeholders::_2));

		LoadConfigurationFile();

		if (!window.Initialize())
		{
			Logger::WriteError("Failed to initialize the window.");
			return false;
		}

		window.RegisterFileSelectionCallback(std::bind(&Emulator::OnFileSelected, this, std::placeholders::_1));
		window.RegisterPauseButtonCallback(std::bind(&Emulator::OnPauseButtonPressed, this));
		window.RegisterStepButtonCallback(std::bind(&Emulator::OnStepButtonPressed, this));
		window.RegisterClearButtonCallback(std::bind(&Emulator::OnClearButtonPressed, this));
		inputManager.RegisterExitEventCallback(std::bind(&Emulator::OnQuit, this));\

		ppu.InitializeFramebuffer(window.GetSDLWindow());
		memoryMap.Reset();
		AssignMemoryMapAddresses();

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

	void Emulator::AssignMemoryMapAddresses()
	{
		// TODO: Use constants for addresses instead of hardcoded values

		memoryMap.AssignDeviceToAddressRange(&cartridge, 0x0000, 0x7FFF);
		memoryMap.AssignDeviceToAddressRange(&vram, 0x8000, 0x9FFF);
		memoryMap.AssignDeviceToAddressRange(&cartridge, 0xA000, 0xBFFF);
		memoryMap.AssignDeviceToAddressRange(&wram, 0xC000, 0xDFFF);
		memoryMap.AssignDeviceToAddressRange(&echoRam, 0xE000, 0xFDFF);
		memoryMap.AssignDeviceToAddressRange(&restrictedMemory, 0xFEA0, 0xFEFF);
		memoryMap.AssignDeviceToAddressRange(&oam, 0xFE00, 0xFE9F);
		memoryMap.AssignDeviceToAddressRange(&serialOutputRegister, 0xFF01, 0xFF03);
		memoryMap.AssignDeviceToAddressRange(&timer, 0xFF04, 0xFF07);
		memoryMap.AssignDeviceToAddressRange(&hram, 0xFF80, 0xFFFE);
		memoryMap.AssignDeviceToAddressRange(&interruptFlagRegister, 0xFF0F, 0xFF0F);
		memoryMap.AssignDeviceToAddressRange(&interruptEnableRegister, 0xFFFF, 0xFFFF);
		memoryMap.AssignDeviceToAddressRange(&joypad, 0xFF00, 0xFF00);

		memoryMap.AssignDeviceToAddressRange(&ioRegisters, 0xFF08, 0xFF0E);
		memoryMap.AssignDeviceToAddressRange(&ioRegisters, 0xFF10, 0xFF39);
		memoryMap.AssignDeviceToAddressRange(&ioRegisters, 0xFF4C, 0xFF7F);
		memoryMap.AssignDeviceToAddressRange(&ioRegisters, 0xFF47, 0xFF49);

		memoryMap.AssignDeviceToAddressRange(ppu.GetLCDC(), GB_LCD_CONTROL_REGISTER_ADDRESS, GB_LCD_CONTROL_REGISTER_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetLCDStatus(), GB_LCD_STATUS_REGISTER_ADDRESS, GB_LCD_STATUS_REGISTER_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetSCY(), GB_SCY_ADDRESS, GB_SCY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetSCX(), GB_SCX_ADDRESS, GB_SCX_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetLY(), GB_LY_ADDRESS, GB_LY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetLYC(), GB_LYC_ADDRESS, GB_LYC_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetWY(), GB_WY_ADDRESS, GB_WY_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetWX(), GB_WX_ADDRESS, GB_WX_ADDRESS);
		memoryMap.AssignDeviceToAddressRange(ppu.GetDMATransferRegister(), GB_DMA_TRANSFER_REGISTER_ADDRESS, GB_DMA_TRANSFER_REGISTER_ADDRESS);
	}

	void Emulator::SetDefaultMemoryMapValues()
	{
		memoryMap.Write(0xFF00, 0xCF);
		memoryMap.Write(0xFF01, 0x00);
		memoryMap.Write(0xFF02, 0x7E);
		memoryMap.Write(0xFF04, 0xAB);
		memoryMap.Write(0xFF05, 0x00);
		memoryMap.Write(0xFF06, 0x00);
		memoryMap.Write(0xFF07, 0xF8);
		memoryMap.Write(0xFF0F, 0xE1);
		memoryMap.Write(0xFF10, 0x80);
		memoryMap.Write(0xFF11, 0xBF);
		memoryMap.Write(0xFF12, 0xF3);
		memoryMap.Write(0xFF14, 0xBF);
		memoryMap.Write(0xFF16, 0x3F);
		memoryMap.Write(0xFF17, 0x00);
		memoryMap.Write(0xFF18, 0xFF);
		memoryMap.Write(0xFF19, 0xBF);
		memoryMap.Write(0xFF1A, 0x7F);
		memoryMap.Write(0xFF1B, 0xFF);
		memoryMap.Write(0xFF1C, 0x9F);
		memoryMap.Write(0xFF1D, 0xFF);
		memoryMap.Write(0xFF1E, 0xBF);
		memoryMap.Write(0xFF20, 0xFF);
		memoryMap.Write(0xFF21, 0x00);
		memoryMap.Write(0xFF22, 0x00);
		memoryMap.Write(0xFF23, 0xBF);
		memoryMap.Write(0xFF24, 0x77);
		memoryMap.Write(0xFF25, 0xF3);
		memoryMap.Write(0xFF26, 0xF1);
		memoryMap.Write(0xFF40, 0x91);
		memoryMap.Write(0xFF41, 0x85);
		memoryMap.Write(0xFF42, 0x00);
		memoryMap.Write(0xFF43, 0x00);
		memoryMap.Write(0xFF44, 0x00);
		memoryMap.Write(0xFF45, 0x00);
		memoryMap.Write(0xFF46, 0xFF);
		memoryMap.Write(0xFF47, 0xFC);
		memoryMap.Write(0xFF48, 0xFF);
		memoryMap.Write(0xFF49, 0xFF);
		memoryMap.Write(0xFF4A, 0x00);
		memoryMap.Write(0xFF4B, 0x00);
		memoryMap.Write(0xFF4D, 0xFF);
		memoryMap.Write(0xFF4F, 0xFF);
		memoryMap.Write(0xFF51, 0xFF);
		memoryMap.Write(0xFF52, 0xFF);
		memoryMap.Write(0xFF53, 0xFF);
		memoryMap.Write(0xFF54, 0xFF);
		memoryMap.Write(0xFF55, 0xFF);
		memoryMap.Write(0xFF56, 0xFF);
		memoryMap.Write(0xFF68, 0xFF);
		memoryMap.Write(0xFF69, 0xFF);
		memoryMap.Write(0xFF6A, 0xFF);
		memoryMap.Write(0xFF6B, 0xFF);
		memoryMap.Write(0xFF70, 0xFF);
		memoryMap.Write(0xFFFF, 0x00);

		// Set the lower nibble of the joypad register to read-only
		memoryMap.SetReadonlyBitMask(0xFF00, 0x0F);

		// Set LYC=LY and Mode flag bits to read-only
		memoryMap.SetReadonlyBitMask(0xFF41, 0b0000111);

		// Set LY to read-only
		memoryMap.SetReadonlyBitMask(0xFF44, 0xFF);
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

		if (!cartridge.LoadFromFile(romFilePath))
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		};

		// Temporarily clear the read-only bit masks so that default values
		// can be written to the memory map.
		processor.Reset();
		ppu.Reset();
		timer.Reset();
		SetDefaultMemoryMapValues();

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

	void Emulator::SaveConfigurationItem(std::ofstream& stream, const std::string& key, const std::string& value)
	{
		stream << key << " " << value << std::endl;
	}

	void Emulator::SaveBoolConfigurationItem(std::ofstream& stream, const std::string& key, const bool& value)
	{
		std::stringstream stringStream;
		stringStream << std::boolalpha << value;

		SaveConfigurationItem(stream, key, stringStream.str());
	}
}