#pragma once
#include <string>
#include "SDL.h"
#include "Memory/MemoryMap.hpp"
#include "CPU/CPU.hpp"
#include "Graphics/PPU.hpp"
#include "Memory/Cartridge.hpp"
#include "CPU/Timer.hpp"
#include "EmulatorWindow.hpp"
#include "Logger.hpp"
#include "Input/Joypad.hpp"
#include "Input/InputManager.hpp"

namespace SHG
{
	class Emulator
	{
	public:
		bool Start();

	private:
		EmulatorWindow window = EmulatorWindow();
		InputManager inputManager = InputManager();
		MemoryMap memoryMap = MemoryMap();
		Cartridge cartridge = Cartridge();
		CPU processor = CPU(memoryMap);
		PPU ppu = PPU(memoryMap);
		Timer timer = Timer(memoryMap);
		Memory vram = Memory(8 * KiB);
		Memory wram = Memory(8 * KiB);
		Memory hram = Memory(127);
		Memory oam = Memory(160);
		Memory echoRam = Memory(122368);
		Memory restrictedMemory = Memory(130464);
		Memory ioRegisters = Memory(127);
		Register8 interruptEnableRegister;
		Register8 interruptFlagRegister;
		Memory serialOutputRegister = Memory(4);
		Joypad joypad = Joypad(inputManager);

		bool isRunning = false;
		bool isPaused = false;
		bool isStepRequested = false;
		uint32_t cyclesPerSecond = 0;

		std::string logEntries;

		void OnQuit();
		void OnPauseButtonPressed();
		void OnStepButtonPressed();
		void OnClearButtonPressed();

		void AddLogEntry(std::string logEntry, LogMessageType messageType);
		void OnFileSelected(std::string path);

		bool LoadROM(std::string& romFilePath);

		void AssignMemoryMapAddresses();
		void SetDefaultMemoryMapValues();

		void SaveConfigurationFile();
		void LoadConfigurationFile();
		void LoadConfigurationItem(std::ifstream& stream, const std::string& key, std::string& value);
		void LoadConfigurationItemAsBool(std::ifstream& stream, const std::string& key, bool& value);
		void SaveConfigurationItem(std::ofstream& stream, const std::string& key, const std::string& value);
		void SaveBoolConfigurationItem(std::ofstream& stream, const std::string& key, const bool& value);
	};
}