#pragma once
#include "Graphics/PixelFetcher.hpp"
#include "Memory/Register8.hpp"

namespace SHG
{
	enum class BackgroundPixelFetcherMode
	{
		Background,
		Window
	};

	enum class BackgroundPixelFetcherState
	{
		FetchingTileIndex,
		FetchingLowTileData,
		FetchingHighTileData,
		Sleeping,
		PushingPixelsToQueue
	};

	class BackgroundPixelFetcher : public PixelFetcher
	{
	public:
		BackgroundPixelFetcher(Memory* memoryMap, Register8* lcdc, Register8* scx, Register8* scy, Register8* wx, Register8* wy);
		void SetMode(BackgroundPixelFetcherMode mode);
		BackgroundPixelFetcherMode GetCurrentMode();
		void Step() override;
		void Reset() override;

	private:
		BackgroundPixelFetcherState currentState = BackgroundPixelFetcherState::FetchingTileIndex;
		BackgroundPixelFetcherMode currentMode = BackgroundPixelFetcherMode::Background;
		uint32_t currentStateElapsedCycles = 0;
		uint8_t currentTileIndex = 0;
		uint8_t currentLowTileData = 0;
		uint8_t currentHighTileData = 0;
		Register8* lcdc = nullptr;
		Register8* scx = nullptr;
		Register8* scy = nullptr;
		Register8* wx = nullptr;
		Register8* wy = nullptr;
		Memory* memoryMap = nullptr;

		void UpdateTileIndexFetchState();
		void UpdateLowTileDataFetchState();
		void UpdateHighTileDataFetchState();
		void UpdateSleepState();
		void UpdatePixelPushState();

		uint8_t GetAdjustedY();
	};
}