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

namespace ModestGB
{
	const std::string CONFIG_FILE_RELATIVE_PATH = "Modest-GB.config";

	Emulator::~Emulator()
	{
		Config::SaveConfiguration((std::filesystem::current_path() / CONFIG_FILE_RELATIVE_PATH).string(), window, apu, ppu, joypad, inputManager, cartridge);
	}

	int Emulator::Run()
	{
		Logger::RegisterLogEntryAddedCallback(std::bind(&Emulator::AddLogEntry, this, std::placeholders::_1, std::placeholders::_2));

		if (!window.Initialize())
		{
			Logger::WriteError("Failed to initialize the window.");
			return -1;
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
		double secondsSinceLastFrame = 0;
		double secondsSinceLastCycleCount = 0;
		uint32_t cyclesSinceLastFrame = 0;
		uint32_t cyclesSinceLastCount = 0;

		std::chrono::steady_clock::time_point prevTime = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point currentTime;
		double deltaTimeInSeconds = 0;

		while (isRunning)
		{
			currentTime = std::chrono::steady_clock::now();
			deltaTimeInSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime).count();
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
					ppu.DebugDrawTiles(cycles);

				if (window.shouldRenderSpritesDebugWindow)
					ppu.DebugDrawSprites(cycles);

				if (window.shouldRenderBackgroundTileMapDebugWindow)
					ppu.DebugDrawBackgroundTileMap(cycles);

				if (window.shouldRenderWindowTileMapDebugWindow)
					ppu.DebugDrawWindowTileMap(cycles);

				if (cycles > 0)
					processor.HandleInterrupts();

				isStepRequested = false;
			}

			secondsSinceLastFrame += deltaTimeInSeconds;
			secondsSinceLastCycleCount += deltaTimeInSeconds;

			if (secondsSinceLastCycleCount >= 1)
			{
				cyclesPerSecond = cyclesSinceLastCount;
				cyclesSinceLastCount = 0;
				secondsSinceLastCycleCount = 0;
			}

			if (secondsSinceLastFrame >= GB_SECONDS_PER_FRAME)
			{
				apu.RefreshOutputDevices();
				inputManager.Update();
				window.Render(memoryMap, ppu, processor, apu, joypad, inputManager, cartridge, timer, cyclesPerSecond, logEntries);
				secondsSinceLastFrame = 0;
				cyclesSinceLastFrame = 0;
			}
		}

		window.Quit();
		return 0;
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
		logEntries.push_back(logEntry);
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
		if (!cartridge.Load(romFilePath))
		{
			Logger::WriteError("Invalid ROM file provided");
			return false;
		};

		processor.Reset();
		ppu.Reset();
		timer.Reset();
		apu.Reset();
		memoryMap.Reset();

		return true;
	}
}