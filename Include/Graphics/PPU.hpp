#pragma once
#include <array>
#include <queue>
#include "Memory/Memory.hpp"
#include "Graphics/PixelColorID.hpp"
#include "Graphics/LCDControlFlags.hpp"
#include "Graphics/LCDStatusFlags.hpp"
#include "Graphics/Display.hpp"
#include "Graphics/PixelData.hpp"
#include "Graphics/Framebuffer.hpp"
#include "LCDStatusModes.hpp"
#include "Memory/MemoryMap.hpp"
#include "Graphics/PixelFetcherState.hpp"
#include "Graphics/TileMapType.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram);
		void Step(uint32_t duration);
		void DrawTileMap(Display& display, Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, TileMapType tileMapType);

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

		uint8_t currentScanlineY = 0;
		uint8_t currentScanlineX = 0;

		Framebuffer framebuffer;

		uint16_t FetchTileIndex(uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, TileMapType tileMapType, bool ignoreScrolling=false);
		uint8_t FetchTileData(uint16_t address);
		void GetPixelsFromTileScanline(uint8_t rawScanlineDataLow, uint8_t rawScanlineDataHigh, uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, std::queue<PixelData>& pixelQueue);
		void TransitionToPixelFetcherState(PixelFetcherState targetState, uint32_t& duration);
		void RenderQueuedPixels();
		void HandleHBlankEvents();
		void HandleVBlankEvents();
		uint8_t GetColorFromID(PixelColorID id);
		int32_t GetBackgroundTileAddress(uint16_t tileIndex, uint8_t scanlineX, uint8_t scanlineY);

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