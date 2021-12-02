#include <string>
#include <cassert>
#include "Graphics/PPU.hpp"
#include "Utils/Interrupts.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"
#include "Utils/GBMemoryMapAddresses.hpp"
#include "Utils/Arithmetic.hpp"

namespace SHG
{
	const uint8_t SPRITE_DEBUG_FRAMEBUFFER_WIDTH = 89;
	const uint8_t SPRITE_DEBUG_FRAMEBUFFER_HEIGHT = 71;

	PPU::PPU(MemoryMap& memoryMap) : memoryMap(memoryMap),
		backgroundPixelFetcher(&memoryMap, &lcdc, &scx, &scy, &wx, &wy),
		spritePixelFetcher(&memoryMap, &lcdc, &backgroundPixelFetcher)
	{

	}

	void PPU::InitializeFramebuffer(SDL_Window* window)
	{
		primaryFramebuffer = Framebuffer(window, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
		backgroundDebugFramebuffer = Framebuffer(window, TILE_MAP_WIDTH_IN_PIXELS, TILE_MAP_HEIGHT_IN_PIXELS);
		windowDebugFramebuffer = Framebuffer(window, TILE_MAP_WIDTH_IN_PIXELS, TILE_MAP_HEIGHT_IN_PIXELS);
		spriteDebugFramebuffer = Framebuffer(window, SPRITE_DEBUG_FRAMEBUFFER_WIDTH, SPRITE_DEBUG_FRAMEBUFFER_HEIGHT);
	}

	void PPU::Reset()
	{
		currentMode = Mode::SearchingOAM;
		currentScanlineX = 0;
		currentScanlineElapsedCycles = 0;

		spritesOnCurrentScanline.clear();

		dmaRegister.ClearPendingTransfer();

		primaryFramebuffer.Clear();
		primaryFramebuffer.UploadData();

		backgroundDebugFramebuffer.Clear();
		backgroundDebugFramebuffer.UploadData();

		spriteDebugFramebuffer.Clear(RGBA_BLACK);
		spriteDebugFramebuffer.UploadData();

		// TODO: Reset other components
	}

	void PPU::Step(uint32_t cycles)
	{
		while (cycles > 0)
		{
			// TODO: Confirm logic
			if (!lcdc.GetBit(LCDC_PPU_ENABLE_BIT_INDEX))
			{
				SetCurrentMode(Mode::HBlank);
				stat.ChangeBit(STAT_LYC_FLAG_INDEX, 0);
				ly.SetData(0);

				currentDMATransferState = DMATransferState::Idle;

				currentScanlineX = 0;
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
			currentScanlineX = 0;
			ChangeStatInterruptLineBit(STAT_HBLANK_INTERRUPT_SOURCE_BIT_INDEX, false);

			if (ly.GetData() == GB_SCREEN_HEIGHT)
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
			if (ly.GetData() == VBLANK_END_LINE)
			{
				// TODO: Is this correct?
				backgroundPixelFetcher.Reset();
				spritePixelFetcher.Reset();
				ly.SetData(0);
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
			wasWYConditionTriggered = wy.GetData() == ly.GetData();

		numberOfPixelsToIgnore = scx.GetData() & 0b111;
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
				uint8_t spriteSize = lcdc.GetBit(LCDC_OBJ_SIZE_BIT_INDEX) ? MAX_SPRITE_HEIGHT_IN_PIXELS : MIN_SPRITE_HEIGHT_IN_PIXELS;

				// If the sprite is on the current scanline, then added it to the list.
				if (ly.GetData() >= sprite.y && ly.GetData() < sprite.y + spriteSize)
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
		uint16_t attributeAddress = SPRITE_ATTRIBUTE_TABLE_START_ADDRESS + (index * SPRITE_ATTRIBUTE_BYTE_COUNT);
		uint8_t flags = memoryMap.Read(attributeAddress + 3);

		Sprite sprite =
		{
			// OAM contains the sprite's X position plus 8, so to get the true X position we need to subtract 8.
			.x = static_cast<int16_t>(memoryMap.Read(attributeAddress + 1) - TILE_WIDTH_IN_PIXELS),

			// OAM contains the sprite's 16 position plus 16, so to get the true Y position we need to subtract 16.
			.y = static_cast<int16_t>(memoryMap.Read(attributeAddress) - MAX_SPRITE_HEIGHT_IN_PIXELS),

			// In 8x16 mode, the least significant bit of the tile index should be ignored to ensure that it points to the first/top tile of the sprite.
			.tileIndex = static_cast<uint8_t>(lcdc.GetBit(LCDC_OBJ_SIZE_BIT_INDEX) ? memoryMap.Read(attributeAddress + 2) & 0xFE : memoryMap.Read(attributeAddress + 2)),

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
		backgroundPixelFetcher.SetY(ly.GetData());
		backgroundPixelFetcher.SetMode(BackgroundPixelFetcherMode::Background);

		spritePixelFetcher.Reset();
		spritePixelFetcher.SetSpritesOnScanline(spritesOnCurrentScanline);
		spritePixelFetcher.SetY(ly.GetData());

		SetCurrentMode(Mode::LCDTransfer);
	}

	void PPU::UpdateLCDTransferMode(uint32_t* cycles)
	{
		uint32_t targetCycles = *cycles;
		for (uint32_t cycle = 0; cycle < targetCycles; cycle++)
		{
			(*cycles)--;

			wasWXConditionTriggered = (currentScanlineX + 7) == wx.GetData();

			// If all of the conditions to render the window have been satisfied, then reset the background
			// pixel fetcher, and set it to 'Window' mode.
			if (backgroundPixelFetcher.GetCurrentMode() != BackgroundPixelFetcherMode::Window &&
				wasWXConditionTriggered &&
				wasWYConditionTriggered &&
				lcdc.GetBit(LCDC_WINDOW_ENABLE_BIT_INDEX))
			{
				backgroundPixelFetcher.Reset();
				backgroundPixelFetcher.SetY(windowLineCounter);
				backgroundPixelFetcher.SetMode(BackgroundPixelFetcherMode::Window);
				windowLineCounter++;
			}

			if (lcdc.GetBit(LCDC_OBJ_ENABLE_BIT_INDEX))
			{
				spritePixelFetcher.SetX(currentScanlineX);
				spritePixelFetcher.Step();
			}

			if (spritePixelFetcher.GetState() != SpritePixelFetcherState::Idle)
				return;

			Pixel selectedPixel;
			bool isSpritePixelAvailable = spritePixelFetcher.GetPixelQueueSize() != 0 && lcdc.GetBit(LCDC_OBJ_ENABLE_BIT_INDEX);

			if (lcdc.GetBit(LCDC_BG_WINDOW_ENABLE_BIT_INDEX))
			{
				backgroundPixelFetcher.Step();

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
							selectedPixel = spritePixel;
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
		primaryFramebuffer.SetPixel(currentScanlineX, ly.GetData(), GetColorFromColorIndex(pixel.colorIndex, memoryMap.Read(pixel.paletteAddress)));
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
		if (!stat.GetBit(bitIndex))
			return;

		// The STAT interrupt sources (LYC=LY, OAM Search, HBlank, VBlank), have their
		// states logically OR'ed into the STAT interrupt line. If a source sets its bit to 1,
		// and the STAT interrupt line is currently 0, then a STAT interrupt will be triggered.
		if (!statInterruptLine.GetData() && value)
			RequestInterrupt(memoryMap, InterruptType::LCDStat);

		statInterruptLine.ChangeBit(bitIndex, value);
	}

	void PPU::RefreshLYCFlag()
	{
		bool result = lyc.GetData() == ly.GetData();
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

	Framebuffer& PPU::GetPrimaryFramebuffer()
	{
		return primaryFramebuffer;
	}

	Register8& PPU::GetLCDC()
	{
		return lcdc;
	}

	Register8& PPU::GetLCDStatusRegister()
	{
		return stat;
	}

	Register8& PPU::GetSCY()
	{
		return scy;
	}

	Register8& PPU::GetSCX()
	{
		return scx;
	}

	Register8& PPU::GetLY()
	{
		return ly;
	}

	Register8& PPU::GetLYC()
	{
		return lyc;
	}

	Register8& PPU::GetWY()
	{
		return wy;
	}

	Register8& PPU::GetWX()
	{
		return wx;
	}

	DMATransferRegister& PPU::GetDMATransferRegister()
	{
		return dmaRegister;
	}

	Framebuffer& PPU::GetTileDebugFramebuffer()
	{
		return tileDebugFramebuffer;
	}

	Framebuffer& PPU::GetSpriteDebugFramebuffer()
	{
		return spriteDebugFramebuffer;
	}

	Framebuffer& PPU::GetBackgroundMapDebugFramebuffer()
	{
		return backgroundDebugFramebuffer;
	}

	Framebuffer& PPU::GetWindowMapDebugFramebuffer()
	{
		return windowDebugFramebuffer;
	}

	void PPU::DebugDrawTileMap(Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, bool useAlternateTileMapAddress)
	{
		uint16_t tileIndex = GetTileIndexFromTileMaps(memoryMap, scanlineX / static_cast<float>(TILE_WIDTH_IN_PIXELS), scanlineY / static_cast<float>(TILE_HEIGHT_IN_PIXELS), useAlternateTileMapAddress);
		uint16_t tileAddress = GetTileAddress(tileIndex, scanlineY, lcdc.GetBit(LCDC_BG_WINDOW_ADDRESSING_MODE_BIT_INDEX));

		// For background and window pixels, the most significant bits/pixels are pushed to the queue first.
		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			// The bit (with the same index as this pixel) in currentLowTileData specifies the least significant bit of 
			// the pixel's color index, and the bit in currentHighTileData specifies the most significant bit of the color index.
			uint8_t colorIndex = GetColorIndexFromTileData(px, memoryMap.Read(tileAddress), memoryMap.Read(tileAddress + 1));

			framebuffer.SetPixel(scanlineX, scanlineY, GetColorFromColorIndex(colorIndex, memoryMap.Read(GB_BACKGROUND_PALETTE_ADDRESS)));

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
		DebugDrawTileMap(backgroundDebugFramebuffer, debugBackgroundMapScanlineX, debugBackgroundMapScanlineY, lcdc.GetBit(LCDC_BG_TILE_MAP_AREA_BIT_INDEX));
	}

	void PPU::DebugDrawWindowTileMap()
	{
		DebugDrawTileMap(windowDebugFramebuffer, debugWindowMapScanlineX, debugWindowMapScanlineY, lcdc.GetBit(LCDC_WINDOW_TILE_MAP_AREA_BIT_INDEX));
	}

	void PPU::DebugDrawSprites()
	{
		Sprite sprite = GetSpriteAtIndex(debugCurrentSpriteIndex);

		uint16_t palette = memoryMap.Read(sprite.palette == 0 ? GB_SPRITE_PALETTE_0_ADDRESS : GB_SPRITE_PALETTE_1_ADDRESS);
		uint8_t spriteSizeInTiles = lcdc.GetBit(LCDC_OBJ_SIZE_BIT_INDEX) + 1;
		uint8_t spriteSizeInPixels = spriteSizeInTiles * MIN_SPRITE_HEIGHT_IN_PIXELS;

		uint8_t tileX = debugCurrentSpriteIndex % MAX_SPRITES_PER_SCANLINE;
		uint8_t tileY = std::floor(debugCurrentSpriteIndex / static_cast<float>(MAX_SPRITES_PER_SCANLINE));
		uint8_t xSpacing = TILE_WIDTH_IN_PIXELS + 1;
		uint8_t ySpacing = TILE_HEIGHT_IN_PIXELS * spriteSizeInTiles + 1;

		// Loops through each tile in the sprite.
		for (uint8_t tile = 0; tile < spriteSizeInTiles; tile++)
		{
			// Goes through each scanline in the tile.
			for (uint8_t scanline = 0; scanline < TILE_HEIGHT_IN_PIXELS; scanline++)
			{
				uint8_t tileScanline = (sprite.yFlip ? (spriteSizeInPixels - 1) - scanline : scanline) + (tile * TILE_HEIGHT_IN_PIXELS);

				uint8_t lowTileData = memoryMap.Read(GetTileAddress(sprite.tileIndex + tile, scanline, true));
				uint8_t highTileData = memoryMap.Read(GetTileAddress(sprite.tileIndex + tile, scanline, true) + 1);

				for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
				{
					uint8_t x = tileX * xSpacing + (sprite.xFlip ? 7 - px : px);
					uint8_t y = tileY * ySpacing + tileScanline;

					uint8_t colorIndex = GetColorIndexFromTileData(px, lowTileData, highTileData);
					spriteDebugFramebuffer.SetPixel(x, y, GetColorFromColorIndex(colorIndex, palette));
				}
			}
		}

		debugCurrentSpriteIndex++;
		if (debugCurrentSpriteIndex >= MAX_SPRITE_COUNT)
		{
			debugCurrentSpriteIndex = 0;
			spriteDebugFramebuffer.UploadData();
			spriteDebugFramebuffer.Clear(RGBA_BLACK);
		}
	}

	void PPU::DebugDrawTiles()
	{
		// TODO: Implement
	}
}