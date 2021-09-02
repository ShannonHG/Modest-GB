#pragma once
#include <array>
#include <queue>
#include "Memory/Memory.hpp"
#include "Graphics/PixelColorID.hpp"
#include "Graphics/LCDControlFlags.hpp"
#include "Graphics/LCDStatusFlags.hpp"
#include "Graphics/Display.hpp"
#include "Graphics/PixelData.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "LCDStatusModes.hpp"
#include "Memory/MemoryMap.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram);
		void Cycle(uint32_t duration);

	private:
		MemoryMap& memoryManagementUnit;
		Display& display;
		DataStorageDevice& vram;

		std::queue<PixelData> backgroundPixelQueue;
		std::queue<PixelData> spritePixelQueue;

		uint8_t currentScanLine = 0;

		FrameBuffer frameBuffer;

		uint8_t GetLCDControlFlag(LCDControlFlags flag);
		void SetLCDControlFlag(LCDControlFlags flag, bool value);
		uint8_t GetLCDStatusFlag(LCDStatusFlags flag);
		void SetLCDStatusFlag(LCDStatusFlags flag, bool value);
		void SetLCDStatusMode(LCDStatusModes mode);
		uint8_t GetSCY();
		uint8_t GetSCX();
		uint8_t GetLY();
		void SetLY(uint8_t value);
		uint8_t GetLYC();
		uint8_t GetWY();
		uint8_t GetWX();
		std::array<PixelData, 8> GetPixelDataFromScanLine(uint16_t scanLine);
	};
}