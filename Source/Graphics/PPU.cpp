#include <iostream>
#include <cassert>
#include "Graphics/PPU.hpp"
#include "Globals.hpp"
#include "Common/Arithmetic.hpp"
#include "SDL.h"
#include "Logger.hpp"
#include "CPU/Interrupts.hpp"

namespace SHG
{
	const uint16_t LCD_CONTROL_REGISTER_ADDRESS = 0xFF40;
	const uint16_t LCD_STATUS_REGISTER_ADDRESS = 0xFF41;

	const uint8_t LCD_STATUS_LY_COMPARE_STAT_INTERRUPT_INDEX = 6;
	const uint8_t LCD_STATUS_OAM_STAT_INTERRUPT_INDEX = 5;
	const uint8_t LCD_STATUS_VBLANK_STAT_INTERRUPT_INDEX = 4;
	const uint8_t LCD_STATUS_HBLANK_STAT_INTERRUPT_INDEX = 3;
	const uint8_t LCD_STATUS_LY_COMPARE_FLAG_INDEX = 2;
	const uint8_t LCD_STATUS_MODE_FLAG_INDEX_1 = 1;
	const uint8_t LCD_STATUS_MODE_FLAG_INDEX_0 = 0;

	const uint8_t LCDC_LCD_ENABLE_INDEX = 7;
	const uint8_t LCDC_WINDOW_TILE_MAP_INDEX = 6;
	const uint8_t LCDC_WINDOW_ENABLED_INDEX = 5;
	const uint8_t LCDC_BG_WINDOW_TILE_DATA_INDEX = 4;
	const uint8_t LCDC_BG_TILE_MAP_INDEX = 3;
	const uint8_t LCDC_OBJ_SIZE_INDEX = 2;
	const uint8_t LCDC_OBJ_ENABLE_INDEX = 1;
	const uint8_t LCDC_BG_WINDOW_PRIORITY_INDEX = 0;

	const uint16_t SCY_ADDRESS = 0xFF42;
	const uint16_t SCX_ADDRESS = 0xFF43;
	const uint16_t LY_ADDRESS = 0xFF44;
	const uint16_t LYC_ADDRESS = 0xFF45;
	const uint16_t WY_ADDRESS = 0xFF4A;
	const uint16_t WX_ADDRESS = 0xFF4B;

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

	const uint16_t DMA_TRANSFER_REGISTER_ADDRESS = 0xFF46;
	const uint16_t DMA_TRANSFER_DESTINATION_START_ADDRESS = 0xFE00;
	const uint8_t DMA_TRANSFER_DURATION = 160;

	const uint16_t CYCLES_PER_SCANLINE = 456;
	const uint8_t HBLANK_DURATION = 208;

	const std::map<PPU::PixelFetcherState, uint32_t> PPU::STATE_DURATIONS =
	{
		{PixelFetcherState::FetchingTile, 2},
		{PixelFetcherState::FetchingLowTileData, 2},
		{PixelFetcherState::FetchingHighTileData, 2},
		{PixelFetcherState::Sleeping, 2},
		// TODO: This might be wrong
		{PixelFetcherState::PushingData, 0},
	};

