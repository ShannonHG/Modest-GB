#include <iostream>
#include <cassert>
#include "Graphics/PPU.hpp"
#include "Utils/DataConversions.hpp"
#include "Utils/Arithmetic.hpp"
#include "Logger.hpp"
#include "CPU/Interrupts.hpp"
#include "Utils/GBSpecs.hpp"

namespace SHG
{
	const uint8_t LCD_STATUS_LY_COMPARE_STAT_INTERRUPT_INDEX = 6;
	const uint8_t LCD_STATUS_OAM_STAT_INTERRUPT_INDEX = 5;
	const uint8_t LCD_STATUS_VBLANK_STAT_INTERRUPT_INDEX = 4;
	const uint8_t LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX = 3;
	const uint8_t LCD_STATUS_LY_COMPARE_FLAG_INDEX = 2;
	const uint8_t LCD_STATUS_MODE_FLAG_INDEX_1 = 1;
	const uint8_t LCD_STATUS_MODE_FLAG_INDEX_0 = 0;

	const uint8_t LCDC_LCD_PPU_ENABLE_INDEX = 7;
	const uint8_t LCDC_WINDOW_TILE_MAP_INDEX = 6;
	const uint8_t LCDC_WINDOW_ENABLED_INDEX = 5;
	const uint8_t LCDC_BG_WINDOW_TILE_DATA_INDEX = 4;
	const uint8_t LCDC_BG_TILE_MAP_INDEX = 3;
	const uint8_t LCDC_OBJ_SIZE_INDEX = 2;
	const uint8_t LCDC_OBJ_ENABLE_INDEX = 1;
	const uint8_t LCDC_BG_WINDOW_PRIORITY_INDEX = 0;

	const uint16_t DEFAULT_TILE_MAP_ADDRESS = 0x9800;
	const uint16_t ALTERNATE_TILE_MAP_ADDRESS = 0x9C00;

	const uint16_t SPRITE_TILES_TABLE_START_ADDRESS = 0x8000;
	const uint16_t SPRITE_TILES_TABLE_END_ADDRESS = 0x8FFF;
	const uint16_t SPRITE_ATTRIBUTE_TABLE_START_ADDRESS = 0xFE00;
	const uint16_t SPRITE_ATTRIBUTE_TABLE_END_ADDRESS = 0xFE9F;
	const uint16_t SPRITE_PALETTE_0_ADDRESS = 0xFF48;
	const uint16_t SPRITE_PALETTE_1_ADDRESS = 0xFF49;
	const uint8_t MAX_SPRITE_COUNT = 40;
	const uint8_t MAX_SPRITES_PER_SCANLINE = 10;

	const uint16_t UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS = 0x8000;
	const uint16_t SIGNED_ADDRESSING_MODE_BASE_ADDRESS = 0x9000;

	const uint8_t TILE_SIZE_IN_BYTES = 16;
	const uint8_t TILE_WIDTH_IN_PIXELS = 8;
	const uint8_t TILE_HEIGHT_IN_PIXELS = 8;

	const uint16_t TILE_DATA_START_ADDRESS = 0x8000;
	const uint16_t TILE_DATA_END_ADDRESS = 0x97FF;
	const uint16_t BACKGROUND_PALETTE_ADDRESS = 0xFF47;
	const uint8_t TILE_MAP_WIDTH = 32;
	const uint8_t TILE_MAP_HEIGHT = 32;
	const uint16_t TILE_MAP_PIXEL_WIDTH = 256;
	const uint16_t TILE_MAP_PIXEL_HEIGHT = 256;
	const uint16_t MAX_TILE_COUNT = 384;

	const uint8_t VBLANK_END_Y = 153;

	const uint16_t DMA_TRANSFER_DESTINATION_START_ADDRESS = 0xFE00;
	const uint8_t DMA_TRANSFER_DURATION = 160;

	const uint16_t CYCLES_PER_SCANLINE = 456;
	const uint8_t HBLANK_DURATION = 208;
	const uint16_t MIN_LCD_TRANSFER_DURATION = 172;

