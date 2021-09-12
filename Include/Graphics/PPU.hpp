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
#include "Graphics/PixelFetcherState.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram);
		void Cycle(uint32_t duration);
		void Step(uint32_t duration);

	private:
		PixelFetcherState pixelFetcherState;

		uint16_t currentTileIndex = 0;
		uint8_t currentTileScanlineLow = 0;
		uint8_t currentTileScanlineHigh = 0;
		
		std::queue<PixelData> backgroundPixelQueue;
		std::queue<PixelData> spritePixelQueue;

		MemoryMap& memoryManagementUnit;
		Display& display;
		DataStorageDevice& vram;

		uint8_t currentScanline = 0;
		uint8_t currentX = 0;

		FrameBuffer frameBuffer;

		void FetchTileIndex();
		void FetchLowTileData();
		void FetchHighTileData();
		void PushPixelsToBackgroundPixelQueue();

		void ProcessBackgroundAndWindowTiles(int scanline, std::queue<PixelData>& pixelQueue);
		void ProcessSpriteTiles(int scanline, std::queue<PixelData>& pixelQueue);

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
		std::array<PixelData, 8> GetPixelDataFromScanline(uint16_t scanLine);
	};
}