	PPU::PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram, DMATransferRegister& dmaRegister) :
		vram(vram), display(display), memoryManagementUnit(memoryManagementUnit),
		framebuffer(display, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT), dmaRegister(dmaRegister)
	{

	}

	void PPU::Cycle(uint32_t duration)
	{
		if (duration == 0) return;

		RefreshLY();

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
			duration = FetchSpritesOnScanline(currentScanline, spritesOnCurrentScanline, duration);
			break;
		case PPUMode::LCDTransfer:
			duration = HandleLCDTransferMode(duration);
			break;
		}

		Cycle(duration);
	}

	uint32_t PPU::HandleHBlankMode(uint32_t duration)
	{
		currentScanlineElapsedTime += duration;
		currentModeElapsedTime += duration;

		if (currentScanlineElapsedTime >= CYCLES_PER_SCANLINE)
		{
			// Save the leftover cycles
			duration = currentScanlineElapsedTime - CYCLES_PER_SCANLINE;

			currentScanline++;
			currentScanlineX = 0;
			currentScanlineElapsedTime = 0;

			//Logger::WriteError("HBlank length: " + std::to_string(currentModeElapsedTime));
			currentModeElapsedTime = 0;

			if (currentScanline == GB_SCREEN_HEIGHT)
				EnterVBlankMode();
			else
				EnterOAMSearchMode();
		}

		return duration;
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

			if (currentScanline == GB_VBLANK_END_Y)
			{
				//Logger::WriteError("VBlank length: " + std::to_string(currentModeElapsedTime));
				currentModeElapsedTime = 0;

				/*auto currentTime = std::chrono::system_clock::now();
				auto deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - frameStartTime).count();

				Logger::WriteWarning("Time to reach VBlank: " + std::to_string(deltaTime) + " Seconds");*/

				currentScanline = 0;
				DrawFrameBuffer();
				EnterOAMSearchMode();

				return duration;
			}

			currentScanline++;
		}

		return duration;
	}

	uint32_t PPU::HandleLCDTransferMode(uint32_t duration)
	{
		currentScanlineElapsedTime += STATE_DURATIONS.at(pixelFetcherState);
		currentModeElapsedTime += STATE_DURATIONS.at(pixelFetcherState);

		switch (pixelFetcherState)
		{
		case PixelFetcherState::FetchingTile:
			currentPixelFetcherTileIndex = FetchTileMapIndex(currentScanlineX, currentScanline, GB_SCREEN_WIDTH, TileMapType::BackgroundAndWindow);
			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingLowTileData, duration);
			break;
		case PixelFetcherState::FetchingLowTileData:
			currentPixelFetcherTileDataLow = memoryManagementUnit.GetByte(GetTileAddressFromTileMaps(currentPixelFetcherTileIndex, currentScanlineX, currentScanline) + 1);
			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingHighTileData, duration);
			break;
		case PixelFetcherState::FetchingHighTileData:
			currentPixelFetcherTileDataHigh = memoryManagementUnit.GetByte(GetTileAddressFromTileMaps(currentPixelFetcherTileIndex, currentScanlineX, currentScanline));
			duration = TransitionToPixelFetcherState(PixelFetcherState::Sleeping, duration);
			break;
		case PixelFetcherState::Sleeping:
			duration = TransitionToPixelFetcherState(PixelFetcherState::PushingData, duration);
			break;
		case PixelFetcherState::PushingData:
			uint8_t queueSize = tileMapPixelQueue.size();
			GetPixelsFromTileScanline(currentPixelFetcherTileDataLow, currentPixelFetcherTileDataHigh, currentScanlineX, currentScanline, GB_SCREEN_WIDTH, tileMapPixelQueue);

			// Increase the X position based on how many pixels were added to the queue.
			// The number of added pixels should generally be 8, but this handles the case where that's not true.
			uint8_t diff = tileMapPixelQueue.size() - queueSize;
			currentScanlineX += diff;

			duration = TransitionToPixelFetcherState(PixelFetcherState::FetchingTile, duration);

			if (currentScanlineX == GB_SCREEN_WIDTH)
			{
				currentScanlineElapsedTime += 2;
				currentModeElapsedTime += 2;

				// TODO: Remove hardcoded values and account for number of sprites
				if (currentModeElapsedTime >= 172)
				{
					if (GetLCDControlBit(LCDC_OBJ_ENABLE_INDEX))
					{
						GetSpritePixelsForScanline(spritesOnCurrentScanline, currentScanlineX, currentScanline, spritePixelQueue);
						spritesOnCurrentScanline.clear();
					}

					//Logger::WriteError("LCD transfer end scan line elapsed time: " + std::to_string(currentScanlineElapsedTime));
					//Logger::WriteError("LCD transfer length: " + std::to_string(currentModeElapsedTime));
					currentModeElapsedTime = 0;
					EnterHBlankMode();
				}
			}

			break;
		}

		return duration;
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
				uint8_t sourceData = memoryManagementUnit.GetByte(dmaRegister.GetSourceStartAddress() + dmaTransferElapsedTime);
				memoryManagementUnit.SetByte(DMA_TRANSFER_DESTINATION_START_ADDRESS + dmaTransferElapsedTime, sourceData);

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

	uint16_t PPU::FetchTileMapIndex(uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, TileMapType tileMapType, bool ignoreScrolling)
	{
		uint16_t tileMapAddress = DEFAULT_TILE_MAP_ADDRESS;

		bool isBackgroundTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::BackgroundOnly) != 0;
		bool isWindowTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::WindowOnly) != 0;

		uint8_t windowX = memoryManagementUnit.GetByte(WX_ADDRESS);
		uint8_t windowY = memoryManagementUnit.GetByte(WY_ADDRESS);

		bool isBackgroundTile = false;
		if (GetLCDControlBit(LCDC_BG_TILE_MAP_INDEX))
		{
			// If the current X is outside of the window then the current tile is a background tile
			// and the tile map at 0x9C00 should be used to fetch the tile.
			if (ignoreScrolling || (scanlineX < windowX || (scanlineX >= windowX + TILE_MAP_PIXEL_WIDTH)))
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
		if (GetLCDControlBit(LCDC_WINDOW_TILE_MAP_INDEX))
		{
			// If the current X is inside of the window then the current tile is a window tile
			// and the tile map at 0x9C00 should be used to fetch the tile.
			if (ignoreScrolling || (scanlineX >= windowX && (scanlineX < windowX + TILE_MAP_PIXEL_WIDTH)))
			{
				isWindowTile = true;
				tileMapAddress = ALTERNATE_TILE_MAP_ADDRESS;
			}
		}
		else
		{
			isWindowTile = GetLCDControlBit(LCDC_WINDOW_ENABLED_INDEX);
		}

		uint8_t tileX = 0;
		uint8_t tileY = 0;

		if (isWindowTile && isWindowTilesEnabled)
		{
			// TODO: Revisit
			tileX = (windowX - 7) + scanlineX;
			tileY = windowX + scanlineY;
			//tileX = static_cast<uint8_t>(std::floor((std::max((int)(windowX - 7), 0) + scanlineX) / 8.0));
			//tileY = static_cast<uint8_t>(std::floor(windowX + scanlineY) / 8.0);
		}
		else if (isBackgroundTile && isBackgroundTilesEnabled)
		{
			// IgnoreScrolling is useful for drawing the entire background map for debugging purposes.
			tileX = ignoreScrolling ? std::floor(scanlineX / TILE_WIDTH_IN_PIXELS) : static_cast<uint8_t>(std::floor(windowX / TILE_WIDTH_IN_PIXELS) + std::floor(scanlineX / TILE_WIDTH_IN_PIXELS)) & 0x1F;
			tileY = ignoreScrolling ? std::floor(scanlineY / TILE_HEIGHT_IN_PIXELS) : std::floor(((scanlineY + windowY) & 255) / TILE_HEIGHT_IN_PIXELS);
		}

		int tileMapIndex = tileX + tileY * TILE_MAP_WIDTH;
		return memoryManagementUnit.GetByte(tileMapAddress + tileMapIndex);
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
		bool isUnsignedAddressingMode = memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_WINDOW_TILE_DATA_INDEX);

		tileIndex = isUnsignedAddressingMode ? tileIndex : (int8_t)tileIndex;
		int tileDataOffset = (tileIndex * TILE_SIZE_IN_BYTES) + ((scanlineY % TILE_WIDTH_IN_PIXELS) * 2);

		return tileDataOffset + (isUnsignedAddressingMode ? UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS : SIGNED_ADDRESSING_MODE_BASE_ADDRESS);
	}

	void PPU::EnterHBlankMode()
	{
		currentMode = PPUMode::HBlank;

		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 3);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 0);
		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);

		while (tileMapPixelQueue.size() > 0)
		{
			PixelData data = tileMapPixelQueue.front();
			tileMapPixelQueue.pop();

			if (data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			framebuffer.SetPixel(data.x, data.y, color, color, color, 255);
		}

		while (spritePixelQueue.size() > 0)
		{
			PixelData data = spritePixelQueue.front();
			spritePixelQueue.pop();

			// TODO: Check background priority
			if (data.colorID == PixelColorID::White || data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			framebuffer.SetPixel(data.x, data.y, color, color, color, 255);
		}
	}

	void PPU::EnterVBlankMode()
	{
		currentMode = PPUMode::VBlank;

		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 4);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 0);

		RequestInterrupt(memoryManagementUnit, InterruptType::VBlank);
		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
	}

	void PPU::EnterOAMSearchMode()
	{
		//Logger::WriteError("OAM search scanline time start: " + std::to_string(currentScanlineElapsedTime));
		currentMode = PPUMode::SearchingOAM;

		ChangeLCDStatusBit(LCD_STATUS_OAM_STAT_INTERRUPT_INDEX, true);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 0);
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 1);

		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
	}

	void PPU::EnterLCDTransferMode()
	{
		//Logger::WriteError("LCD transfer scanline time start: " + std::to_string(currentScanlineElapsedTime));
		currentMode = PPUMode::LCDTransfer;

		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 0);
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 1);

		//frameStartTime = std::chrono::system_clock::now();
	}

	void PPU::DrawFrameBuffer()
	{
		framebuffer.UploadData();
		display.Draw(framebuffer);
		framebuffer.Clear();
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
					break;
				else
					return 0;
			}

			duration -= 2;
		}

		currentModeElapsedTime = 0;
		currentSpriteIndex = 0;
		EnterLCDTransferMode();

		return duration;
	}

	void PPU::GetSpritePixelsForScanline(const std::vector<Sprite>& spritesOnCurrentScanline, uint8_t scanlineX, uint8_t scanlineY, std::queue<PixelData>& pixelQueue)
	{
		for (Sprite s : spritesOnCurrentScanline)
		{
			int16_t spriteScanline = 16 - (s.y - scanlineY);

			uint16_t scanlineAddress = s.tileAddress + (spriteScanline * 2);

			for (uint8_t x = 0; x < 8; x++)
			{
				uint8_t px = 7 - x;

				int tilePixelX = px % 8;

				uint16_t modifier = (1 << tilePixelX);

				// The bit at the specified index in the low byte
				// will determine the most significant bit of the color, while the bit 
				// in the high byte will determine the least significant bit of the color.
				uint8_t low = (memoryManagementUnit.GetByte(scanlineAddress + 1) & modifier) >> tilePixelX;
				uint8_t high = (memoryManagementUnit.GetByte(scanlineAddress) & modifier) >> tilePixelX;

				PixelData pixelData;

				pixelData.x = std::max((s.x - 8) + x, 0);
				pixelData.y = scanlineY;
				pixelData.colorID = static_cast<PixelColorID>((low << 1) | high);

				pixelQueue.push(pixelData);
			}
		}
	}

	PPU::Sprite PPU::FetchSpriteAtIndex(uint8_t index)
	{
		uint16_t attributeAddress = SPRITE_ATTRIBUTE_TABLE_START_ADDRESS + (index * 4);

		Sprite sprite;

		sprite.y = memoryManagementUnit.GetByte(attributeAddress);
		sprite.x = memoryManagementUnit.GetByte(attributeAddress + 1);
		uint8_t tileIndex = memoryManagementUnit.GetByte(attributeAddress + 2);
		uint8_t flags = memoryManagementUnit.GetByte(attributeAddress + 3);

		sprite.xFlip = Arithmetic::GetBit(flags, 5);

		sprite.tileAddress = SPRITE_TILES_TABLE_START_ADDRESS + (tileIndex * TILE_SIZE_IN_BYTES);

		return sprite;
	}

	void PPU::DrawTileMap(Display& display, Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, TileMapType tileMapType)
	{
		uint16_t tileIndex = FetchTileMapIndex(scanlineX, scanlineY, TILE_MAP_PIXEL_WIDTH, tileMapType, true);

		uint8_t low = memoryManagementUnit.GetByte(GetTileAddressFromTileMaps(tileIndex, scanlineX, scanlineY) + 1);
		uint8_t high = memoryManagementUnit.GetByte(GetTileAddressFromTileMaps(tileIndex, scanlineX, scanlineY));

		auto pixelQueue = std::queue<PixelData>();
		GetPixelsFromTileScanline(low, high, scanlineX, scanlineY, TILE_MAP_PIXEL_WIDTH, pixelQueue);

		uint16_t x = scanlineX;
		uint16_t y = scanlineY;

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
			display.Draw(framebuffer);
			framebuffer.Clear();
			scanlineY = 0;
		}

		scanlineX = x;
		scanlineY = y;
	}

	void PPU::DrawSprites(Display& display, Framebuffer& framebuffer, uint8_t& spriteIndex, uint8_t& scanline)
	{
		Sprite sprite = FetchSpriteAtIndex(spriteIndex);

		uint8_t spriteSpacing = 12;
		uint16_t scanlineAddress = sprite.tileAddress + (scanline * 2);

		// TODO: Swapping the order of the pixels is a temporary hack to 
		// address bug that causes tiles to be flipped horizontally when rendered.
		uint8_t tileDataHigh = memoryManagementUnit.GetByte(scanlineAddress);
		uint8_t tileDataLow = memoryManagementUnit.GetByte(scanlineAddress + 1);

		auto pixelQueue = std::queue<PixelData>();
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
			uint8_t xOffset = (spriteIndex % MAX_SPRITES_PER_SCANLINE) * spriteSpacing;
			uint8_t yOffset = std::floor(spriteIndex / (double)MAX_SPRITES_PER_SCANLINE) * spriteSpacing;

			framebuffer.SetPixel(scanlineX + xOffset, scanline + yOffset, color, color, color, 255);
		}

		scanline++;

		if (scanline >= TILE_HEIGHT_IN_PIXELS)
		{
			spriteIndex++;
			scanline = 0;
		}

		if (spriteIndex == MAX_SPRITE_COUNT)
		{
			framebuffer.UploadData();
			display.Draw(framebuffer);
			framebuffer.Clear();
			spriteIndex = 0;
		}
	}

	void PPU::DrawAllTiles(Display& display, Framebuffer& framebuffer, uint16_t& scanlineX, uint16_t& scanlineY)
	{
		uint16_t tileDataAddress = 0x8000;
		uint16_t maxTiles = 384;
		uint16_t width = 32 * TILE_WIDTH_IN_PIXELS;
		uint16_t height = 12 * TILE_HEIGHT_IN_PIXELS;

		uint16_t tileIndex = std::floor(scanlineX / 8.0) + (std::floor(scanlineY / 8.0) * 32);
		//Logger::WriteError("Tile index: " + std::to_string(tileIndex));
	/*	if (tileIndex == 32)
		{
			Logger::WriteError("Scanline X: " + std::to_string(scanlineX));
			Logger::WriteError("Scanline Y: " + std::to_string(scanlineY));
			return;
		}*/

		uint16_t scanlineAddress = tileDataAddress + (tileIndex * TILE_SIZE_IN_BYTES) + ((scanlineY % 8) * 2);

		uint8_t tileDataLow = memoryManagementUnit.GetByte(scanlineAddress + 1);
		uint8_t tileDataHigh = memoryManagementUnit.GetByte(scanlineAddress);

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

			framebuffer.SetPixel(scanlineX + px, scanlineY, color, color, color, 255);
		}

		scanlineX += TILE_WIDTH_IN_PIXELS;

		if (scanlineX >= width)
		{
			scanlineY++;
			scanlineX = 0;
		}

		if (scanlineY >= height)
		{
			framebuffer.UploadData();
			display.Draw(framebuffer);
			framebuffer.Clear();

			scanlineY = 0;
		}
	}

	void PPU::RefreshLY()
	{
		memoryManagementUnit.SetByte(LY_ADDRESS, currentScanline);
		RefreshLYCompare();
	}

	void PPU::RefreshLYCompare()
	{
		bool lyCompare = currentScanline == memoryManagementUnit.GetByte(LYC_ADDRESS);
		ChangeLCDStatusBit(LCD_STATUS_LY_COMPARE_FLAG_INDEX, lyCompare);

		if (lyCompare)
		{
			ChangeLCDStatusBit(LCD_STATUS_LY_COMPARE_STAT_INTERRUPT_INDEX, true);
			RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
		}
	}

	void PPU::ChangeLCDControlBit(uint8_t bitIndex, bool isSet)
	{
		memoryManagementUnit.ChangeBit(LCD_CONTROL_REGISTER_ADDRESS, bitIndex, isSet);
	}

	bool PPU::GetLCDControlBit(uint8_t bitIndex)
	{
		return memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, bitIndex);
	}

	void PPU::ChangeLCDStatusBit(uint8_t bitIndex, bool isSet)
	{
		memoryManagementUnit.ChangeBit(LCD_STATUS_REGISTER_ADDRESS, bitIndex, isSet);
	}

	bool PPU::GetLCDStatusBit(uint8_t bitIndex)
	{
		return memoryManagementUnit.GetBit(LCD_STATUS_REGISTER_ADDRESS, bitIndex);
	}
}