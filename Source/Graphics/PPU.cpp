#include <iostream>
#include <cassert>
#include "Graphics/PPU.hpp"
#include "Globals.hpp"
#include "Common/Arithmetic.hpp"
#include "SDL.h"
#include "Logger.hpp"

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

	PPU::PPU(Display& display, DataStorageDevice& memoryManagementUnit) : display(display), memoryManagementUnit(memoryManagementUnit), frameBuffer(FrameBuffer(GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT))
	{

	}

	void PPU::Cycle()
	{
		bool flag = GetLY() == GetLYC();
		SetLCDStatusFlag(LCDStatusFlags::LYCLYFlag, flag);

		if (flag)
		{
			uint8_t interruptFlag = memoryManagementUnit.GetByte(0xFF0F);
			uint8_t interruptEnable = memoryManagementUnit.GetByte(0xFFFF);
			if ((interruptEnable >> 1) & 1)
			{
				ChangeBit(interruptFlag, 1, true);
				memoryManagementUnit.SetByte(0xFF0F, interruptFlag);
			}
		}

		memoryManagementUnit.SetByte(0xFF44, currentScanLine);

		uint16_t tileMapAddress = 0x9800;

		uint8_t tileX = 0;
		uint8_t tileY = 0;

		uint8_t windowX = GetWX();

		bool isWindowTile = GetLCDControlFlag(LCDControlFlags::WindowTileMapArea) == 1;
		bool isBackgroundTile = GetLCDControlFlag(LCDControlFlags::BackgroundTileMapArea) == 1;

		uint8_t addressMode = GetLCDControlFlag(LCDControlFlags::BackgroundAndWindowAddressMode);
		//Logger::Write("[PPU] Tile address mode is " + std::to_string(addressMode));

		uint8_t scrollX = GetSCX();
		uint8_t scrollY = GetSCY();
		uint16_t tileIndexOffset = 0;
		uint16_t tileAddress = 0;
		uint8_t tileDataHigh = 0;
		uint8_t tileDataLow = 0;
		std::array<PixelData, 8> pixelData;

		for (int x = 0; x < GB_SCREEN_WIDTH / 8; x++)
		{
			if (isWindowTile)
			{
				// Use address 0x9C00 if the scan line's X is inside of the window
				if (x >= windowX && x < windowX + GB_SCREEN_WIDTH) tileMapAddress = 0x9C00;

				tileX = x;
				tileY = std::floor(currentScanLine / 8);
			}
			else
			{
				// Use address 0x9C00 if the scan line's X is not inside of the window
				if (x < GetWX() || x >= windowX + GB_SCREEN_WIDTH) tileMapAddress = 0x9C00;

				tileX = ((scrollX / 8) + x) & 0x1F;
				tileY = (currentScanLine + scrollY) & 255;
			}

			tileIndexOffset = (tileX * 8) + tileY;

			switch (addressMode)
			{
			case 0:
				// Tiles with indexes 0-127 will be located in the 0x8000 - 0x87FF range, 
				// while tiles with indexes 128-255 will be in the 0x8800 - 0x8FFF range.
				tileAddress = TILE_DATA_START_ADDRESS + tileIndexOffset;
				break;
			case 1:
				// Tiles with indexes 0-127 will be located in the 0x9000 - 0x97FF range, 
				// while tiles with indexes 128-255 will be in the 0x8800 - 0x8FFF range.
				tileAddress = (TILE_DATA_START_ADDRESS + 800) + (tileIndexOffset + 255);
				break;
			default:
				break;
			}
			//Logger::Write("[PPU] Tile address: " + ConvertToHexString(tileAddress, 4));
			tileDataHigh = memoryManagementUnit.GetByte(tileAddress);
			tileDataLow = memoryManagementUnit.GetByte(tileAddress + 1);
			pixelData = GetPixelDataFromScanLine((tileDataHigh << 8) | tileDataLow);

			for (int px = 0; px < pixelData.size(); px++)
			{
				frameBuffer.SetPixel(px, currentScanLine, 20 * (int)pixelData[px].colorID);
			}
		}

		currentScanLine++;
		SetLCDStatusMode(LCDStatusModes::HBlank);

		//Logger::Write("[PPU] Entering HBlank");

		if (currentScanLine >= GB_SCREEN_HEIGHT)
		{
			currentScanLine = 0;
			//display.Draw(frameBuffer);
			Logger::Write("[PPU] Entering VBlank");
			SetLCDStatusMode(LCDStatusModes::VBlank);
			uint8_t interruptFlag = memoryManagementUnit.GetByte(0xFF0F);
			uint8_t interruptEnable = memoryManagementUnit.GetByte(0xFFFF);
			ChangeBit(interruptFlag, 0, true);
			ChangeBit(interruptEnable, 0, true);
			memoryManagementUnit.SetByte(0xFF0F, interruptFlag);
			memoryManagementUnit.SetByte(0xFFFF, interruptEnable);
			//Logger::Write("[PPU] Entering VBlank");
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

	void PPU::SetLCDControlFlag(LCDControlFlags flag, bool value)
	{
		uint8_t statusRegister = memoryManagementUnit.GetByte(LCD_CONTROL_REGISTER_ADDRESS);
		ChangeBit(statusRegister, (uint8_t)flag, value);
		memoryManagementUnit.SetByte(LCD_CONTROL_REGISTER_ADDRESS, statusRegister);
	}

	void PPU::SetLCDStatusFlag(LCDStatusFlags flag, bool value)
	{
		uint8_t statusRegister = memoryManagementUnit.GetByte(LCD_STATUS_REGISTER_ADDRESS);
		ChangeBit(statusRegister, (uint8_t)flag, value);
		memoryManagementUnit.SetByte(LCD_STATUS_REGISTER_ADDRESS, statusRegister);
	}

	void PPU::SetLCDStatusMode(LCDStatusModes mode)
	{
		uint8_t statusRegister = memoryManagementUnit.GetByte(LCD_STATUS_REGISTER_ADDRESS);
		ChangeBit(statusRegister, (uint8_t)LCDStatusFlags::ModeFlag0, ((uint8_t)mode) & 1);
		ChangeBit(statusRegister, (uint8_t)LCDStatusFlags::ModeFlag1, (((uint8_t)mode) & 2) >> 1);
		memoryManagementUnit.SetByte(LCD_STATUS_REGISTER_ADDRESS, statusRegister);
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