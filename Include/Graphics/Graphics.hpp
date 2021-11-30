#pragma once
#include <cstdint>
#include "Memory/MemoryMap.hpp"
#include "Memory/Register8.hpp"
#include "Graphics/Color.hpp"

namespace SHG
{
	// PPU Mode Durations
	// ------------------------------
	// HBlank = 87 - 204 cycles
	// VBlank = 4560 cycles
	// OAM Search = 80 cycles
	// LCD Transfer = 172 - 289 cycles

	// LCD Status Register Bits (FF41)
	// ------------------------------
	// 0-1 = Mode Flag
	// 2 = LYC=LY Flag
	// 3 = HBlank Stat Interrupt Source
	// 4 = VBlank Stat Interrupt Source
	// 5 = OAM Stat Interrupt Source
	// 6 = LYC=LY Stat Interrupt Source

	const uint16_t SCANLINE_DURATION_IN_CYCLES = 456;
	const uint8_t STAT_HBLANK_INTERRUPT_SOURCE_BIT_INDEX = 3;
	const uint8_t STAT_VBLANK_INTERRUPT_SOURCE_BIT_INDEX = 4;
	const uint8_t STAT_OAM_INTERRUPT_SOURCE_BIT_INDEX = 5;
	const uint8_t STAT_LYC_INTERRUPT_SOURCE_BIT_INDEX = 6;
	const uint8_t VBLANK_END_LINE = 153;
	const uint8_t STAT_LYC_FLAG_INDEX = 2;

	// LCD Control Register Bits (FF40)
	// ------------------------------
	// 0 = Background and Window Enable
	// 1 = OBJ Enable
	// 2 = OBJ Size (0 = 8x8, 1 = 8x16)
	// 3 = Background Tile Map Area (0 = $9800-$9BFF, 1 = $9C00-$9FFF)
	// 4 = Background and Window Tile Data Area (0 = $8800-$97FF, 1 = $8000-$8FFF)
	// 5 = Window Enable
	// 6 = Window Tile Map Area

	const uint8_t LCDC_BG_WINDOW_ENABLE_BIT_INDEX = 0;
	const uint8_t LCDC_OBJ_ENABLE_BIT_INDEX = 1;
	const uint8_t LCDC_OBJ_SIZE_BIT_INDEX = 2;
	const uint8_t LCDC_BG_TILE_MAP_AREA_BIT_INDEX = 3;
	const uint8_t LCDC_BG_WINDOW_ADDRESSING_MODE_BIT_INDEX = 4;
	const uint8_t LCDC_WINDOW_ENABLE_BIT_INDEX = 5;
	const uint8_t LCDC_WINDOW_TILE_MAP_AREA_BIT_INDEX = 6;
	const uint8_t LCDC_PPU_ENABLE_BIT_INDEX = 7;

	// Sprite Attribute Table (OAM)
	// ------------------------------
	// Byte 0 = Y Position
	// Byte 1 = X Position
	// Byte 2 = Tile Index
	// Byte 3 = Attributes/Flags
	//     Bit 0-2 = Palette number (CGB Mode)
	//     Bit 3 = Tile VRAM Bank (CGB Mode)
	//     Bit 4 = Palette number (Non-CGB Mode)
	//     Bit 5 = X Flip (0 = Normal, 1 = Horizontal flip)
	//     Bit 6 = Y Flip (0 = Normal, 1 = Vertical flip)
	//     Bit 7 = BG and Window over OBJ (0 = No, 1 = BG and Window colors 1-3 over Sprite)

	const uint16_t SPRITE_ATTRIBUTE_TABLE_START_ADDRESS = 0xFE00;
	const uint16_t SPRITE_ATTRIBUTE_TABLE_END_ADDRESS = 0xFE9F;
	const uint8_t SPRITE_ATTRIBUTE_BYTE_COUNT = 4;
	const uint8_t SPRITE_FETCH_DURATION_IN_CYCLES = 2;
	const uint8_t MAX_SPRITES_PER_SCANLINE = 10;
	const uint8_t MAX_SPRITE_COUNT = 40;
	const uint8_t OAM_SEARCH_MODE_DURATION_IN_CYCLES = SPRITE_FETCH_DURATION_IN_CYCLES * MAX_SPRITE_COUNT;
	const uint8_t MIN_SPRITE_HEIGHT_IN_PIXELS = 8;
	const uint8_t MAX_SPRITE_HEIGHT_IN_PIXELS = 16;

	const uint8_t TILE_SIZE_IN_BYTES = 16;
	const uint8_t TILE_WIDTH_IN_PIXELS = 8;
	const uint8_t TILE_HEIGHT_IN_PIXELS = 8;
	const uint16_t TILE_MAP_WIDTH_IN_PIXELS = 256;
	const uint16_t TILE_MAP_HEIGHT_IN_PIXELS = 256;
	const uint16_t TILE_MAP_WIDTH_IN_TILES = 32;
	const uint16_t TILE_MAP_HEIGHT_IN_TILES = 32;

	const uint8_t MAX_WX = 166;
	const uint8_t MAX_WY = 143;

	const uint16_t DEFAULT_TILE_MAP_ADDRESS = 0x9800;
	const uint16_t ALTERNATE_TILE_MAP_ADDRESS = 0x9C00;

	const uint16_t UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS = 0x8000;
	const uint16_t SIGNED_ADDRESSING_MODE_BASE_ADDRESS = 0x9000;

	const uint16_t DMA_TRANSFER_DESTINATION_START_ADDRESS = 0xFE00;
	const uint8_t DMA_TRANSFER_DURATION = 160;

	const uint16_t TILE_DATA_START_ADDRESS = 0x8000;

	enum class TileMapType
	{
		Background, 
		Window
	};

	uint16_t GetTileIndexFromTileMaps(const MemoryMap& memoryMap, uint8_t tileX, uint8_t tileY, bool useAlternateTileMapAddress);
	int32_t GetTileAddress(uint16_t tileIndex, uint8_t scanline, bool useUnsignedAddressingMode);
	uint8_t GetColorIndexFromTileData(uint8_t pixelIndex, uint8_t lowTileData, uint8_t highTileData);
	Color GetColorFromColorIndex(uint8_t colorIndex, uint8_t palette);
}