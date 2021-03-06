#include <string>
#include "Graphics/BackgroundPixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Logger.hpp"
#include "Utils/MemoryUtils.hpp"
#include "Utils/GBSpecs.hpp"

namespace ModestGB
{
	const uint8_t TILE_INDEX_FETCH_DURATION_IN_CYCLES = 2;
	const uint8_t TILE_DATA_FETCH_DURATION_IN_CYCLES = 2;
	const uint8_t SLEEP_DURATION_IN_CYCLES = 2;

	BackgroundPixelFetcher::BackgroundPixelFetcher(Memory& vram, Register8& lcdc, Register8& scx, Register8& scy, Register8& wx, Register8& wy)
		: vram(&vram), lcdc(&lcdc), scx(&scx), scy(&scy), wx(&wx), wy(&wy)
	{

	}

	void BackgroundPixelFetcher::SetMode(BackgroundPixelFetcherMode mode)
	{
		currentMode = mode;
	}

	BackgroundPixelFetcherMode BackgroundPixelFetcher::GetCurrentMode()
	{
		return currentMode;
	}

	void BackgroundPixelFetcher::Reset()
	{
		PixelFetcher::Reset();
		currentState = BackgroundPixelFetcherState::FetchingTileIndex;
		currentTileIndex = 0;
		currentStateElapsedCycles = 0;
		currentLowTileData = 0;
		currentHighTileData = 0;
		currentMode = BackgroundPixelFetcherMode::Background;
	}

	void BackgroundPixelFetcher::Tick()
	{
		currentStateElapsedCycles++;

		switch (currentState)
		{
		case BackgroundPixelFetcherState::FetchingTileIndex:
			UpdateTileIndexFetchState();
			break;
		case BackgroundPixelFetcherState::FetchingLowTileData:
			UpdateLowTileDataFetchState();
			break;
		case BackgroundPixelFetcherState::FetchingHighTileData:
			UpdateHighTileDataFetchState();
			break;
		case BackgroundPixelFetcherState::Sleeping:
			UpdateSleepState();
			break;
		case BackgroundPixelFetcherState::PushingPixelsToQueue:
			UpdatePixelPushState();
			break;
		}
	}

	void BackgroundPixelFetcher::UpdateTileIndexFetchState()
	{
		if (currentStateElapsedCycles < TILE_INDEX_FETCH_DURATION_IN_CYCLES)
			return;

		uint8_t tileX = 0;
		uint8_t tileY = 0;

		switch (currentMode)
		{
		case BackgroundPixelFetcherMode::Background:
			tileX = static_cast<uint8_t>(std::floor((scx->Read() + x) / static_cast<float>(TILE_WIDTH_IN_PIXELS))) & 0x1F;
			tileY = static_cast<uint8_t>(std::floor((GetAdjustedY() & 255) / static_cast<float>(TILE_HEIGHT_IN_PIXELS)));

			currentTileIndex = static_cast<uint8_t>(GetTileIndexFromTileMaps(vram, tileX, tileY, lcdc->Read(LCDC_BG_TILE_MAP_AREA_BIT_INDEX)));
			break;
		case BackgroundPixelFetcherMode::Window:
			tileX = static_cast<uint8_t>(std::floor(x / static_cast<float>(TILE_WIDTH_IN_PIXELS)));
			tileY = static_cast<uint8_t>(std::floor(y / static_cast<float>(TILE_HEIGHT_IN_PIXELS)));

			currentTileIndex = static_cast<uint8_t>(GetTileIndexFromTileMaps(vram, tileX, tileY, lcdc->Read(LCDC_WINDOW_TILE_MAP_AREA_BIT_INDEX)));
			break;
		}

		currentState = BackgroundPixelFetcherState::FetchingLowTileData;
		currentStateElapsedCycles = 0;
	}

	void BackgroundPixelFetcher::UpdateLowTileDataFetchState()
	{
		if (currentStateElapsedCycles < TILE_DATA_FETCH_DURATION_IN_CYCLES)
			return;

		currentLowTileData = NormalizedReadFromVRAM(vram, GetTileAddress(currentTileIndex, GetAdjustedY() , lcdc->Read(LCDC_BG_WINDOW_ADDRESSING_MODE_BIT_INDEX)));

		currentState = BackgroundPixelFetcherState::FetchingHighTileData;
		currentStateElapsedCycles = 0;
	}

	void BackgroundPixelFetcher::UpdateHighTileDataFetchState()
	{
		if (currentStateElapsedCycles < TILE_DATA_FETCH_DURATION_IN_CYCLES)
			return;

		currentHighTileData = NormalizedReadFromVRAM(vram, GetTileAddress(currentTileIndex, GetAdjustedY(), lcdc->Read(LCDC_BG_WINDOW_ADDRESSING_MODE_BIT_INDEX)) + 1);

		currentState = BackgroundPixelFetcherState::Sleeping;
		currentStateElapsedCycles = 0;
	}

	void BackgroundPixelFetcher::UpdatePixelPushState()
	{
		// Pixels can only be pushed to the queue if there are less than 9 pixels already in the queue.
		if (GetPixelQueueSize() > TILE_WIDTH_IN_PIXELS)
			return;

		// For background and window pixels, the most significant bits/pixels are pushed to the queue first.
		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			Pixel pixel =
			{
				.colorIndex = GetColorIndexFromTileData(px, currentLowTileData, currentHighTileData),
				.paletteAddress = GB_BACKGROUND_PALETTE_ADDRESS
			};

			PushPixel(pixel);
			x++;
		}

		currentState = BackgroundPixelFetcherState::FetchingTileIndex;
		currentStateElapsedCycles = 0;
	}

	void BackgroundPixelFetcher::UpdateSleepState()
	{
		if (currentStateElapsedCycles < SLEEP_DURATION_IN_CYCLES)
			return;

		currentState = BackgroundPixelFetcherState::PushingPixelsToQueue;
		currentStateElapsedCycles = 0;
	}

	uint8_t BackgroundPixelFetcher::GetAdjustedY()
	{
		return currentMode == BackgroundPixelFetcherMode::Background ? y + scy->Read() : y;
	}
}