	const uint16_t DEBUG_WINDOW_TILE_MAP_FRAMEBUFFER_WIDTH = TILE_MAP_PIXEL_WIDTH;
	const uint16_t DEBUG_WINDOW_TILE_MAP_FRAMEBUFFER_HEIGHT = TILE_MAP_PIXEL_HEIGHT;
	const uint16_t DEBUG_BACKGROUND_TILE_MAP_FRAMEBUFFER_WIDTH = TILE_MAP_PIXEL_WIDTH;
	const uint16_t DEBUG_BACKGROUND_TILE_MAP_FRAMEBUFFER_HEIGHT = TILE_MAP_PIXEL_HEIGHT;
	const uint16_t DEBUG_SPRITE_FRAMEBUFFER_WIDTH = 116;
	const uint16_t DEBUG_SPRITE_FRAMEBUFFER_HEIGHT = 44;
	const uint16_t DEBUG_TILE_FRAMEBUFFER_WIDTH = 256;
	const uint16_t DEBUG_TILE_FRAMEBUFFER_HEIGHT = 96;

	const std::map<PPU::PixelFetcherState, uint32_t> PPU::STATE_DURATIONS =
	{
		{PixelFetcherState::FetchingTile, 2},
		{PixelFetcherState::FetchingLowTileData, 2},
		{PixelFetcherState::FetchingHighTileData, 2},
		{PixelFetcherState::Sleeping, 2},
		// TODO: This might be wrong
		{PixelFetcherState::PushingData, 0},
	};

	PPU::PPU(MemoryMap& memoryManagementUnit) : memoryManagementUnit(memoryManagementUnit)
	{
	}

	void PPU::InitializeFramebuffer(SDL_Window* window)
	{
		mainFramebuffer.Initialize(window, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT);
		debugGenericTileFramebuffer.Initialize(window, DEBUG_TILE_FRAMEBUFFER_WIDTH, DEBUG_TILE_FRAMEBUFFER_HEIGHT);
		debugSpriteFramebuffer.Initialize(window, DEBUG_SPRITE_FRAMEBUFFER_WIDTH, DEBUG_SPRITE_FRAMEBUFFER_HEIGHT);
		debugBackgroundTileMapFramebuffer.Initialize(window, DEBUG_BACKGROUND_TILE_MAP_FRAMEBUFFER_WIDTH, DEBUG_BACKGROUND_TILE_MAP_FRAMEBUFFER_HEIGHT);
		debugWindowTileMapFramebuffer.Initialize(window, DEBUG_WINDOW_TILE_MAP_FRAMEBUFFER_WIDTH, DEBUG_WINDOW_TILE_MAP_FRAMEBUFFER_HEIGHT);
	}

	void PPU::Reset()
	{
		currentMode = PPUMode::SearchingOAM;
		pixelFetcherState = PixelFetcherState::FetchingTile;
		dmaTransferState = DMATransferState::Idle;

		currentModeElapsedTime = 0;

		currentScanlineX = 0;
		currentScanlineElapsedTime = 0;

		currentPixelFetcherTileIndex = 0;
		currentPixelFetcherTileDataLow = 0;
		currentPixelFetcherTileDataHigh = 0;

		spritesOnCurrentScanline.clear();
		currentSpriteIndex = 0;

		tileMapPixelQueue = std::queue<PixelData>();
		spritePixelQueue = std::queue<PixelData>();

		dmaTransferElapsedTime = 0;
		dmaRegister.ClearPendingTransfer();

		mainFramebuffer.Clear();
		debugGenericTileFramebuffer.Clear();
		debugSpriteFramebuffer.Clear(0, 0, 0, 0);
		debugBackgroundTileMapFramebuffer.Clear();
		debugWindowTileMapFramebuffer.Clear();
	}

	void PPU::Step(uint32_t duration)
	{
		if (duration == 0)
			return;

		// TODO: Is this correct?
		if (!lcdControl.GetBit(LCDC_LCD_PPU_ENABLE_INDEX))
		{
			lcdStatus.SetData(0);
			mainFramebuffer.Clear();
			return;
		}

		duration = ExecuteDMATransferCycle(duration);

		switch (currentMode)
		{
		case PPUMode::HBlank:
			duration = HandleHBlankMode(duration);
			break;
		case PPUMode::VBlank:
			duration = HandleVBlankMode(duration);
			break;
		case PPUMode::SearchingOAM:
			duration = FetchSpritesOnScanline(ly.GetData(), spritesOnCurrentScanline, duration);
			break;
		case PPUMode::LCDTransfer:
			duration = HandleLCDTransferMode(duration);
			break;
		}

		RefreshLYCompare();

		Step(duration);
	}

