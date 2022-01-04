#include <string>
#include <cassert>
#include "Graphics/PPU.hpp"
#include "Utils/Interrupts.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/MemoryUtils.hpp"
#include "Utils/Arithmetic.hpp"
#include "Utils/DataConversions.hpp"

namespace SHG
{
	const std::string PPU_MESSAGE_HEADER = "[PPU]";

	const uint8_t SPRITE_DEBUG_FRAMEBUFFER_WIDTH = 89;
	const uint8_t SPRITE_DEBUG_FRAMEBUFFER_HEIGHT = 71;

	const uint8_t TILE_DEBUG_FRAMEBUFFER_WIDTH_IN_PIXELS = 215;
	const uint8_t TILE_DEBUG_FRAMEBUFFER_HEIGHT_IN_PIXELS = 143;
	const uint8_t TILE_DEBUG_FRAMEBUFFER_WIDTH_IN_TILES = 24;
	const uint8_t TILE_DEBUG_FRAMEBUFFER_HEIGHT_IN_TILES = 16;
	const Color SCROLL_BOX_DEBUG_COLOR = { 0, 100, 170, 255 };

	const int16_t DEFAULT_SCANLINE_X = -7;

	PPU::PPU(Memory& memoryMap) : memoryMap(memoryMap),
		backgroundPixelFetcher(&memoryMap, &lcdc, &scx, &scy, &wx, &wy),
		spritePixelFetcher(&memoryMap, &lcdc, &backgroundPixelFetcher)
	{
		paletteTints =
		{
			{ GB_BACKGROUND_PALETTE_ADDRESS, {RGBA_WHITE, RGBA_WHITE, RGBA_WHITE, RGBA_WHITE}},
			{ GB_SPRITE_PALETTE_0_ADDRESS, {RGBA_WHITE, RGBA_WHITE, RGBA_WHITE, RGBA_WHITE}},
			{ GB_SPRITE_PALETTE_1_ADDRESS, {RGBA_WHITE, RGBA_WHITE, RGBA_WHITE, RGBA_WHITE}},
		};
	}

