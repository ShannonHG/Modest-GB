#pragma once
#include <cstdint>
#include <map>
#include "SDL.h"
#include "Graphics/Framebuffer.hpp"
#include "Memory/Memory.hpp"
#include "Memory/Register8.hpp"
#include "Graphics/Sprite.hpp"
#include "Graphics/BackgroundPixelFetcher.hpp"
#include "Graphics/SpritePixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Graphics/DMATransferRegister.hpp"
#include "Memory/BasicMemory.hpp"

namespace SHG
{
	class PPU
	{
	public:
		PPU(Memory& memoryMap);
		void Tick(uint32_t cycles);
		void Reset();
		void InitializeFramebuffer(SDL_Window* window);
		void DebugDrawBackgroundTileMap();
		void DebugDrawWindowTileMap();
		void DebugDrawSprites();
		void DebugDrawTiles();

		void SetPaletteTint(uint16_t paletteAddress, uint8_t colorIndex, Color color);
		Color GetPaletteTint(uint16_t paletteAddress, uint8_t colorIndex) const;

		const Framebuffer& GetPrimaryFramebuffer() const;
		const Framebuffer& GetTileDebugFramebuffer() const;
		const Framebuffer& GetSpriteDebugFramebuffer() const;
		const Framebuffer& GetBackgroundMapDebugFramebuffer() const;
		const Framebuffer& GetWindowMapDebugFramebuffer() const;

		void WriteLCDC(uint8_t value);
		void WriteLCDSTAT(uint8_t value);
		void WriteSCY(uint8_t value);
		void WriteSCX(uint8_t value);
		void WriteLY(uint8_t value);
		void WriteLYC(uint8_t value);
		void WriteWY(uint8_t value);
		void WriteWX(uint8_t value);
		void WriteDMA(uint8_t value);
		void WriteToVRAM(uint16_t address, uint8_t value);
		void WriteToOAM(uint16_t address, uint8_t value);

		uint8_t ReadLCDC() const;
		uint8_t ReadLCDSTAT() const;
		uint8_t ReadSCY() const;
		uint8_t ReadSCX() const;
		uint8_t ReadLY() const;
		uint8_t ReadLYC() const;
		uint8_t ReadWY() const;
		uint8_t ReadWX() const;
		uint8_t ReadDMA() const;
		uint8_t ReadVRAM(uint16_t address) const;
		uint8_t ReadOAM(uint16_t address) const;

	private:
		enum class Mode
		{
			HBlank,
			VBlank,
			SearchingOAM,
			LCDTransfer,
		};

		enum class DMATransferState
		{
			Idle,
			InProgress
		};

		std::map<uint16_t, std::array<Color, 4>> paletteTints;

		Mode currentMode = Mode::SearchingOAM;
		uint32_t currentScanlineElapsedCycles = 0;

		std::vector<Sprite> spritesOnCurrentScanline;
		std::queue<Pixel> queuedSpritePixels;
		int16_t currentScanlineX = 0;
		Memory* memoryMap;
		Register8 lcdc;
		Register8 stat;
		Register8 scy;
		Register8 scx;
		Register8 ly;
		Register8 lyc;
		Register8 wy;
		Register8 wx;
		BasicMemory oam = BasicMemory(160);
		BasicMemory vram = BasicMemory(8 * KiB);
		Register8 statInterruptLine;
		DMATransferRegister dmaRegister;
		Framebuffer primaryFramebuffer;
		Framebuffer tileDebugFramebuffer;
		Framebuffer spriteDebugFramebuffer;
		Framebuffer backgroundDebugFramebuffer;
		Framebuffer windowDebugFramebuffer;
		BackgroundPixelFetcher backgroundPixelFetcher;
		SpritePixelFetcher spritePixelFetcher;
		DMATransferState currentDMATransferState = DMATransferState::Idle;
		uint32_t currentDMATransferElapsedTime = 0;
		uint8_t numberOfPixelsToIgnore = 0;
		uint8_t ignoredPixels = 0;

		bool wasWYConditionTriggered = false;
		bool wasWXConditionTriggered = false;
		uint8_t windowLineCounter = 0;

		uint8_t debugBackgroundMapScanlineX = 0;
		uint8_t debugBackgroundMapScanlineY = 0;
		uint8_t debugWindowMapScanlineX = 0;
		uint8_t debugWindowMapScanlineY = 0;
		uint8_t debugCurrentSpriteIndex = 0;
		uint8_t debugSpriteScanline = 0;
		uint8_t debugTileScanline = 0;
		uint16_t debugTileIndex = 0;

		void SetCurrentMode(Mode mode);
		void ChangeStatInterruptLineBit(uint8_t bitIndex, bool value);

		void EnterHBlankMode();
		void UpdateHBlankMode(uint32_t& cycles);

		void EnterVBlankMode();
		void UpdateVBlankMode(uint32_t& cycles);

		void EnterOAMSearchMode();
		void UpdateOAMSearchMode(uint32_t& cycles);

		void EnterLCDTransferMode();
		void UpdateLCDTransferMode(uint32_t& cycles);

		void RefreshLYCFlag();
		Sprite GetSpriteAtIndex(uint8_t index);

		void UpdateDMATransferProcess(uint32_t cycles);
		void RenderPixel(const Pixel& pixel);

		
		
		uint8_t NormalizedReadFromOAM(uint16_t address);
		void NormalizedWriteToOAM(uint16_t address, uint8_t value);

		void DebugDrawTileMap(Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, bool useAlternateTileMapAddress, TileMapType tileMapType);
	};
}