	uint32_t PPU::ExecuteDMATransferCycle(uint32_t duration)
	{
		if (dmaRegister.IsTransferPending())
		{
			dmaRegister.ClearPendingTransfer();
			dmaTransferState = DMATransferState::InProgress;
		}

		if (dmaTransferState == DMATransferState::InProgress)
		{
			for (uint32_t i = 0; i < duration; i++)
			{
				// A DMA transfer causes data to be transfer from $XX00-$XX9F to $FFE0-$FE9F.
				// Since a DMA transfer lasts 160 cycles and also transfers 160 bytes, the elapsed DMA transfer time
				// can be used as the offset from the source and destination addresses.
				uint8_t sourceData = memoryManagementUnit.Read(dmaRegister.GetSourceStartAddress() + dmaTransferElapsedTime);
				memoryManagementUnit.Write(DMA_TRANSFER_DESTINATION_START_ADDRESS + dmaTransferElapsedTime, sourceData);

				dmaTransferElapsedTime++;

				if (dmaTransferElapsedTime == DMA_TRANSFER_DURATION)
				{
					dmaTransferState = DMATransferState::Idle;
					dmaTransferElapsedTime = 0;
				}
			}

			return 0;
		}

		return duration;
	}

	uint32_t PPU::HandleHBlankMode(uint32_t duration)
	{
		currentScanlineElapsedTime += duration;
		currentModeElapsedTime += duration;

		if (currentScanlineElapsedTime >= CYCLES_PER_SCANLINE)
		{
			// Save the leftover cycles
			duration = currentScanlineElapsedTime - CYCLES_PER_SCANLINE;

			ly.Increment();
			currentScanlineX = 0;
			currentScanlineElapsedTime = 0;

			currentModeElapsedTime = 0;

			if (ly.GetData() == GB_SCREEN_HEIGHT)
				EnterVBlankMode();
			else
				EnterOAMSearchMode();

			return duration;
		}

		return 0;
	}

	uint32_t PPU::HandleVBlankMode(uint32_t duration)
	{
		currentScanlineElapsedTime += duration;
		currentModeElapsedTime += duration;

		if (currentScanlineElapsedTime >= CYCLES_PER_SCANLINE)
		{
			// Save the leftover cycles
			duration = currentScanlineElapsedTime - CYCLES_PER_SCANLINE;
			currentScanlineElapsedTime = 0;

			if (ly.GetData() == VBLANK_END_Y)
			{
				currentModeElapsedTime = 0;

				ly.SetData(0);

				mainFramebuffer.UploadData();
				EnterOAMSearchMode();

				return duration;
			}

			ly.Increment();
			return duration;
		}

		return 0;
	}

	uint32_t PPU::FetchSpritesOnScanline(uint8_t scanlineY, std::vector<Sprite>& sprites, uint32_t duration)
	{
		while (currentSpriteIndex < MAX_SPRITE_COUNT)
		{
			currentModeElapsedTime += 2;

			Sprite sprite = FetchSpriteAtIndex(currentSpriteIndex);

			int16_t spriteScanline = 16 - (sprite.y - scanlineY);

			// TODO: This won't work for 8x16 sprites
			if (spriteScanline >= 0 && spriteScanline < 8)
				sprites.push_back(sprite);

			currentScanlineElapsedTime += 2;
			currentSpriteIndex++;

			if ((int)(duration - 2) <= 0)
			{
				if (currentSpriteIndex == MAX_SPRITE_COUNT)
				{
					duration = 0;
					break;
				}
				else
				{
					return 0;
				}
			}

			duration -= 2;
		}

		currentModeElapsedTime = 0;
		currentSpriteIndex = 0;
		EnterLCDTransferMode();

		return duration;
	}

