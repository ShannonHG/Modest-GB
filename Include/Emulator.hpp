#pragma once
#include <string>
#include "SDL.h"
#include "Memory/MemoryMap.hpp"
#include "CPU/CPU.hpp"
#include "Graphics/PPU.hpp"
#include "Memory/Cartridge.hpp"
#include "CPU/Timer.hpp"
#include "EmulatorWindow.hpp"

namespace SHG
{
	class Emulator
	{
	public:
		bool Start(const std::string& romFilePath);

	private:
		// TODO: Move this
		bool scrollToBottom = false;
		EmulatorWindow window = EmulatorWindow();
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
		Register8 joypadRegister;
		Memory serialOutputRegister = Memory(4);

		bool isRunning = false;
		bool isPaused = false;
		bool isStepRequested = false;
		uint32_t cyclesPerSecond = 0;

		std::string logEntries;

		void InitializeCPU();
		void InitializeMemoryMap();
		void OnQuit();

		void AddLogEntry(std::string logEntry);
	};
}