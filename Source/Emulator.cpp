#include <filesystem>
#include <fstream>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Emulator.hpp"
#include "Logger.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/ConfigUtils.hpp"

namespace SHG
{
	const std::string CONFIG_FILE_RELATIVE_PATH = "emulator.config";
	const uint32_t MAX_LOG_ENTRY_STRING_SIZE = 50000;

	Emulator::~Emulator()
	{
		Config::SaveConfiguration((std::filesystem::current_path() / CONFIG_FILE_RELATIVE_PATH).string(), window, apu, ppu, joypad, inputManager, cartridge);
	}

	bool Emulator::Run()
	{
		Logger::RegisterLogEntryAddedCallback(std::bind(&Emulator::AddLogEntry, this, std::placeholders::_1, std::placeholders::_2));

		if (!window.Initialize())
		{
			Logger::WriteError("Failed to initialize the window.");
			return false;
		}

		window.RegisterFileSelectionCallback(std::bind(&Emulator::OnFileSelected, this, std::placeholders::_1));
		window.RegisterPauseButtonCallback(std::bind(&Emulator::OnPauseButtonPressed, this));
		window.RegisterStepButtonCallback(std::bind(&Emulator::OnStepButtonPressed, this));
		window.RegisterClearButtonCallback(std::bind(&Emulator::OnClearButtonPressed, this));
		inputManager.RegisterGenericInputEventCallback(std::bind(&Emulator::OnInputEventReceived, this, std::placeholders::_1));

		ppu.InitializeFramebuffer(window.GetSDLWindow());
		SetupMemoryMap();
		memoryMap.Reset();

		apu.Initialize();
		inputManager.Initialize();

		Logger::WriteInfo("Loading configuration file...");
		Config::LoadConfiguration((std::filesystem::current_path() / CONFIG_FILE_RELATIVE_PATH).string(), window, apu, ppu, joypad, inputManager, cartridge);
		Logger::WriteInfo("Configuration file attributes applied.");

		window.Show();

		isRunning = true;
		double timeSinceLastFrame = 0;
		double timeSinceLastCycleCount = 0;
		uint32_t cyclesSinceLastFrame = 0;
		uint32_t cyclesSinceLastCount = 0;

		auto prevTime = std::chrono::system_clock::now();

		while (isRunning)
		{
			auto currentTime = std::chrono::system_clock::now();
			auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime).count();
			prevTime = currentTime;

			if (cartridge.IsROMLoaded() && (!isPaused || isStepRequested) && cyclesSinceLastFrame < GB_CYCLES_PER_FRAME)
			{
				uint32_t cycles = processor.Tick();

				cyclesSinceLastFrame += cycles;
				cyclesSinceLastCount += cycles;

				timer.Tick(cycles);
				ppu.Tick(cycles);
				apu.Tick(cycles);

				if (window.shouldRenderTilesDebugWindow)
					ppu.DebugDrawTiles();

				if (window.shouldRenderSpritesDebugWindow)
					ppu.DebugDrawSprites();

				if (window.shouldRenderBackgroundTileMapDebugWindow)
					ppu.DebugDrawBackgroundTileMap();

				if (window.shouldRenderWindowTileMapDebugWindow)
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

			if (timeSinceLastFrame >= GB_SECONDS_PER_FRAME)
			{
				apu.RefreshOutputDevices();
				inputManager.Update();
				window.Render(memoryMap, ppu, processor, apu, joypad, inputManager, cartridge, timer, cyclesPerSecond, logEntries);
				timeSinceLastFrame = 0;
				cyclesSinceLastFrame = 0;
			}
		}

		return true;
	}

	void Emulator::SetupMemoryMap()
	{
		memoryMap.AttachPPU(&ppu);
		memoryMap.AttachAPU(&apu);
		memoryMap.AttachCartridge(&cartridge);
		memoryMap.AttachEchoRAM(&echoRam);
		memoryMap.AttachGenericIO(&ioRegisters);
		memoryMap.AttachHRAM(&hram);
		memoryMap.AttachInterruptEnableRegister(&interruptEnableRegister);
		memoryMap.AttachInterruptFlagRegister(&interruptFlagRegister);
		memoryMap.AttachJoypadRegister(&joypad);
		memoryMap.AttachTimer(&timer);
		memoryMap.AttachWRAM(&wram);
		memoryMap.AttachRestrictedMemory(&restrictedMemory);
	}

	void Emulator::AddLogEntry(const std::string& logEntry, LogMessageType messageType)
	{
		if (messageType != LogMessageType::SystemEvent)
			logEntries += logEntry + '\n';

		if (logEntries.size() >= MAX_LOG_ENTRY_STRING_SIZE)
		{
			std::string trimmed = logEntries.substr(logEntries.size() - MAX_LOG_ENTRY_STRING_SIZE);
			size_t pos = trimmed.find_first_of('\n');

			// Start on the first complete line
			logEntries = trimmed.substr(pos + 1);
		}
	}

	void Emulator::OnInputEventReceived(SDL_Event e)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_WINDOWEVENT:
		{
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_MAXIMIZED:
				window.SetMaximizedValue(true, false);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				window.SetMaximizedValue(false, false);
				break;
			}
			break;
		}
		}
	}

	void Emulator::OnFileSelected(const std::string& path)
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

	bool Emulator::LoadROM(const std::string& romFilePath)
	{
		memoryMap.Reset();

		if (!cartridge.Load(romFilePath))
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		};

		processor.Reset();
		ppu.Reset();
		timer.Reset();
		memoryMap.Reset();
		apu.Reset();

		return true;
	}
}