	uint32_t PPU::HandleLCDTransferMode(uint32_t duration)
	{
		currentScanlineElapsedTime += STATE_DURATIONS.at(pixelFetcherState);
		currentModeElapsedTime += STATE_DURATIONS.at(pixelFetcherState);

		switch (pixelFetcherState)
		{
		case PixelFetcherState::FetchingTile:
			currentPixelFetcherTileIndex = FetchTileMapIndex(currentScanlineX, ly.GetData(), GB_SCREEN_WIDTH, TileMapType::BackgroundAndWindow);
			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingLowTileData, duration);
			break;
		case PixelFetcherState::FetchingLowTileData:
			currentPixelFetcherTileDataLow = memoryManagementUnit.Read(GetTileAddressFromTileMaps(currentPixelFetcherTileIndex, currentScanlineX, ly.GetData()) + 1);
			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingHighTileData, duration);
			break;
		case PixelFetcherState::FetchingHighTileData:
			currentPixelFetcherTileDataHigh = memoryManagementUnit.Read(GetTileAddressFromTileMaps(currentPixelFetcherTileIndex, currentScanlineX, ly.GetData()));
			duration = TransitionToPixelFetcherState(PixelFetcherState::Sleeping, duration);
			break;
		case PixelFetcherState::Sleeping:
			duration = TransitionToPixelFetcherState(PixelFetcherState::PushingData, duration);
			break;
		case PixelFetcherState::PushingData:
			uint8_t queueSize = tileMapPixelQueue.size();
			GetPixelsFromTileScanline(currentPixelFetcherTileDataLow, currentPixelFetcherTileDataHigh, currentScanlineX, ly.GetData(), GB_SCREEN_WIDTH, tileMapPixelQueue);

