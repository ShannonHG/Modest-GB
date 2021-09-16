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
		void Step(uint32_t duration);
		void DrawBackgroundMap(Display& targetDisplay);

	private:
		PixelFetcherState pixelFetcherState = PixelFetcherState::Idle;

		uint16_t currentBackgroundTileIndex = 0;
		uint8_t currentBackgroundTileScanlineLow = 0;
		uint8_t currentBackgroundTileScanlineHigh = 0;
		
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
		void TransitionToPixelFetcherState(PixelFetcherState targetState, uint32_t& duration);
		void RenderQueuedPixels();
		void HandleHBlankEvents();
		void HandleVBlankEvents();
		uint8_t GetColorFromID(PixelColorID id);
		int32_t GetCurrentBackgroundTileAddress();

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