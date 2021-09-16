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
	const uint8_t TILE_PIXEL_WIDTH = 8;

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

	PPU::PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram) : vram(vram), display(display), memoryManagementUnit(memoryManagementUnit), frameBuffer(display, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT)
	{

	}

	void PPU::Step(uint32_t duration)
	{
		if (duration == 0) return;
		SetLY(currentScanline);

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
			FetchTileIndex();
			TransitionToPixelFetcherState(PixelFetcherState::FetchingLowTileData, duration);
			Step(duration);
			break;
		case PixelFetcherState::FetchingLowTileData:
			FetchLowTileData();
			TransitionToPixelFetcherState(PixelFetcherState::FetchingHighTileData, duration);
			Step(duration);
			break;
		case PixelFetcherState::FetchingHighTileData:
			FetchHighTileData();
			TransitionToPixelFetcherState(PixelFetcherState::Sleeping, duration);
			Step(duration);
			break;
		case PixelFetcherState::Sleeping:
			TransitionToPixelFetcherState(PixelFetcherState::PushingData, duration);
			Step(duration);
			break;
		case PixelFetcherState::PushingData:
			PushPixelsToBackgroundPixelQueue();

			if (currentX == GB_SCREEN_WIDTH)
				HandleHBlankEvents();

			if (currentScanline == GB_SCREEN_HEIGHT)
				HandleVBlankEvents();

			if (currentScanline == GB_VBLANK_END_Y)
				RenderQueuedPixels();

			TransitionToPixelFetcherState(PixelFetcherState::Idle, duration);
			Step(duration);
			break;
		}
	}

	void PPU::FetchTileIndex()
	{
		uint16_t tileMapAddress = DEFAULT_TILE_MAP_ADDRESS;
		uint8_t endX = std::clamp(currentX + 8, 0, (int)GB_SCREEN_WIDTH);

		for (uint8_t x = currentX; x < endX; x++)
		{
			bool isBackgroundTile = false;
			if (memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_TILE_MAP_INDEX))
			{
				// If the current X is outside of the window then the current tile is a background tile
				// and the tile map at 0x9C00 should be used to fetch the tile.
				if (x < GetWX() || (x >= GetWX() + TILE_MAP_PIXEL_WIDTH))
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
				if (x >= GetWX() && (x < GetWX() + TILE_MAP_PIXEL_WIDTH))
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

			if (isWindowTile)
			{
				tileX = (GetWX() - 7) + x;
				tileY = GetWY() + currentScanline;
				return;
			}
			else if (isBackgroundTile)
			{
				tileX = static_cast<uint8_t>(std::floor(GetSCX() / 8.0f) + std::floor(x / 8.0f)) & 0x1F;
				tileY = std::floor(((currentScanline + GetSCY()) & 255) / 8.0f);
			}

			int tileMapIndex = tileX + tileY * TILE_MAP_WIDTH;
			currentBackgroundTileIndex = memoryManagementUnit.GetByte(tileMapAddress + tileMapIndex);
		}
	}
	void PPU::FetchLowTileData()
	{
		// TODO: Swapping the order of the pixels is a temporary hack to 
		// address bug that causes tiles to be drawn flipped horizontally.
		currentBackgroundTileScanlineLow = 0;
		uint8_t tempLow = memoryManagementUnit.GetByte(GetCurrentBackgroundTileAddress() + 1);
		for (int b = 0; b < 8; b++)
		{
			Arithmetic::ChangeBit(currentBackgroundTileScanlineLow, b, Arithmetic::GetBit(tempLow, 7 - b));
		}
	}

	void PPU::FetchHighTileData()
	{
		// TODO: Swapping the order of the pixels is a temporary hack to 
		// address bug that causes tiles to be drawn flipped horizontally.
		currentBackgroundTileScanlineHigh = 0;
		uint8_t tempHigh = memoryManagementUnit.GetByte(GetCurrentBackgroundTileAddress());
		for (int b = 0; b < 8; b++)
		{
			Arithmetic::ChangeBit(currentBackgroundTileScanlineHigh, b, Arithmetic::GetBit(tempHigh, 7 - b));
		}
	}

	void PPU::PushPixelsToBackgroundPixelQueue()
	{
		uint8_t endX = std::clamp(currentX + 8, 0, (int)GB_SCREEN_WIDTH);
		uint8_t increment = endX - currentX;
		for (uint8_t x = currentX; x < endX; x++)
		{
			int tilePixelX = x % 8;

			uint16_t modifier = (1 << tilePixelX);

			// The bit at the specified index in the low byte
			// will determine the most significant bit of the color, while the bit 
			// in the high byte will determine the least significant bit of the color.
			uint8_t low = (currentBackgroundTileScanlineLow & modifier) >> tilePixelX;
			uint8_t high = (currentBackgroundTileScanlineHigh & modifier) >> tilePixelX;

			PixelData pixelData;

			pixelData.x = x;
			pixelData.y = currentScanline;
			pixelData.colorID = static_cast<PixelColorID>((low << 1) | high);

			backgroundPixelQueue.push(pixelData);
		}

		currentX += increment;
	}

	int32_t PPU::GetCurrentBackgroundTileAddress()
	{
		bool isUnsignedAddressingMode = memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_WINDOW_TILE_DATA_INDEX);

		int tileIndex = isUnsignedAddressingMode ? currentBackgroundTileIndex : (int8_t)currentBackgroundTileIndex;
		int tileDataOffset = (tileIndex * TILE_SIZE_IN_BYTES) + ((currentScanline % TILE_PIXEL_WIDTH) * 2);

		return tileDataOffset + (isUnsignedAddressingMode ? UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS : SIGNED_ADDRESSING_MODE_BASE_ADDRESS);
	}

	void PPU::HandleHBlankEvents()
	{
		memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 3);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
		memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 0);
		RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);

		currentScanline++;
		currentX = 0;

		uint8_t queueSize = backgroundPixelQueue.size();
		for (int i = 0; i < queueSize; i++)
		{
			PixelData data = backgroundPixelQueue.front();
			backgroundPixelQueue.pop();

			if (data.y >= GB_SCREEN_HEIGHT) continue;

			// TODO: Properly implement palettes
			uint8_t color = GetColorFromID(data.colorID);

			frameBuffer.SetPixel(data.x, data.y, color, color, color, 255);
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
		currentScanline = 0;
		frameBuffer.UploadData();
		display.Draw(frameBuffer);
		frameBuffer.Clear();
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
}