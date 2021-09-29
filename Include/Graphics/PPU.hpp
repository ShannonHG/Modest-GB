#pragma once
#include <map>
#include <array>
#include <queue>
#include <chrono>
#include "Memory/Memory.hpp"
#include "Graphics/PixelColorID.hpp"
#include "Graphics/Display.hpp"
#include "Graphics/PixelData.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Memory/MemoryMap.hpp"
#include "Graphics/TileMapType.hpp"
#include "Graphics/DMATransferRegister.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram, DMATransferRegister& dmaRegister);
		void Cycle(uint32_t duration);
		void DebugDrawBackgroundTileMap();
		void DebugDrawWindowTileMap();
		void DebugDrawSprites();
		void DebugDrawTiles();
		void AttachDisplayForWindowDebugging(Display* display);
		void AttachDisplayForBackgroundDebugging(Display* display);
		void AttachDisplayForSpriteDebugging(Display* display);
		void AttachDisplayForTileDebugging(Display* display);
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

		struct Sprite
		{
			uint8_t x = 0;
			uint8_t y = 0;
			uint16_t tileAddress = 0;
			bool xFlip = false;
			bool yFlip = false;
			uint8_t priority = 0;
		};

		static const std::map<PixelFetcherState, uint32_t> STATE_DURATIONS;

		PPUMode currentMode = PPUMode::SearchingOAM;
		PixelFetcherState pixelFetcherState = PixelFetcherState::FetchingTile;
		DMATransferState dmaTransferState = DMATransferState::Idle;

		std::chrono::system_clock::time_point frameStartTime;

		uint16_t currentModeElapsedTime = 0;

		uint8_t currentScanline = 0;
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
		Display& mainDisplay;
		DataStorageDevice& vram;

		DMATransferRegister& dmaRegister;
		uint8_t dmaTransferElapsedTime = 0;

		Framebuffer mainFramebuffer;

		Display* debugWindowTileMapDisplay;
		Display* debugBackgroundTileMapDisplay;
		Display* debugSpriteDisplay;
		Display* debugGenericTileDisplay;

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

		void ExecutePixelFetcherCycle(uint32_t& duration);
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

		void DrawFrameBuffer();

		uint8_t GetColorFromID(PixelColorID id);
		void GetPixelsFromTileScanline(uint8_t rawScanlineDataLow, uint8_t rawScanlineDataHigh, uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, std::queue<PixelData>& pixelQueue);

		void RefreshLY();
		void RefreshLYCompare();

		void ChangeLCDControlBit(uint8_t bitIndex, bool isSet);
		bool GetLCDControlBit(uint8_t bitIndex);
		void ChangeLCDStatusBit(uint8_t bitIndex, bool isSet);
		bool GetLCDStatusBit(uint8_t bitIndex);

		void DebugDrawTileMap(Display& display, Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanline, TileMapType tileMapType);
	};
}