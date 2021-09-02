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
	const uint16_t TILE_DATA_START_ADDRESS = 0x8000;
	const uint16_t TILE_DATA_END_ADDRESS = 0x97FF;
	const uint16_t BACKGROUND_PALETTE_ADDRESS = 0xFF47;
	const uint16_t SPRITE_PALETTE_0_ADDRESS = 0xFF48;
	const uint16_t SPRITE_PALETTE_1_ADDRESS = 0xFF49;
	const uint8_t TILE_MAP_WIDTH = 32;
	const uint8_t TILE_MAP_HEIGHT = 32;

	const uint8_t LCDC_LCD_ENABLE_INDEX = 7;
	const uint8_t LCDC_WINDOW_TILE_MAP_INDEX = 6;
	const uint8_t LCDC_WINDOW_ENABLED_INDEX = 5;
	const uint8_t LCDC_BG_WINDOW_TILE_DATA_INDEX = 4;
	const uint8_t LCDC_BG_TILE_MAP_INDEX = 3;
	const uint8_t LCDC_OBJ_SIZE_INDEX = 2;
	const uint8_t LCDC_OBJ_ENABLE_INDEX = 1;
	const uint8_t LCDC_BG_WINDOW_PRIORITY_INDEX = 0;

	PPU::PPU(Display& display, MemoryMap& memoryManagementUnit, DataStorageDevice& vram) : vram(vram), display(display), memoryManagementUnit(memoryManagementUnit), frameBuffer(display, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT)
	{

	}

	void PPU::Cycle(uint32_t duration)
	{
		uint16_t tileMapAddress = 0x9800;

		for (int cycle = 0; cycle < duration; cycle++)
		{
			// Set the current line that's about to be drawn
			SetLY(currentScanLine);

			bool lyCompare = GetLY() == GetLYC();
			memoryManagementUnit.ChangeBit(LCD_STATUS_REGISTER_ADDRESS, 2, lyCompare);

			if (lyCompare)
			{
				memoryManagementUnit.ChangeBit(LCD_STATUS_REGISTER_ADDRESS, 6, true);
				RequestInterrupt(memoryManagementUnit, InterruptType::LCDStat);
			}

			for (int x = 0; x < GB_SCREEN_WIDTH; x++)
			{
				if (currentScanLine >= GB_SCREEN_HEIGHT) break;

				bool isBackgroundTile = false;
				if (memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_TILE_MAP_INDEX))
				{
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
					tileX = GetWX() + x;
					tileY = GetWY() + currentScanLine;
				}
				else if (isBackgroundTile)
				{
					tileX = static_cast<uint8_t>(std::floor(GetSCX() / 8.0f) + std::floor(x / 8.0f)) & 0x1F;
					tileY = std::floor(((currentScanLine + GetSCY()) & 255) / 8.0f);
				}

				int tileMapIndex = tileX + tileY * TILE_MAP_WIDTH;

				int tileIndex = memoryManagementUnit.GetByte(tileMapAddress + tileMapIndex);

				int tileAddress = 0;
				int tileDataOffset = (tileIndex * 16) + ((currentScanLine % 8) * 2);

				if (memoryManagementUnit.GetBit(LCD_CONTROL_REGISTER_ADDRESS, LCDC_BG_WINDOW_TILE_DATA_INDEX))
				{
					tileAddress = 0x8000 + tileDataOffset;
				}
				else
				{
					tileAddress = 0x9000 + (int8_t)tileDataOffset;
				}

				// TODO: Swapping the order of the pixels is a temporary hack to 
				// address bug that causes tiles to be drawn flipped horizontally.
				uint8_t tileDataHigh = 0;
				uint8_t tempHigh = memoryManagementUnit.GetByte(tileAddress);
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

				int tilePixelX = x % 8;

				uint16_t modifier = (1 << tilePixelX);

				// The bit at the specified index in the low byte
				// will determine the most significant bit of the color, while the bit 
				// in the high byte will determine the least significant bit of the color.
				uint8_t low = (tileDataLow & modifier) >> tilePixelX;
				uint8_t high = (tileDataHigh & modifier) >> tilePixelX;

				uint8_t color = 0;
				auto colorID = static_cast<PixelColorID>((low << 1) | high);

				switch (colorID)
				{
				case PixelColorID::Black:
					color = 0;
					break;
				case PixelColorID::DarkGray:
					color = 100;
					break;
				case PixelColorID::LightGray:
					color = 175;
					break;
				case PixelColorID::White:
					color = 255;
					break;
				default:
					assert(false);
					break;
				}

				frameBuffer.SetPixel(x, currentScanLine, color, color, color, 255);
			}

			memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 3);
			memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
			memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 0);

			currentScanLine++;
			if (currentScanLine == GB_SCREEN_HEIGHT)
			{
				memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 4);
				memoryManagementUnit.ResetBit(LCD_STATUS_REGISTER_ADDRESS, 1);
				memoryManagementUnit.SetBit(LCD_STATUS_REGISTER_ADDRESS, 0);

				RequestInterrupt(memoryManagementUnit, InterruptType::VBlank);
			}

			if (currentScanLine == GB_VBLANK_END_Y)
			{
				currentScanLine = 0;
				frameBuffer.UploadData();
				display.Draw(frameBuffer);
				frameBuffer.Clear();

				return;
			}
		}
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

	std::array<PixelData, 8> PPU::GetPixelDataFromScanLine(uint16_t scanLine)
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