	void PPU::InitializeFramebuffer(SDL_Window* window)
	{
		primaryFramebuffer = Framebuffer(window, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
		backgroundDebugFramebuffer = Framebuffer(window, TILE_MAP_WIDTH_IN_PIXELS, TILE_MAP_HEIGHT_IN_PIXELS);
		windowDebugFramebuffer = Framebuffer(window, TILE_MAP_WIDTH_IN_PIXELS, TILE_MAP_HEIGHT_IN_PIXELS);
		spriteDebugFramebuffer = Framebuffer(window, SPRITE_DEBUG_FRAMEBUFFER_WIDTH, SPRITE_DEBUG_FRAMEBUFFER_HEIGHT);
		tileDebugFramebuffer = Framebuffer(window, TILE_DEBUG_FRAMEBUFFER_WIDTH_IN_PIXELS, TILE_DEBUG_FRAMEBUFFER_HEIGHT_IN_PIXELS);
	}

	void PPU::Reset()
	{
		currentMode = Mode::SearchingOAM;
		currentScanlineX = DEFAULT_SCANLINE_X;
		currentScanlineElapsedCycles = 0;

		spritesOnCurrentScanline.clear();

		dmaRegister.ClearPendingTransfer();

		primaryFramebuffer.Clear();
		primaryFramebuffer.UploadData();

		backgroundDebugFramebuffer.Clear();
		backgroundDebugFramebuffer.UploadData();

		spriteDebugFramebuffer.Clear(RGBA_BLACK);
		spriteDebugFramebuffer.UploadData();

		tileDebugFramebuffer.Clear(RGBA_BLACK);
		tileDebugFramebuffer.UploadData();
	}

	void PPU::WriteLCDC(uint8_t value)
	{
		lcdc.Write(value);
	}

	void PPU::WriteLCDSTAT(uint8_t value)
	{
		stat.Write(value);
	}

	void PPU::WriteSCY(uint8_t value)
	{
		scy.Write(value);
	}

	void PPU::WriteSCX(uint8_t value)
	{
		scx.Write(value);
	}

	void PPU::WriteLY(uint8_t value)
	{	
		ly.Write(value);
	}

	void PPU::WriteLYC(uint8_t value)
	{
		lyc.Write(value);
	}

	void PPU::WriteWY(uint8_t value)
	{
		wy.Write(value);
	}

	void PPU::WriteWX(uint8_t value)
	{
		wx.Write(value);
	}

	void PPU::WriteDMA(uint8_t value)
	{
		dmaRegister.Write(value);
	}

	uint8_t PPU::ReadLCDC() const
	{
		return lcdc.Read();
	}

	uint8_t PPU::ReadLCDSTAT() const
	{
		return stat.Read();
	}

	uint8_t PPU::ReadSCY() const
	{
		return scy.Read();
	}

	uint8_t PPU::ReadSCX() const
	{
		return scx.Read();
	}

	uint8_t PPU::ReadLY() const
	{
		return ly.Read();
	}

	uint8_t PPU::ReadLYC() const
	{
		return lyc.Read();
	}
	uint8_t PPU::ReadWY() const
	{
		return wy.Read();
	}

	uint8_t PPU::ReadWX() const
	{
		return wx.Read();
	}

	uint8_t PPU::ReadDMA() const
	{
		return dmaRegister.Read();
	}

	void PPU::SetPaletteTint(uint16_t paletteAddress, uint8_t colorIndex, Color color)
	{
		if (paletteTints.find(paletteAddress) == paletteTints.end())
		{
			Logger::WriteError("Invalid palette address: " + GetHexString16(paletteAddress) , PPU_MESSAGE_HEADER);
			return;
		}

		if (colorIndex > 3)
		{
			Logger::WriteError("Invalid color index: " + std::to_string(colorIndex), PPU_MESSAGE_HEADER);
			return;
		}

		paletteTints[paletteAddress][colorIndex] = color;
	}

	Color PPU::GetPaletteTint(uint16_t paletteAddress, uint8_t colorIndex) const
	{
		if (paletteTints.find(paletteAddress) == paletteTints.end())
		{
			Logger::WriteError("Invalid palette address: " + GetHexString16(paletteAddress), PPU_MESSAGE_HEADER);
			return RGBA_WHITE;
		}

		if (colorIndex > 3)
		{
			Logger::WriteError("Invalid color index: " + std::to_string(colorIndex), PPU_MESSAGE_HEADER);
			return RGBA_WHITE;
		}

		return paletteTints.at(paletteAddress)[colorIndex];
	}

	void PPU::Tick(uint32_t cycles)
	{
		while (cycles > 0)
		{
			// TODO: Confirm logic
			if (!lcdc.Read(LCDC_PPU_ENABLE_BIT_INDEX))
			{
				SetCurrentMode(Mode::HBlank);
				stat.ChangeBit(STAT_LYC_FLAG_INDEX, 0);
				ly.Write(0);

				currentDMATransferState = DMATransferState::Idle;

				currentScanlineX = DEFAULT_SCANLINE_X;
				currentScanlineElapsedCycles = 0;

				spritesOnCurrentScanline.clear();
				backgroundPixelFetcher.Reset();
				spritePixelFetcher.Reset();
				return;
			}

			UpdateDMATransferProcess(cycles);

			switch (currentMode)
			{
			case Mode::HBlank:
				UpdateHBlankMode(&cycles);
				break;
			case Mode::VBlank:
				UpdateVBlankMode(&cycles);
				break;
			case Mode::SearchingOAM:
				UpdateOAMSearchMode(&cycles);
				break;
			case Mode::LCDTransfer:
				UpdateLCDTransferMode(&cycles);
				break;
			}

			// The scanline may have been changed, so update the STAT register's LYC=LY flag 
			// and the STAT interrupt line if needed.
			RefreshLYCFlag();
		}
	}

	void PPU::EnterHBlankMode()
	{
		SetCurrentMode(Mode::HBlank);
		ChangeStatInterruptLineBit(STAT_HBLANK_INTERRUPT_SOURCE_BIT_INDEX, true);
	}

	void PPU::UpdateHBlankMode(uint32_t* cycles)
	{
		assert(cycles != nullptr);

		uint32_t newElapsedCycles = currentScanlineElapsedCycles + *cycles;
		if (newElapsedCycles >= SCANLINE_DURATION_IN_CYCLES)
		{
			// Restart the count since a new scanline is about to begin.
			currentScanlineElapsedCycles = 0;

			ly.Increment();
			currentScanlineX = DEFAULT_SCANLINE_X;
			ChangeStatInterruptLineBit(STAT_HBLANK_INTERRUPT_SOURCE_BIT_INDEX, false);

			if (ly.Read() == GB_SCREEN_HEIGHT)
				EnterVBlankMode();
			else
				EnterOAMSearchMode();

			// We only need enough cycles to reach 456, so any remaining cycles should be return.
			*cycles = (newElapsedCycles - SCANLINE_DURATION_IN_CYCLES);
			return;
		}

		currentScanlineElapsedCycles = newElapsedCycles;
		*cycles = 0;
	}


	void PPU::EnterVBlankMode()
	{
		SetCurrentMode(Mode::VBlank);
		ChangeStatInterruptLineBit(STAT_VBLANK_INTERRUPT_SOURCE_BIT_INDEX, true);

		RequestInterrupt(memoryMap, InterruptType::VBlank);

		// Causes the window to render the most up to date version of the framebuffer.
		primaryFramebuffer.UploadData();
		primaryFramebuffer.Clear();

	}

	void PPU::UpdateVBlankMode(uint32_t* cycles)
	{
		assert(cycles != nullptr);

		uint32_t newElapsedCycles = currentScanlineElapsedCycles + *cycles;
		if (newElapsedCycles >= SCANLINE_DURATION_IN_CYCLES)
		{
			// Reset the elapsed cycles, since a new scanline is about to be started.
			currentScanlineElapsedCycles = 0;

			ly.Increment();
			if (ly.Read() == VBLANK_END_LINE)
			{
				// TODO: Is this correct?
				backgroundPixelFetcher.Reset();
				spritePixelFetcher.Reset();
				ly.Write(0);
				wasWYConditionTriggered = false;
				wasWXConditionTriggered = false;
				windowLineCounter = 0;

				ChangeStatInterruptLineBit(STAT_VBLANK_INTERRUPT_SOURCE_BIT_INDEX, false);
				EnterOAMSearchMode();
			}

			// We only need enough cycles to reach 456, so any remaining cycles should be returned.
			*cycles = newElapsedCycles - SCANLINE_DURATION_IN_CYCLES;
			return;
		}

		currentScanlineElapsedCycles = newElapsedCycles;
		*cycles = 0;
	}


	void PPU::EnterOAMSearchMode()
	{
		if (!wasWYConditionTriggered)
			wasWYConditionTriggered = wy.Read() == ly.Read();

		numberOfPixelsToIgnore = scx.Read() & 0b111;
		ignoredPixels = 0;
		spritesOnCurrentScanline.clear();
		SetCurrentMode(Mode::SearchingOAM);
		ChangeStatInterruptLineBit(STAT_OAM_INTERRUPT_SOURCE_BIT_INDEX, true);

	}

	void PPU::UpdateOAMSearchMode(uint32_t* cycles)
	{
		assert(cycles != nullptr);

		uint32_t previousElapsedCycles = Arithmetic::EvenCeil(currentScanlineElapsedCycles);
		currentScanlineElapsedCycles = std::min(static_cast<int>(currentScanlineElapsedCycles + *cycles), static_cast<int>(OAM_SEARCH_MODE_DURATION_IN_CYCLES));

		uint32_t targetCycles = Arithmetic::EvenCeil(currentScanlineElapsedCycles);
		for (uint32_t cycle = previousElapsedCycles; cycle < targetCycles; cycle += 2)
		{
			if (spritesOnCurrentScanline.size() < MAX_SPRITES_PER_SCANLINE)
			{
				// Each sprite fetch takes 2 cycles, and the elapsed cycles for the current scanline
				// is guaranteed to be 0 at the start of OAM search mode (since it's always after HBlank or VBlank). 
				// Due to this, the index of the current sprite that should be fetched can be determined by 
				// dividing the scanline's elapsed cycles by 2.
				uint8_t spriteIndex = cycle / 2;

				Sprite sprite = GetSpriteAtIndex(spriteIndex);
				uint8_t spriteSize = lcdc.Read(LCDC_OBJ_SIZE_BIT_INDEX) ? MAX_SPRITE_HEIGHT_IN_PIXELS : MIN_SPRITE_HEIGHT_IN_PIXELS;

				// If the sprite is on the current scanline, then added it to the list.
				if (ly.Read() >= sprite.y && ly.Read() < sprite.y + spriteSize)
				{
					spritesOnCurrentScanline.push_back(sprite);
				}
			}
		}

		(*cycles) = 0;
		if (currentScanlineElapsedCycles >= OAM_SEARCH_MODE_DURATION_IN_CYCLES)
		{
			EnterLCDTransferMode();
			ChangeStatInterruptLineBit(STAT_OAM_INTERRUPT_SOURCE_BIT_INDEX, false);
		}
	}

	Sprite PPU::GetSpriteAtIndex(uint8_t index)
	{
		// Each sprite has 4-bytes of attribute data in OAM.
		uint16_t attributeAddress = GB_OAM_START_ADDRESS + (index * SPRITE_ATTRIBUTE_BYTE_COUNT);
		uint8_t flags = memoryMap.Read(attributeAddress + 3);

		Sprite sprite =
		{
			// OAM contains the sprite's X position plus 8, so to get the true X position we need to subtract 8.
			.x = static_cast<int16_t>(memoryMap.Read(attributeAddress + 1) - TILE_WIDTH_IN_PIXELS),

			// OAM contains the sprite's 16 position plus 16, so to get the true Y position we need to subtract 16.
			.y = static_cast<int16_t>(memoryMap.Read(attributeAddress) - MAX_SPRITE_HEIGHT_IN_PIXELS),

			// In 8x16 mode, the least significant bit of the tile index should be ignored to ensure that it points to the first/top tile of the sprite.
			.tileIndex = static_cast<uint8_t>(lcdc.Read(LCDC_OBJ_SIZE_BIT_INDEX) ? memoryMap.Read(attributeAddress + 2) & 0xFE : memoryMap.Read(attributeAddress + 2)),

			// Flags 
			.xFlip = static_cast<bool>((flags >> 5) & 1),
			.yFlip = static_cast<bool>((flags >> 6) & 1),
			.palette = static_cast<uint8_t>((flags >> 4) & 1),
			.backgroundOverSprite = static_cast<bool>((flags >> 7) & 1),
			.oamIndex = index
		};

		return sprite;
	}

	void PPU::EnterLCDTransferMode()
	{
		backgroundPixelFetcher.Reset();
		backgroundPixelFetcher.SetY(ly.Read());
		backgroundPixelFetcher.SetMode(BackgroundPixelFetcherMode::Background);

		spritePixelFetcher.Reset();
		spritePixelFetcher.SetSpritesOnScanline(spritesOnCurrentScanline);
		spritePixelFetcher.SetY(ly.Read());
		SetCurrentMode(Mode::LCDTransfer);
	}

	void PPU::UpdateLCDTransferMode(uint32_t* cycles)
	{
		while ((*cycles) > 0)
		{
			wasWXConditionTriggered = currentScanlineX + 7 == wx.Read();

			// If all of the conditions to render the window have been satisfied, then reset the background
			// pixel fetcher, and set it to 'Window' mode.
			if (backgroundPixelFetcher.GetCurrentMode() != BackgroundPixelFetcherMode::Window &&
				wasWXConditionTriggered &&
				wasWYConditionTriggered &&
				lcdc.Read(LCDC_WINDOW_ENABLE_BIT_INDEX))
			{
				backgroundPixelFetcher.Reset();
				backgroundPixelFetcher.SetY(windowLineCounter);
				backgroundPixelFetcher.SetMode(BackgroundPixelFetcherMode::Window);
				windowLineCounter++;
			}

			// The current scanline X position only needs to support negative positions so 
			// that the "WX Trigger" condition can be passed when WX is between 0 and 6.
			// However, the pixel fetchers should not be ticked until the scanline X position is 0 or greater.
			if (currentScanlineX < 0)
			{
				currentScanlineX++;
				continue;
			}

			(*cycles)--;
			currentScanlineElapsedCycles++;

			SpritePixelFetcherState prevSpritePixelFetcherState = spritePixelFetcher.GetState();
			if (lcdc.Read(LCDC_OBJ_ENABLE_BIT_INDEX))
			{
				spritePixelFetcher.SetX(currentScanlineX);
				spritePixelFetcher.Tick();
			}

			if (!(prevSpritePixelFetcherState == SpritePixelFetcherState::Idle &&
				spritePixelFetcher.GetState() == SpritePixelFetcherState::Idle))
				return;

			Pixel selectedPixel;
			bool isSpritePixelAvailable = spritePixelFetcher.GetPixelQueueSize() != 0 && lcdc.Read(LCDC_OBJ_ENABLE_BIT_INDEX);

			if (lcdc.Read(LCDC_BG_WINDOW_ENABLE_BIT_INDEX))
			{
				backgroundPixelFetcher.Tick();

				// If the background is enabled, pixels will only be pushed to the framebuffer when the background pixel fetcher is not empty.
				if (backgroundPixelFetcher.GetPixelQueueSize() > TILE_WIDTH_IN_PIXELS)
				{
					selectedPixel = backgroundPixelFetcher.PopPixel();

					// TODO: Add explanation
					if (backgroundPixelFetcher.GetCurrentMode() == BackgroundPixelFetcherMode::Background && ignoredPixels < numberOfPixelsToIgnore)
					{
						ignoredPixels++;
						continue;
					}

					if (isSpritePixelAvailable)
					{
						Pixel spritePixel = spritePixelFetcher.PopPixel();

						// Compare the sprite pixel against the currently selected background pixel to determine which should be rendered.
						// If the sprite pixel is not transparent, and the background pixel uses color 0 or the sprite pixel does not allow 
						// the background to be drawn on top, then the sprite pixel should be drawn. Otherwise, the background pixel should be rendered.
						if (spritePixel.colorIndex != 0 && (selectedPixel.colorIndex == 0 || !spritePixel.backgroundOverSprite))
						{
							selectedPixel = spritePixel;
						}
					}

					// Render the pixel and increase the X coordinate of the current scanline by 1.
					RenderPixel(selectedPixel);
				}
			}
			else
			{
				// If the background is disabled, then check if there are any sprite pixels that should be rendered.
				if (isSpritePixelAvailable)
					selectedPixel = spritePixelFetcher.PopPixel();

				// Render the pixel and increase the X coordinate of the current scanline by 1.
				RenderPixel(selectedPixel);
			}

			if (currentScanlineX >= GB_SCREEN_WIDTH)
			{
				EnterHBlankMode();
				break;
			}
		}
	}

	void PPU::RenderPixel(const Pixel& pixel)
	{
		Color color = GetColorFromColorIndex(pixel.colorIndex, memoryMap.Read(pixel.paletteAddress));

		// Convert color components to the 0 - 1 range so they can be 
		// multiplied with the palette tint's components to produce the final color.
		float floatRed = color.r / 255.0f;
		float floatGreen = color.g / 255.0f;
		float floatBlue = color.b / 255.0f;

		color =
		{
			static_cast<uint8_t>(floatRed * paletteTints[pixel.paletteAddress][pixel.colorIndex].r),
			static_cast<uint8_t>(floatGreen * paletteTints[pixel.paletteAddress][pixel.colorIndex].g),
			static_cast<uint8_t>(floatBlue * paletteTints[pixel.paletteAddress][pixel.colorIndex].b)
		};

		primaryFramebuffer.SetPixel(currentScanlineX, ly.Read(), color);
		currentScanlineX++;
	}

	void PPU::SetCurrentMode(Mode mode)
	{
		currentMode = mode;

		// Bits 0 and 1 represent the current mode of the PPU.
		// HBlank = 00
		// VBlank = 01
		// OAM Search = 10
		// LCD Transfer = 11
		stat.ChangeBit(0, static_cast<uint8_t>(mode) & 1);
		stat.ChangeBit(1, static_cast<uint8_t>(mode) & 2);
	}

	void PPU::ChangeStatInterruptLineBit(uint8_t bitIndex, bool value)
	{
		// If the STAT interrupt source is not enabled, then the interrupt line should not be changed.
		if (!stat.Read(bitIndex))
			return;

		// The STAT interrupt sources (LYC=LY, OAM Search, HBlank, VBlank), have their
		// states logically OR'ed into the STAT interrupt line. If a source sets its bit to 1,
		// and the STAT interrupt line is currently 0, then a STAT interrupt will be triggered.
		if (!statInterruptLine.Read() && value)
			RequestInterrupt(memoryMap, InterruptType::LCDStat);

		statInterruptLine.ChangeBit(bitIndex, value);
	}

	void PPU::RefreshLYCFlag()
	{
		bool result = lyc.Read() == ly.Read();
		stat.ChangeBit(STAT_LYC_FLAG_INDEX, result);

		ChangeStatInterruptLineBit(STAT_LYC_INTERRUPT_SOURCE_BIT_INDEX, result);
	}

	void PPU::UpdateDMATransferProcess(uint32_t cycles)
	{
		if (dmaRegister.IsTransferPending())
		{
			dmaRegister.ClearPendingTransfer();
			currentDMATransferState = DMATransferState::InProgress;
		}

		if (currentDMATransferState == DMATransferState::InProgress)
		{
			for (uint32_t i = 0; i < cycles; i++)
			{
				// A DMA transfer causes data to be transfer from $XX00-$XX9F to $FFE0-$FE9F.
				// Since a DMA transfer lasts 160 cycles and also transfers 160 bytes, the elapsed DMA transfer time
				// can be used as the offset from the source and destination addresses.
				uint8_t sourceData = memoryMap.Read(dmaRegister.GetSourceStartAddress() + currentDMATransferElapsedTime);
				memoryMap.Write(DMA_TRANSFER_DESTINATION_START_ADDRESS + currentDMATransferElapsedTime, sourceData);

				currentDMATransferElapsedTime++;

				if (currentDMATransferElapsedTime == DMA_TRANSFER_DURATION)
				{
					currentDMATransferState = DMATransferState::Idle;
					currentDMATransferElapsedTime = 0;
					break;
				}
			}
		}
	}

	const Framebuffer& PPU::GetPrimaryFramebuffer() const
	{
		return primaryFramebuffer;
	}

	const Framebuffer& PPU::GetTileDebugFramebuffer() const
	{
		return tileDebugFramebuffer;
	}

	const Framebuffer& PPU::GetSpriteDebugFramebuffer() const
	{
		return spriteDebugFramebuffer;
	}

	const Framebuffer& PPU::GetBackgroundMapDebugFramebuffer() const
	{
		return backgroundDebugFramebuffer;
	}

	const Framebuffer& PPU::GetWindowMapDebugFramebuffer() const
	{
		return windowDebugFramebuffer;
	}

	void PPU::DebugDrawTileMap(Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, bool useAlternateTileMapAddress, TileMapType tileMapType)
	{
		uint16_t tileIndex = GetTileIndexFromTileMaps(memoryMap, scanlineX / static_cast<float>(TILE_WIDTH_IN_PIXELS), scanlineY / static_cast<float>(TILE_HEIGHT_IN_PIXELS), useAlternateTileMapAddress);
		uint16_t tileAddress = GetTileAddress(tileIndex, scanlineY, lcdc.Read(LCDC_BG_WINDOW_ADDRESSING_MODE_BIT_INDEX));

		// For background and window pixels, the most significant bits/pixels are pushed to the queue first.
		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			Color color;
			uint8_t minX = scx.Read();
			uint8_t maxX = (scx.Read() + GB_SCREEN_WIDTH) & 255;
			uint8_t minY = scy.Read();
			uint8_t maxY = (scy.Read() + GB_SCREEN_HEIGHT) & 255;

			// TODO: Cleanup
			bool isOnXScrollBox = (scanlineX == minX || scanlineX == maxX) && (maxY > minY ? (scanlineY >= minY && scanlineY <= maxY) : (scanlineY >= minY || scanlineY <= maxY));
			bool isOnYScrollBox = (scanlineY == minY || scanlineY == maxY) && (maxX > minX ? (scanlineX >= minX && scanlineX <= maxX) : (scanlineX >= minX || scanlineX <= maxX));

			if (tileMapType == TileMapType::Background && (isOnXScrollBox || isOnYScrollBox))
			{
				color = SCROLL_BOX_DEBUG_COLOR;
			}
			else
			{
				// The bit (with the same index as this pixel) in currentLowTileData specifies the least significant bit of 
				// the pixel's color index, and the bit in currentHighTileData specifies the most significant bit of the color index.
				uint8_t colorIndex = GetColorIndexFromTileData(px, memoryMap.Read(tileAddress), memoryMap.Read(tileAddress + 1));
				color = GetColorFromColorIndex(colorIndex, memoryMap.Read(GB_BACKGROUND_PALETTE_ADDRESS));
			}

			framebuffer.SetPixel(scanlineX, scanlineY, color);

			// HBlank
			if (scanlineX == TILE_MAP_WIDTH_IN_PIXELS - 1)
			{
				scanlineX = 0;

				// VBlank
				if (scanlineY == TILE_MAP_HEIGHT_IN_PIXELS - 1)
				{
					scanlineY = 0;
					framebuffer.UploadData();
					framebuffer.Clear();
				}
				else
				{
					scanlineY++;
				}
			}
			else
			{
				scanlineX++;
			}
		}
	}

