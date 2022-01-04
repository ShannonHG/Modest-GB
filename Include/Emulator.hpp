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
		BasicMemory vram = BasicMemory(8 * KiB);
		BasicMemory wram = BasicMemory(8 * KiB);
		BasicMemory hram = BasicMemory(127);
		BasicMemory oam = BasicMemory(160);
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

		void OnQuit();
		void OnPauseButtonPressed();
		void OnStepButtonPressed();
		void OnClearButtonPressed();

		void AddLogEntry(std::string logEntry, LogMessageType messageType);
		void OnFileSelected(std::string path);

		bool LoadROM(std::string& romFilePath);

		void SetupMemoryMap();

		void SaveConfigurationFile();
		void LoadConfigurationFile();
		void LoadConfigurationItem(std::ifstream& stream, const std::string& key, std::string& value);
		void LoadConfigurationItemAsBool(std::ifstream& stream, const std::string& key, bool& value);
		void LoadConfigurationItemAsInt(std::ifstream& stream, const std::string& key, int& value);
		void SaveConfigurationItem(std::ofstream& stream, const std::string& key, const std::string& value);
		void SaveBoolConfigurationItem(std::ofstream& stream, const std::string& key, bool value);
		void SaveIntConfigurationItem(std::ofstream& stream, const std::string& key, int value);
	};
}