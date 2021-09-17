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
	const uint16_t LCD_SCY_ADDRESS = 0xFF42;
	const uint16_t LCD_SCX_ADDRESS = 0xFF43;
	const uint16_t LCD_LY_ADDRESS = 0xFF44;
	const uint16_t LCD_LYC_ADDRESS = 0xFF45;
	const uint16_t LCD_WY_ADDRESS = 0xFF4A;
	const uint16_t LCD_WX_ADDRESS = 0xFF4B;

	const uint8_t LCDC_LCD_ENABLE_INDEX = 7;
	const uint8_t LCDC_WINDOW_TILE_MAP_INDEX = 6;
	const uint8_t LCDC_WINDOW_ENABLED_INDEX = 5;
	const uint8_t LCDC_BG_WINDOW_TILE_DATA_INDEX = 4;
	const uint8_t LCDC_BG_TILE_MAP_INDEX = 3;
	const uint8_t LCDC_OBJ_SIZE_INDEX = 2;
	const uint8_t LCDC_OBJ_ENABLE_INDEX = 1;
	const uint8_t LCDC_BG_WINDOW_PRIORITY_INDEX = 0;

	const uint16_t DEFAULT_TILE_MAP_ADDRESS = 0x9800;

	const uint16_t SPRITE_TILES_TABLE_START_ADDRESS = 0x8000;
	const uint16_t SPRITE_TILES_TABLE_END_ADDRESS = 0x8FFF;
	const uint16_t SPRITE_ATTRIBUTE_TABLE_START_ADDRESS = 0xFE00;
	const uint16_t SPRITE_ATTRIBUTE_TABLE_END_ADDRESS = 0xFE9F;
	const uint16_t SPRITE_PALETTE_0_ADDRESS = 0xFF48;
	const uint16_t SPRITE_PALETTE_1_ADDRESS = 0xFF49;
	const uint8_t SPRITE_TILES_TABLE_SIZE = 40;

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

	const std::map<PixelFetcherState, uint32_t> STATE_DURATIONS =
	{
		{PixelFetcherState::Idle, 0},
		{PixelFetcherState::FetchingTile, 2},
		{PixelFetcherState::FetchingLowTileData, 2},
		{PixelFetcherState::FetchingHighTileData, 2},
		{PixelFetcherState::Sleeping, 2},
		{PixelFetcherState::PushingData, 1},
	};

	PPU::PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram) : vram(vram), display(display), memoryManagementUnit(memoryManagementUnit), framebuffer(display, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT)
	{

	}

	void PPU::Step(uint32_t duration)
	{
		if (duration == 0) return;
		SetLY(currentScanlineY);

		bool lyCompare = GetLY() == GetLYC();
		memoryManagementUnit.ChangeBit(LCD_STATUS_REGISTER_ADDRESS, 2, lyCompare);

		if (lyCompare)
		{
			memoryManagementUnit.ChangeBit(LCD_STATUS_REGISTER_ADDRESS, 6, true);
			RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
		}

		switch (pixelFetcherState)
		{
		case PixelFetcherState::Idle:
			TransitionToPixelFetcherState(PixelFetcherState::FetchingTile, duration);
			Step(duration);
			break;
		case PixelFetcherState::FetchingTile:
			currentBackgroundTileIndex = FetchTileIndex(currentScanlineX, currentScanlineY, GB_SCREEN_WIDTH, TileMapType::BackgroundAndWindow);
			TransitionToPixelFetcherState(PixelFetcherState::FetchingLowTileData, duration);
			Step(duration);
			break;
		case PixelFetcherState::FetchingLowTileData:
			currentBackgroundTileScanlineLow = FetchTileData(GetBackgroundTileAddress(currentBackgroundTileIndex, currentScanlineX, currentScanlineY) + 1);
			TransitionToPixelFetcherState(PixelFetcherState::FetchingHighTileData, duration);
			Step(duration);
			break;
		case PixelFetcherState::FetchingHighTileData:
			currentBackgroundTileScanlineHigh = FetchTileData(GetBackgroundTileAddress(currentBackgroundTileIndex, currentScanlineX, currentScanlineY));
			TransitionToPixelFetcherState(PixelFetcherState::Sleeping, duration);
			Step(duration);
			break;
		case PixelFetcherState::Sleeping:
			TransitionToPixelFetcherState(PixelFetcherState::PushingData, duration);
			Step(duration);
			break;
		case PixelFetcherState::PushingData:
			uint8_t queueSize = backgroundPixelQueue.size();
			GetPixelsFromTileScanline(currentBackgroundTileScanlineLow, currentBackgroundTileScanlineHigh, currentScanlineX, currentScanlineY, GB_SCREEN_WIDTH, backgroundPixelQueue);

			// Increase the X position based on how many pixels were added to the queue.
			// The number of added pixels should generally be 8, but this handles the case where that's not true.
			currentScanlineX += backgroundPixelQueue.size() - queueSize;

			if (currentScanlineX == GB_SCREEN_WIDTH)
				HandleHBlankEvents();

			if (currentScanlineY == GB_SCREEN_HEIGHT)
				HandleVBlankEvents();

			if (currentScanlineY == GB_VBLANK_END_Y)
				RenderQueuedPixels();

			TransitionToPixelFetcherState(PixelFetcherState::Idle, duration);
			Step(duration);
			break;
		}
	}

	uint16_t PPU::FetchTileIndex(uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, TileMapType tileMapType, bool ignoreScrolling)
	{
		uint16_t tileMapAddress = DEFAULT_TILE_MAP_ADDRESS;
		uint16_t endX = std::clamp(scanlineX + TILE_WIDTH_IN_PIXELS, 0, (int)tileMapRegionWidth);

		bool isBackgroundTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::BackgroundOnly) != 0;
		bool isWindowTilesEnabled = ((uint8_t)tileMapType & (uint8_t)TileMapType::WindowOnly) != 0;

		for (uint16_t x = scanlineX; x < endX; x++)
		{
			bool isBackgroundTile = false;
			if (memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_TILE_MAP_INDEX))
			{
				// If the current X is outside of the window then the current tile is a background tile
				// and the tile map at 0x9C00 should be used to fetch the tile.
				if (ignoreScrolling || (x < GetWX() || (x >= GetWX() + TILE_MAP_PIXEL_WIDTH)))
				{
					isBackgroundTile = true;
					tileMapAddress = 0x9C00;
				}
			}
			else
			{
				isBackgroundTile = true;
			}

			bool isWindowTile = false;
			if (memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_WINDOW_TILE_MAP_INDEX))
			{
				// If the current X is inside of the window then the current tile is a window tile
				// and the tile map at 0x9C00 should be used to fetch the tile.
				if (ignoreScrolling || (x >= GetWX() && (x < GetWX() + TILE_MAP_PIXEL_WIDTH)))
				{
					isWindowTile = true;
					tileMapAddress = 0x9C00;
				}
			}
			else
			{
				isWindowTile = memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_WINDOW_ENABLED_INDEX);
			}

			uint8_t tileX = 0;
			uint8_t tileY = 0;

			if (isWindowTile && isWindowTilesEnabled)
			{
				// TODO: Revisit
				tileX = (GetWX() - 7) + x;
				tileY = GetWY() + scanlineY;
			}
			else if (isBackgroundTile && isBackgroundTilesEnabled)
			{
				tileX = ignoreScrolling ? std::floor(x / TILE_WIDTH_IN_PIXELS) : static_cast<uint8_t>(std::floor(GetSCX() / TILE_WIDTH_IN_PIXELS) + std::floor(x / TILE_WIDTH_IN_PIXELS)) & 0x1F;
				tileY = ignoreScrolling ? std::floor(scanlineY / TILE_HEIGHT_IN_PIXELS) : std::floor(((scanlineY + GetSCY()) & 255) / TILE_HEIGHT_IN_PIXELS);
			}

			int tileMapIndex = tileX + tileY * TILE_MAP_WIDTH;
			return memoryManagementUnit.GetByte(tileMapAddress + tileMapIndex);
		}
	}

	uint8_t PPU::FetchTileData(uint16_t address)
	{
		uint8_t data = 0;
		uint8_t temp = memoryManagementUnit.GetByte(address);

		// TODO: Swapping the order of the pixels is a temporary hack to 
		// address bug that causes tiles to be drawn flipped horizontally.
		for (int b = 0; b < 8; b++)
		{
			Arithmetic::ChangeBit(data, b, Arithmetic::GetBit(temp, 7 - b));
		}

		return data;
	}

	void PPU::GetPixelsFromTileScanline(uint8_t rawScanlineDataLow, uint8_t rawScanlineDataHigh, uint8_t scanlineX, uint8_t scanlineY, uint16_t tileMapRegionWidth, std::queue<PixelData>& pixelQueue)
	{
		uint16_t endX = std::clamp(scanlineX + 8, 0, (int)tileMapRegionWidth);
		for (uint16_t x = scanlineX; x < endX; x++)
		{
			int tilePixelX = x % 8;

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

	int32_t PPU::GetBackgroundTileAddress(uint16_t tileIndex, uint8_t scanlineX, uint8_t scanlineY)
	{
		bool isUnsignedAddressingMode = memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_WINDOW_TILE_DATA_INDEX);

		tileIndex = isUnsignedAddressingMode ? tileIndex : (int8_t)tileIndex;
		int tileDataOffset = (tileIndex * TILE_SIZE_IN_BYTES) + ((scanlineY % TILE_WIDTH_IN_PIXELS) * 2);

		return tileDataOffset + (isUnsignedAddressingMode ? UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS : SIGNED_ADDRESSING_MODE_BASE_ADDRESS);
	}

	void PPU::HandleHBlankEvents()
	{
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 3);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 0);
		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);

		currentScanlineY++;
		currentScanlineX = 0;

		while (backgroundPixelQueue.size() > 0)
		{
			PixelData data = backgroundPixelQueue.front();
			backgroundPixelQueue.pop();

			if (data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			framebuffer.SetPixel(data.x, data.y, color, color, color, 255);
		}
	}

	void PPU::HandleVBlankEvents()
	{
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 4);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 0);

		RequestInterrupt(memoryManagementUnit, InterruptType::VBlank);
		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
	}

	void PPU::RenderQueuedPixels()
	{
		currentScanlineY = 0;
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

	void PPU::ProcessSpriteTiles(int scanline, std::queue<PixelData>& pixelQueue)
	{
		for (int i = 0; i < SPRITE_TILES_TABLE_SIZE; i++)
		{
			uint16_t attributeAddress = SPRITE_ATTRIBUTE_TABLE_START_ADDRESS + (i * 4);

			uint8_t spriteY = memoryManagementUnit.GetByte(attributeAddress);

			int8_t spriteScanline = 16 - (spriteY - scanline);

			// Sprite is hidden
			if (spriteScanline <= 0) continue;

			uint8_t spriteX = memoryManagementUnit.GetByte(attributeAddress + 1);

			uint8_t tileIndex = memoryManagementUnit.GetByte(attributeAddress + 2);

			uint16_t tileAddress = (tileIndex * 16) + (spriteScanline * 2);

			uint8_t flags = memoryManagementUnit.GetByte(attributeAddress + 3);

			// TODO: Swapping the order of the pixels is a temporary hack to 
			// address bug that causes tiles to be flipped horizontally when rendered.
			uint8_t tileDataHigh = 0;
			uint8_t tempHigh = memoryManagementUnit.GetByte(tileAddress);

			if (tileIndex != 0)
			{
				Logger::WriteError("\nAttribute x: " + std::to_string(spriteX));
				Logger::WriteError("Tile index: " + std::to_string(tileIndex) + "\n");
			}

			for (int b = 0; b < 8; b++)
			{
				Arithmetic::ChangeBit(tileDataHigh, b, Arithmetic::GetBit(tempHigh, 7 - b));
			}

			uint8_t tileDataLow = 0;
			uint8_t tempLow = memoryManagementUnit.GetByte(tileAddress + 1);
			for (int b = 0; b < 8; b++)
			{
				Arithmetic::ChangeBit(tileDataLow, b, Arithmetic::GetBit(tempLow, 7 - b));
			}

			for (int px = 0; px < 8; px++)
			{
				PixelData data;

				data.x = spriteX - (8 - px);
				data.y = scanline;

				uint16_t modifier = (1 << px);

				// The bit at the specified index in the low byte
				// will determine the most significant bit of the color, while the bit 
				// in the high byte will determine the least significant bit of the color.
				uint8_t low = (tileDataLow & modifier) >> px;
				uint8_t high = (tileDataHigh & modifier) >> px;

				data.colorID = static_cast<PixelColorID>((low << 1) | high);

				pixelQueue.push(data);
			}
		}
	}

	void PPU::TransitionToPixelFetcherState(PixelFetcherState targetState, uint32_t& duration)
	{
		int32_t result = duration - STATE_DURATIONS.at(pixelFetcherState);
		duration = result <= 0 ? 0 : result;
		pixelFetcherState = targetState;
	}

	uint8_t PPU::GetLCDControlFlag(LCDControlFlags flag)
	{
		return (memoryManagementUnit.GetByte(LCD_CONTROL_REGISTER_ADDRESS) >> (int)flag) & 1;
	}

	uint8_t PPU::GetLCDStatusFlag(LCDStatusFlags flag)
	{
		return (memoryManagementUnit.GetByte(LCD_STATUS_REGISTER_ADDRESS) >> (int)flag) & 1;
	}

	// Scroll Y
	uint8_t PPU::GetSCY()
	{
		return memoryManagementUnit.GetByte(LCD_SCY_ADDRESS);
	}

	// Scroll X
	uint8_t PPU::GetSCX()
	{
		return memoryManagementUnit.GetByte(LCD_SCX_ADDRESS);
	}

	// LCD Y Coordinate
	uint8_t PPU::GetLY()
	{
		return memoryManagementUnit.GetByte(LCD_LY_ADDRESS);
	}

	void PPU::SetLY(uint8_t value)
	{
		return memoryManagementUnit.SetByte(LCD_LY_ADDRESS, value);
	}

	// LY Compare
	uint8_t PPU::GetLYC()
	{
		return memoryManagementUnit.GetByte(LCD_LYC_ADDRESS);
	}

	// Window Y Position
	uint8_t PPU::GetWY()
	{
		return memoryManagementUnit.GetByte(LCD_WY_ADDRESS);
	}

	// Window X Position + 7
	uint8_t PPU::GetWX()
	{
		return memoryManagementUnit.GetByte(LCD_WX_ADDRESS);
	}

	std::array<PixelData, 8> PPU::GetPixelDataFromScanline(uint16_t scanLine)
	{
		std::array<PixelData, 8> result{};
		uint8_t index = 0;

		// Isolate the low and high bytes
		uint8_t low = scanLine & 0x00FF;
		uint8_t high = scanLine >> 8;

		for (int i = 0; i < 8; i++)
		{
			uint16_t modifier = (1 << i);

			// The bit at the specified index in the low byte
			// will determine the most significant bit of the color, while the bit 
			// in the high byte will determine the least significant bit of the color.
			low = (low & modifier) >> i;
			high = (high & modifier) >> i;

			result[i].colorID = (PixelColorID)((low << 1) | high);
			// TODO: Fill out other pixel data fields
		}
		return result;
	}

	void PPU::DrawTileMap(Display& display, Framebuffer& framebuffer, uint8_t& scanlineX, uint8_t& scanlineY, TileMapType tileMapType)
	{
		uint16_t tileIndex = FetchTileIndex(scanlineX, scanlineY, TILE_MAP_PIXEL_WIDTH, tileMapType, true);
		uint8_t low = FetchTileData(GetBackgroundTileAddress(tileIndex, scanlineX, scanlineY) + 1);
		uint8_t high = FetchTileData(GetBackgroundTileAddress(tileIndex, scanlineX, scanlineY));

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
}