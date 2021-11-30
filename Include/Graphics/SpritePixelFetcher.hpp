#pragma once
#include <vector>
#include "Graphics/PixelFetcher.hpp"
#include "Graphics/Sprite.hpp"
#include "Memory/Register8.hpp"
#include "Graphics/BackgroundPixelFetcher.hpp"

namespace SHG
{
	enum class SpritePixelFetcherState
	{
		Idle,
		AdvancingBackgroundPixelFetcher,
		FetchingLowTileData,
		PushingPixelsToQueue
	};

	class SpritePixelFetcher : public PixelFetcher
	{
	public:
		SpritePixelFetcher(MemoryMap* memoryMap, Register8* lcdc, BackgroundPixelFetcher* backgroundPixelFetcher);
		void SetSpritesOnScanline(const std::vector<Sprite>& sprites);
		SpritePixelFetcherState GetState();
		void Step() override;
		void Reset() override;

	private:
		SpritePixelFetcherState currentState = SpritePixelFetcherState::Idle;
		Sprite currentSprite;
		uint8_t currentLowTileData = 0;
		MemoryMap* memoryMap = nullptr;
		Register8* lcdc = nullptr;
		BackgroundPixelFetcher* backgroundPixelFetcher = nullptr;
		std::vector<Sprite> spritesOnCurrentScanline;

		void UpdateIdleState();
		void UpdateBackgroundFetcherAdvanceState();
		void UpdateLowTileDataFetchState();
		void UpdatePixelPushState();

		uint16_t GetCurrentSpriteTileAddress();
	};
}