			// Increase the X position based on how many pixels were added to the queue.
			// The number of added pixels should generally be 8, but this handles the case where that's not true.
			uint8_t diff = tileMapPixelQueue.size() - queueSize;
			currentScanlineX += diff;

			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingTile, duration);

			if (currentScanlineX == GB_SCREEN_WIDTH)
			{
				currentScanlineElapsedTime += 2;
				currentModeElapsedTime += 2;

				duration = std::max((int)(duration - 2), 0);

				// TODO: Account for number of sprites
				if (currentModeElapsedTime >= MIN_LCD_TRANSFER_DURATION)
				{
					if (lcdControl.GetBit(LCDC_OBJ_ENABLE_INDEX))
					{
						GetSpritePixelsForScanline(spritesOnCurrentScanline, currentScanlineX, ly.GetData(), spritePixelQueue);
						spritesOnCurrentScanline.clear();
					}

					currentModeElapsedTime = 0;
					EnterHBlankMode();
				}
			}

			break;
		}

		return duration;
	}

	void PPU::EnterVBlankMode()
	{
		currentMode = PPUMode::VBlank;

		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_0, true);
		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_1, false);

		RequestInterrupt(memoryManagementUnit, InterruptType::VBlank);
		ChangeStatInterruptSourceBit(LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX, true);
	}

	void PPU::EnterHBlankMode()
	{
		currentMode = PPUMode::HBlank;

		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_0, false);
		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_1, false);

		ChangeStatInterruptSourceBit(LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX, true);

		while (tileMapPixelQueue.size() > 0)
		{
			PixelData data = tileMapPixelQueue.front();
			tileMapPixelQueue.pop();

			if (data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			mainFramebuffer.SetPixel(data.x, data.y, color, color, color, 255);
		}

		while (spritePixelQueue.size() > 0)
		{
			PixelData data = spritePixelQueue.front();
			spritePixelQueue.pop();

			if (data.colorID == PixelColorID::White || data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			// TODO: Revisit
			if (data.backgroundPriority == 0 || mainFramebuffer.GetPixel(data.x, data.y) == std::numeric_limits<uint32_t>::max())
			{
				mainFramebuffer.SetPixel(data.x, data.y, color, color, color, 255);
			}
		}
	}

	void PPU::EnterLCDTransferMode()
	{
		currentMode = PPUMode::LCDTransfer;

		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_0, true);
		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_1, true);

		ChangeStatInterruptSourceBit(LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX, true);
	}

	uint16_t PPU::FetchTileMapIndex(uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, TileMapType tileMapType, bool ignoreScrolling)
	{
		uint16_t tileMapAddress = DEFAULT_TILE_MAP_ADDRESS;

		bool isBackgroundTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::Background) != 0;
		bool isWindowTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::Window) != 0;

		uint8_t windowX = wx.GetData();
		int8_t trueWindowX = windowX - 7;
		uint8_t windowY = wy.GetData();

		uint8_t scrollX = scx.GetData();
		uint8_t scrollY = scy.GetData();

		bool isWindowEnabled = lcdControl.GetBit(LCDC_WINDOW_ENABLED_INDEX);
		bool isScanlineXInsideWindow = scanlineX >= windowX && scanlineY < windowX + TILE_MAP_PIXEL_WIDTH;
		
		bool isBackgroundTile = false;
		if (lcdControl.GetBit(LCDC_BG_TILE_MAP_INDEX))
		{
			// If the current X is outside of the window then the current tile is a background tile
			// and the tile map at 0x9C00 should be used to fetch the tile.
			if (ignoreScrolling || !(isWindowEnabled && isScanlineXInsideWindow))
			{
				isBackgroundTile = true;
				tileMapAddress = ALTERNATE_TILE_MAP_ADDRESS;
			}
		}
		else
		{
			isBackgroundTile = true;
		}

		bool isWindowTile = false;
		if (isWindowEnabled)
		{
			if (lcdControl.GetBit(LCDC_WINDOW_TILE_MAP_INDEX))
			{
				// If the current X is inside of the window then the current tile is a window tile
				// and the tile map at 0x9C00 should be used to fetch the tile.
				if (ignoreScrolling || isScanlineXInsideWindow)
				{
					isWindowTile = true;
					tileMapAddress = ALTERNATE_TILE_MAP_ADDRESS;
				}
			}
			else
			{
				isWindowTile = true;
			}
		}

		uint8_t tileX = 0;
		uint8_t tileY = 0;

		if (isWindowTile && isWindowTilesEnabled)
		{
			// TODO: Revisit
			tileX = trueWindowX + scanlineX;
			tileY = windowX + scanlineY;
			tileX = static_cast<uint8_t>(std::floor((trueWindowX + scanlineX) / 8.0));
			tileY = static_cast<uint8_t>(std::floor(windowX + scanlineY) / 8.0);
		}

		if (isBackgroundTile && isBackgroundTilesEnabled)
		{
			// IgnoreScrolling is useful for drawing the entire background, or window, tile map for debugging purposes.
			if (ignoreScrolling)
			{
				tileX = std::floor(scanlineX / (float)TILE_WIDTH_IN_PIXELS);
				tileY = std::floor(scanlineY / (float)TILE_HEIGHT_IN_PIXELS);
			}
			else
			{
				tileX = (uint8_t)(std::floor(scrollX / (float)TILE_WIDTH_IN_PIXELS) + std::floor(scanlineX / (float)TILE_WIDTH_IN_PIXELS)) & 0x1F;
				tileY = std::floor(((scanlineY + scrollY) & 255) / (float)TILE_HEIGHT_IN_PIXELS);
			}
		}

		int tileMapIndex = tileX + tileY * TILE_MAP_WIDTH;
		return memoryManagementUnit.Read(tileMapAddress + tileMapIndex);
	}

	void PPU::GetPixelsFromTileScanline(uint8_t rawScanlineDataLow, uint8_t rawScanlineDataHigh, uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, std::queue<PixelData>& pixelQueue)
	{
		uint16_t endX = std::clamp(scanlineX + 8, 0, (int)tileMapRegionWidth);

		for (uint16_t x = scanlineX; x < endX; x++)
		{
			// This ensures that pixel positions reflect the position of their respective bits.
			// For example, the pixel belonging to the most significant bit should be rendered
			// in the left-most position of the tile.
			int tilePixelX = 7 - (x % 8);

			uint16_t modifier = (1 << tilePixelX);

			// The bit at the specified index in the low byte
			// will determine the most significant bit of the color, while the bit 
			// in the high byte will determine the least significant bit of the color.
			uint8_t low = (rawScanlineDataLow & modifier) >> tilePixelX;
			uint8_t high = (rawScanlineDataHigh & modifier) >> tilePixelX;

			PixelData pixelData;

			pixelData.x = x;
			pixelData.y = scanlineY;
			pixelData.colorID = static_cast<PixelColorID>((low << 1) | high);

			pixelQueue.push(pixelData);
		}
	}

	int32_t PPU::GetTileAddressFromTileMaps(uint16_t tileIndex, uint8_t scanlineX, uint8_t scanlineY)
	{
		bool isUnsignedAddressingMode = lcdControl.GetBit(LCDC_BG_WINDOW_TILE_DATA_INDEX);

		tileIndex = isUnsignedAddressingMode ? tileIndex : (int8_t)tileIndex;
		int tileDataOffset = (tileIndex * TILE_SIZE_IN_BYTES) + ((scanlineY % TILE_WIDTH_IN_PIXELS) * 2);

		return tileDataOffset + (isUnsignedAddressingMode ? UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS : SIGNED_ADDRESSING_MODE_BASE_ADDRESS);
	}

	uint8_t PPU::GetColorFromID(PixelColorID id)
	{
		switch (id)
		{
		case PixelColorID::Black:
			return 0;
		case PixelColorID::DarkGray:
			return 100;
		case PixelColorID::LightGray:
			return 175;
		case PixelColorID::White:
			return 255;
		default:
			assert(false);
		}
	}

	uint32_t PPU::TransitionToPixelFetcherState(PixelFetcherState targetState, uint32_t duration)
	{
		int32_t result = duration - STATE_DURATIONS.at(pixelFetcherState);
		duration = result <= 0 ? 0 : result;
		pixelFetcherState = targetState;

		return duration;
	}

	void PPU::EnterOAMSearchMode()
	{
		currentMode = PPUMode::SearchingOAM;

		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_0, false);
		lcdStatus.ChangeBit(LCD_STATUS_MODE_FLAG_INDEX_1, true);

		ChangeStatInterruptSourceBit(LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX, true);
	}

	void PPU::GetSpritePixelsForScanline(const std::vector<Sprite>& spritesOnCurrentScanline, uint8_t scanlineX, uint8_t scanlineY, std::queue<PixelData>& pixelQueue)
	{
		for (const Sprite& sprite : spritesOnCurrentScanline)
		{
			int16_t spriteScanline = 16 - (sprite.y - scanlineY);

			uint16_t scanlineAddress = sprite.tileAddress + (spriteScanline * 2);

			for (uint8_t x = 0; x < 8; x++)
			{
				// TODO: Implement Y flip
				uint8_t px = sprite.xFlip ? x : 7 - x;

				int tilePixelX = px % 8;

				uint16_t modifier = (1 << tilePixelX);

				// The bit at the specified index in the low byte
				// will determine the most significant bit of the color, while the bit 
				// in the high byte will determine the least significant bit of the color.
				uint8_t low = (memoryManagementUnit.Read(scanlineAddress + 1) & modifier) >> tilePixelX;
				uint8_t high = (memoryManagementUnit.Read(scanlineAddress) & modifier) >> tilePixelX;

				PixelData pixelData;

				pixelData.backgroundPriority = sprite.priority;
				pixelData.x = std::max((sprite.x - 8) + x, 0);
				pixelData.y = scanlineY;
				pixelData.colorID = static_cast<PixelColorID>((low << 1) | high);

				pixelQueue.push(pixelData);
			}

			/*uint16_t duration = (11 - std::min(5, (s.x + scx.GetData()) % 8));
			currentModeElapsedTime += duration;
			currentScanlineElapsedTime += duration;*/
		}
	}

	PPU::Sprite PPU::FetchSpriteAtIndex(uint8_t index)
	{
		uint16_t attributeAddress = SPRITE_ATTRIBUTE_TABLE_START_ADDRESS + (index * 4);

		Sprite sprite;

		sprite.y = memoryManagementUnit.Read(attributeAddress);
		sprite.x = memoryManagementUnit.Read(attributeAddress + 1);
		uint8_t tileIndex = memoryManagementUnit.Read(attributeAddress + 2);
		uint8_t flags = memoryManagementUnit.Read(attributeAddress + 3);

		sprite.xFlip = Arithmetic::GetBit(flags, 5);
		sprite.yFlip = Arithmetic::GetBit(flags, 6);
		sprite.priority = Arithmetic::GetBit(flags, 7);
		sprite.tileAddress = SPRITE_TILES_TABLE_START_ADDRESS + (tileIndex * TILE_SIZE_IN_BYTES);

		return sprite;
	}

	void PPU::RefreshLYCompare()
	{
		bool lyCompare = ly.GetData() == lyc.GetData();
		lcdStatus.ChangeBit(LCD_STATUS_LY_COMPARE_FLAG_INDEX, lyCompare);

		if (lyCompare)
		{
			lcdStatus.ChangeBit(LCD_STATUS_LY_COMPARE_STAT_INTERRUPT_INDEX, true);
			RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
		}
	}

	void PPU::DebugDrawBackgroundTileMap()
	{
		DebugDrawTileMap(debugBackgroundTileMapFramebuffer, debugBackgroundScanlineX, debugBackgroundScanlineY, TileMapType::Background);
	}

	void PPU::DebugDrawWindowTileMap()
	{
		DebugDrawTileMap(debugWindowTileMapFramebuffer, debugWindowScanlineX, debugWindowScanlineY, TileMapType::Window);
	}

	void PPU::DebugDrawTileMap(Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanline, TileMapType tileMapType)
	{
		uint16_t tileIndex = FetchTileMapIndex(scanlineX, scanline, TILE_MAP_PIXEL_WIDTH, tileMapType, true);

		uint8_t low = memoryManagementUnit.Read(GetTileAddressFromTileMaps(tileIndex, scanlineX, scanline) + 1);
		uint8_t high = memoryManagementUnit.Read(GetTileAddressFromTileMaps(tileIndex, scanlineX, scanline));

		auto pixelQueue = std::queue<PixelData>();
		GetPixelsFromTileScanline(low, high, scanlineX, scanline, TILE_MAP_PIXEL_WIDTH, pixelQueue);

		uint16_t x = scanlineX;
		uint16_t y = scanline;

		x += pixelQueue.size();

		while (pixelQueue.size() > 0)
		{
			PixelData data = pixelQueue.front();
			pixelQueue.pop();

			uint8_t color = GetColorFromID(data.colorID);

			framebuffer.SetPixel(data.x, data.y, color, color, color, 255);
		}

		if (x == TILE_MAP_PIXEL_WIDTH)
		{
			x = 0;
			y++;
		}

		if (y == TILE_MAP_PIXEL_HEIGHT)
		{
			framebuffer.UploadData();
			scanline = 0;
		}

		scanlineX = x;
		scanline = y;
	}

	void PPU::DebugDrawSprites()
	{
		Sprite sprite = FetchSpriteAtIndex(debugSpriteIndex);

		uint8_t spriteSpacing = 12;
		uint16_t scanlineAddress = sprite.tileAddress + (debugSpriteScanline * 2);

		uint8_t tileDataHigh = memoryManagementUnit.Read(scanlineAddress);
		uint8_t tileDataLow = memoryManagementUnit.Read(scanlineAddress + 1);

		for (uint8_t scanlineX = 0; scanlineX < 8; scanlineX++)
		{
			uint16_t modifier = (1 << scanlineX);

			// The bit at the specified index in the low byte
			// will determine the most significant bit of the color, while the bit 
			// in the high byte will determine the least significant bit of the color.
			uint8_t low = (tileDataLow & modifier) >> scanlineX;
			uint8_t high = (tileDataHigh & modifier) >> scanlineX;

			uint8_t color = GetColorFromID(static_cast<PixelColorID>((low << 1) | high));

			// Evenly space out sprites across the screen
			uint8_t xOffset = (debugSpriteIndex % MAX_SPRITES_PER_SCANLINE) * spriteSpacing;
			uint8_t yOffset = std::floor(debugSpriteIndex / (double)MAX_SPRITES_PER_SCANLINE) * spriteSpacing;

			debugSpriteFramebuffer.SetPixel(scanlineX + xOffset, debugSpriteScanline + yOffset, color, color, color, 255);
		}

		debugSpriteScanline++;

		if (debugSpriteScanline >= TILE_HEIGHT_IN_PIXELS)
		{
			debugSpriteIndex++;
			debugSpriteScanline = 0;
		}

		if (debugSpriteIndex == MAX_SPRITE_COUNT)
		{
			debugSpriteFramebuffer.UploadData();
			debugSpriteIndex = 0;
		}
	}

	Framebuffer& PPU::GetTileDebugFramebuffer()
	{
		return debugGenericTileFramebuffer;
	}

	Framebuffer& PPU::GetSpriteDebugFramebuffer()
	{
		return debugSpriteFramebuffer;
	}

	Framebuffer& PPU::GetBackgroundMapDebugFramebuffer()
	{
		return debugBackgroundTileMapFramebuffer;
	}

	Framebuffer& PPU::GetWindowMapDebugFramebuffer()
	{
		return debugWindowTileMapFramebuffer;
	}

	void PPU::DebugDrawTiles()
	{
		uint16_t tileDataAddress = TILE_DATA_START_ADDRESS;
		uint16_t maxTiles = MAX_TILE_COUNT;
		uint8_t widthInTiles = 32;
		uint8_t heightInTiles = 12;
		uint16_t widthInPixels = widthInTiles * TILE_WIDTH_IN_PIXELS;
		uint16_t heightInPixels = heightInTiles * TILE_HEIGHT_IN_PIXELS;

		uint16_t tileIndex = std::floor(debugTilesScanlineX / TILE_WIDTH_IN_PIXELS) + (std::floor(debugTilesScanlineY / TILE_HEIGHT_IN_PIXELS) * widthInTiles);

		uint16_t scanlineAddress = tileDataAddress + (tileIndex * TILE_SIZE_IN_BYTES) + ((debugTilesScanlineY % TILE_HEIGHT_IN_PIXELS) * 2);

		uint8_t tileDataLow = memoryManagementUnit.Read(scanlineAddress + 1);
		uint8_t tileDataHigh = memoryManagementUnit.Read(scanlineAddress);

		for (uint8_t px = 0; px < 8; px++)
		{
			uint8_t flippedX = 7 - px;
			uint16_t modifier = (1 << flippedX);

			// The bit at the specified index in the low byte
			// will determine the most significant bit of the color, while the bit 
			// in the high byte will determine the least significant bit of the color.
			uint8_t low = (tileDataLow & modifier) >> flippedX;
			uint8_t high = (tileDataHigh & modifier) >> flippedX;

			uint8_t color = GetColorFromID(static_cast<PixelColorID>((low << 1) | high));

			debugGenericTileFramebuffer.SetPixel(debugTilesScanlineX + px, debugTilesScanlineY, color, color, color, 255);
		}

		debugTilesScanlineX += TILE_WIDTH_IN_PIXELS;

		if (debugTilesScanlineX >= widthInPixels)
		{
			debugTilesScanlineY++;
			debugTilesScanlineX = 0;
		}

		if (debugTilesScanlineY >= heightInPixels)
		{
			debugGenericTileFramebuffer.UploadData();
			debugTilesScanlineY = 0;
		}
	}

	Register8* PPU::GetLCDC()
	{
		return &lcdControl;
	}

	Register8* PPU::GetLCDStatus()
	{
		return &lcdStatus;
	}

	Register8* PPU::GetSCY()
	{
		return &scy;
	}

	Register8* PPU::GetSCX()
	{
		return &scx;
	}

	Register8* PPU::GetLY()
	{
		return &ly;
	}
	
	Register8* PPU::GetLYC()
	{
		return &lyc;
	}

	Register8* PPU::GetWY()
	{
		return &wy;
	}

	Register8* PPU::GetWX()
	{
		return &wx;
	}

	DMATransferRegister* PPU::GetDMATransferRegister()
	{
		return &dmaRegister;
	}

	void PPU::ChangeStatInterruptSourceBit(uint8_t bitIndex, bool isSet)
	{
		// The STAT interrupt will only trigger when the respective bit is change from 0 to 1.
		// So if the bit was already 1, then no interrupt should be requested.
		if (!lcdStatus.GetBit(bitIndex) && isSet)
			RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);

		lcdStatus.ChangeBit(bitIndex, isSet);
	}

	Framebuffer& PPU::GetFramebuffer()
	{
		return mainFramebuffer;
	}
}