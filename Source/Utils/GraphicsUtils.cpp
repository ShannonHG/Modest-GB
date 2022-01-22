#include "Utils/GraphicsUtils.hpp"
#include "Utils/Arithmetic.hpp"
#include "Utils/MemoryUtils.hpp"

namespace ModestGB
{
	uint16_t GetTileIndexFromTileMaps(const Memory* vram, uint8_t tileX, uint8_t tileY, bool useAlternateTileMapAddress)
	{
		return NormalizedReadFromVRAM(vram, (tileX + tileY * TILE_MAP_WIDTH_IN_TILES) + (useAlternateTileMapAddress ? ALTERNATE_TILE_MAP_ADDRESS : DEFAULT_TILE_MAP_ADDRESS));
	}

	int32_t GetTileAddress(uint16_t tileIndex, uint8_t scanline, bool useUnsignedAddressingMode)
	{
		tileIndex = useUnsignedAddressingMode ? tileIndex : (int8_t)tileIndex;
		int tileDataOffset = (tileIndex * TILE_SIZE_IN_BYTES) + ((scanline % TILE_WIDTH_IN_PIXELS) * 2);

		return tileDataOffset + (useUnsignedAddressingMode ? UNSIGNED_ADDRESSING_MODE_BASE_ADDRESS : SIGNED_ADDRESSING_MODE_BASE_ADDRESS);
	}

	uint8_t GetColorIndexFromTileData(uint8_t pixelIndex, uint8_t lowTileData, uint8_t highTileData)
	{
		// The bit (with the same index as this pixel) in currentLowTileData specifies the least significant bit of 
		// the pixel's color index, and the bit in currentHighTileData specifies the most significant bit of the color index.
		uint8_t lowBit = (lowTileData >> pixelIndex) & 1;
		uint8_t highBit = ((highTileData >> pixelIndex) & 1);

		return (highBit << 1) | lowBit;
	}

	void GetColorFromColorIndex(uint8_t colorIndex, uint8_t palette, Color& color)
	{
		// Isolate the bits associated with the color index.
		uint8_t colorIndexBits = (palette >> (colorIndex * 2)) & 0b11;

		// Construct the 8-bit color.
		uint8_t rawColor = 255 - ((colorIndexBits << 6) | (colorIndexBits << 4) | (colorIndexBits << 2) | colorIndexBits);

		color.r = color.g = color.b = rawColor;
		color.a = 255;
	}

	uint8_t NormalizedReadFromVRAM(const Memory* vram, uint16_t address)
	{
		return vram->Read(Arithmetic::NormalizeAddress(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS));
	}

	void NormalizedWriteToVRAM(Memory* vram, uint16_t address, uint8_t value)
	{
		vram->Write(Arithmetic::NormalizeAddress(address, GB_VRAM_START_ADDRESS, GB_VRAM_END_ADDRESS), value);
	}
}