#pragma once
#include <map>
#include <array>
#include <queue>
#include <chrono>
#include "SDL.h"
#include "Memory/Memory.hpp"
#include "Graphics/PixelData.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Memory/MemoryMap.hpp"
#include "Graphics/DMATransferRegister.hpp"
#include "Memory/Register8.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(MemoryMap& memoryManagementUnit);
		void Step(uint32_t duration);
		void InitializeFramebuffer(SDL_Window* window);
		void Reset();
		Framebuffer& GetFramebuffer();
		Framebuffer& GetTileDebugFramebuffer();
		Framebuffer& GetSpriteDebugFramebuffer();
		Framebuffer& GetBackgroundMapDebugFramebuffer();
		Framebuffer& GetWindowMapDebugFramebuffer();
		void DebugDrawBackgroundTileMap();
		void DebugDrawWindowTileMap();
		void DebugDrawSprites();
		void DebugDrawTiles();

		Register8* GetLCDC();
		Register8* GetLCDStatus();
		Register8* GetSCY();
		Register8* GetSCX();
		Register8* GetLY();
		Register8* GetLYC();
		Register8* GetWY();
		Register8* GetWX();
		DMATransferRegister* GetDMATransferRegister();
	private:
		enum class PPUMode
		{
			HBlank = 0, // 87 - 204 cycles
			VBlank = 1, // 4560 cycles
			SearchingOAM = 2, // 80 cycles
			LCDTransfer = 3 // 172 - 289 cycles
		};

		enum class PixelFetcherState
		{
			FetchingTile,
			FetchingLowTileData,
			FetchingHighTileData,
			Sleeping,
			PushingData
		};

		enum class DMATransferState
		{
			Idle,
			InProgress
		};

		enum class TileMapType
		{
			Background = 1,
			Window = 2,
			BackgroundAndWindow = 3
		};

		struct Sprite
		{
			uint8_t x = 0;
			uint8_t y = 0;
			uint16_t tileAddress = 0;
			bool xFlip = false;
			bool yFlip = false;
			uint8_t priority = 0;
		};

		Register8 lcdControl = Register8();
		Register8 lcdStatus = Register8();
		Register8 scy = Register8();
		Register8 scx = Register8();
		Register8 ly = Register8();
		Register8 lyc = Register8();
		Register8 wy = Register8();
		Register8 wx = Register8();
		DMATransferRegister dmaRegister = DMATransferRegister();

		static const std::map<PixelFetcherState, uint32_t> STATE_DURATIONS;

		PPUMode currentMode = PPUMode::SearchingOAM;
		PixelFetcherState pixelFetcherState = PixelFetcherState::FetchingTile;
		DMATransferState dmaTransferState = DMATransferState::Idle;

		std::chrono::system_clock::time_point frameStartTime;

		uint16_t currentModeElapsedTime = 0;

		uint8_t currentScanlineX = 0;
		uint16_t currentScanlineElapsedTime = 0;

		uint16_t currentPixelFetcherTileIndex = 0;
		uint8_t currentPixelFetcherTileDataLow = 0;
		uint8_t currentPixelFetcherTileDataHigh = 0;

		std::vector<Sprite> spritesOnCurrentScanline;
		uint8_t currentSpriteIndex = 0;

		std::queue<PixelData> tileMapPixelQueue;
		std::queue<PixelData> spritePixelQueue;

		MemoryMap& memoryManagementUnit;

		uint8_t dmaTransferElapsedTime = 0;

		Framebuffer mainFramebuffer;

		Framebuffer debugWindowTileMapFramebuffer;
		Framebuffer debugBackgroundTileMapFramebuffer;
		Framebuffer debugSpriteFramebuffer;
		Framebuffer debugGenericTileFramebuffer;

		uint8_t debugBackgroundScanlineX = 0;
		uint8_t debugBackgroundScanlineY = 0;
		uint8_t debugWindowScanlineX = 0;
		uint8_t debugWindowScanlineY = 0;
		uint8_t debugSpriteIndex = 0;
		uint8_t debugSpriteScanline = 0;
		uint16_t debugTilesScanlineX = 0;
		uint16_t debugTilesScanlineY = 0;

		uint32_t ExecuteDMATransferCycle(uint32_t duration);

		int32_t GetTileAddressFromTileMaps(uint16_t tileIndex, uint8_t scanlineX, uint8_t scanlineY);
		uint16_t FetchTileMapIndex(uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, TileMapType tileMapType, bool ignoreScrolling = false);
		uint32_t TransitionToPixelFetcherState(PixelFetcherState targetState, uint32_t duration);

		uint32_t FetchSpritesOnScanline(uint8_t scanlineY, std::vector<Sprite>& sprites, uint32_t duration);
		Sprite FetchSpriteAtIndex(uint8_t index);
		void GetSpritePixelsForScanline(const std::vector<Sprite>& spritesOnScanline, uint8_t scanlineX, uint8_t scanlineY, std::queue<PixelData>& pixelQueue);

		uint32_t HandleHBlankMode(uint32_t duration);
		uint32_t HandleVBlankMode(uint32_t duration);
		uint32_t HandleLCDTransferMode(uint32_t duration);

		void EnterHBlankMode();
		void EnterVBlankMode();
		void EnterOAMSearchMode();
		void EnterLCDTransferMode();

		void ChangeStatInterruptSourceBit(uint8_t bitIndex, bool isSet);

		Color GetColorFromID(uint8_t id);
		void GetPixelsFromTileScanline(uint8_t rawScanlineDataLow, uint8_t rawScanlineDataHigh, uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, std::queue<PixelData>& pixelQueue);

		void RefreshLYCompare();

		void DebugDrawTileMap(Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanline, TileMapType tileMapType);
	};
}