	void PPU::DebugDrawBackgroundTileMap()
	{
		DebugDrawTileMap(backgroundDebugFramebuffer, debugBackgroundMapScanlineX, debugBackgroundMapScanlineY, lcdc.Read(LCDC_BG_TILE_MAP_AREA_BIT_INDEX), TileMapType::Background);
	}

	void PPU::DebugDrawWindowTileMap()
	{
		DebugDrawTileMap(windowDebugFramebuffer, debugWindowMapScanlineX, debugWindowMapScanlineY, lcdc.Read(LCDC_WINDOW_TILE_MAP_AREA_BIT_INDEX), TileMapType::Window);
	}

	void PPU::DebugDrawSprites()
	{
		Sprite sprite = GetSpriteAtIndex(debugCurrentSpriteIndex);

		uint8_t palette = memoryMap.Read(sprite.palette == 0 ? GB_SPRITE_PALETTE_0_ADDRESS : GB_SPRITE_PALETTE_1_ADDRESS);
		uint8_t spriteSizeInTiles = lcdc.Read(LCDC_OBJ_SIZE_BIT_INDEX) + 1;
		uint8_t spriteSizeInPixels = spriteSizeInTiles * MIN_SPRITE_HEIGHT_IN_PIXELS;

		uint8_t tileX = debugCurrentSpriteIndex % MAX_SPRITES_PER_SCANLINE;
		uint8_t tileY = std::floor(debugCurrentSpriteIndex / static_cast<float>(MAX_SPRITES_PER_SCANLINE));
		uint8_t xSpacing = TILE_WIDTH_IN_PIXELS + 1;
		uint8_t ySpacing = TILE_HEIGHT_IN_PIXELS * spriteSizeInTiles + 1;

		uint8_t tile = std::floor(debugSpriteScanline / static_cast<float>(TILE_HEIGHT_IN_PIXELS));
		uint8_t tileScanline = (sprite.yFlip ? (spriteSizeInPixels - 1) - debugSpriteScanline : debugSpriteScanline) % TILE_HEIGHT_IN_PIXELS;

		uint8_t lowTileData = memoryMap.Read(GetTileAddress(sprite.tileIndex + tile, tileScanline, true));
		uint8_t highTileData = memoryMap.Read(GetTileAddress(sprite.tileIndex + tile, tileScanline, true) + 1);

		uint8_t y = (tileY * ySpacing) + debugSpriteScanline;

		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			uint8_t x = (tileX * xSpacing) + (sprite.xFlip ? 7 - px : px);

			uint8_t colorIndex = GetColorIndexFromTileData(px, lowTileData, highTileData);
			spriteDebugFramebuffer.SetPixel(x, y, GetColorFromColorIndex(colorIndex, palette));
		}

