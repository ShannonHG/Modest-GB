#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Memory/Memory.hpp"
#include "Graphics/DMATransferRegister.hpp"
#include "Timer.hpp"
#include "Memory/BasicMemory.hpp"
#include "Memory/Memory.hpp"
#include "Memory/Cartridge.hpp"
#include "Graphics/PPU.hpp"
#include "Audio/APU.hpp"
#include "Input/Joypad.hpp"

namespace SHG
{
	using MemoryMapWriteCallback = std::function<void(uint16_t address, uint8_t value)>;

	class MemoryMap : public Memory
	{
	public:
		uint8_t Read(uint16_t address) const override;
		void Write(uint16_t address, uint8_t value) override;
		void Reset() override;

		void AttachCartridge(Cartridge* cartridge);
		void AttachPPU(PPU* ppu);
		void AttachAPU(APU* ppu);
		void AttachTimer(Timer* timer);
		void AttachVRAM(BasicMemory* vram);
		void AttachWRAM(BasicMemory* wram);
		void AttachHRAM(BasicMemory* hram);
		void AttachEchoRAM(BasicMemory* echoRam);
		void AttachGenericIO(BasicMemory* ioRegisters);
		void AttachInterruptEnableRegister(Register8* interruptEnableRegister);
		void AttachInterruptFlagRegister(Register8* interruptFlagRegister);
		void AttachJoypadRegister(Joypad* joypadRegister);
		void AttachRestrictedMemory(BasicMemory* restrictedMem);

	private:
		Cartridge* cartridge;
		PPU* ppu;
		APU* apu;
		Timer* timer;
		BasicMemory* wram;
		BasicMemory* hram;
		BasicMemory* echoRam;
		BasicMemory* ioRegisters;
		Register8* interruptEnableRegister;
		Register8* interruptFlagRegister;
		Joypad* joypad;
		BasicMemory* restrictedMemory;

		uint8_t ReadIO(uint16_t address) const;
		void WriteIO(uint16_t address, uint8_t value);
		uint8_t WriteWithReadOnlyBits(uint8_t destination, uint8_t value, uint8_t bitMask);
	};
}