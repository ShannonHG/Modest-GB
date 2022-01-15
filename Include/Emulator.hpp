#pragma once
#include <string>
#include "SDL.h"
#include "Memory/MemoryMap.hpp"
#include "CPU/CPU.hpp"
#include "Graphics/PPU.hpp"
#include "Audio/APU.hpp"
#include "Memory/Cartridge.hpp"
#include "Timer.hpp"
#include "EmulatorWindow.hpp"
#include "Logger.hpp"
#include "Input/Joypad.hpp"
#include "Input/InputManager.hpp"
#include "Utils/MemoryUtils.hpp"

namespace SHG
{
	class Emulator
	{
	public:
		~Emulator();
		bool Run();

	private:
		EmulatorWindow window;
		InputManager inputManager;
		MemoryMap memoryMap;
		Cartridge cartridge;
		CPU processor = CPU(memoryMap);
		PPU ppu = PPU(memoryMap);
		APU apu;
		Timer timer = Timer(memoryMap);
		BasicMemory wram = BasicMemory(8 * KiB);
		BasicMemory hram = BasicMemory(127);
		BasicMemory echoRam = BasicMemory(122368);
		BasicMemory restrictedMemory = BasicMemory(130464);
		BasicMemory ioRegisters = BasicMemory(128);
		Register8 interruptEnableRegister;
		Register8 interruptFlagRegister;
		Joypad joypad = Joypad(inputManager, memoryMap);

		bool isRunning = false;
		bool isPaused = false;
		bool isStepRequested = false;
		uint32_t cyclesPerSecond = 0;

		std::string logEntries;

		void OnInputEventReceived(SDL_Event e);
		void OnPauseButtonPressed();
		void OnStepButtonPressed();
		void OnClearButtonPressed();

		void AddLogEntry(const std::string& logEntry, LogMessageType messageType);
		void OnFileSelected(const std::string& path);

		bool LoadROM(const std::string& romFilePath);

		void SetupMemoryMap();
	};
}