		debugSpriteScanline++;

		if (debugSpriteScanline >= spriteSizeInPixels)
		{
			debugSpriteScanline = 0;
			debugCurrentSpriteIndex++;
			if (debugCurrentSpriteIndex >= MAX_SPRITE_COUNT)
			{
				debugCurrentSpriteIndex = 0;
				spriteDebugFramebuffer.UploadData();
				spriteDebugFramebuffer.Clear(RGBA_BLACK);
			}
		}
	}

	void PPU::DebugDrawTiles()
	{
		uint8_t palette = memoryMap.Read(GB_BACKGROUND_PALETTE_ADDRESS);
		uint8_t tileX = debugTileIndex % TILE_DEBUG_FRAMEBUFFER_WIDTH_IN_TILES;
		uint8_t tileY = std::floor(debugTileIndex / static_cast<float>(TILE_DEBUG_FRAMEBUFFER_WIDTH_IN_TILES));
		uint8_t spacing = TILE_WIDTH_IN_PIXELS + 1;

		uint8_t lowTileData = memoryMap.Read(GetTileAddress(debugTileIndex, debugTileScanline, true));
		uint8_t highTileData = memoryMap.Read(GetTileAddress(debugTileIndex, debugTileScanline, true) + 1);

		uint8_t y = (tileY * spacing) + debugTileScanline;

		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			uint8_t x = (tileX * spacing) + px;
			uint8_t colorIndex = GetColorIndexFromTileData(px, lowTileData, highTileData);
			tileDebugFramebuffer.SetPixel(x, y, GetColorFromColorIndex(colorIndex, palette));
		}

		debugTileScanline++;

		if (debugTileScanline >= TILE_HEIGHT_IN_PIXELS)
		{
			debugTileScanline = 0;
			debugTileIndex++;
			if (debugTileIndex >= MAX_TILE_COUNT)
			{
				debugTileIndex = 0;
				tileDebugFramebuffer.UploadData();
				tileDebugFramebuffer.Clear(RGBA_BLACK);
			}
		}
	}
}