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
		SpritePixelFetcher(Memory* memoryMap, Register8* lcdc, BackgroundPixelFetcher* backgroundPixelFetcher);
		void SetSpritesOnScanline(const std::vector<Sprite>& sprites);
		SpritePixelFetcherState GetState();
		void Tick() override;
		void Reset() override;

	private:
		SpritePixelFetcherState currentState = SpritePixelFetcherState::Idle;
		Sprite currentSprite;
		int currentSpriteIndex = 0;
		uint8_t currentLowTileData = 0;
		Memory* memoryMap = nullptr;
		Register8* lcdc = nullptr;
		BackgroundPixelFetcher* backgroundPixelFetcher = nullptr;
		std::vector<Sprite> spritesOnCurrentScanline;

		void UpdateIdleState();
		void UpdateBackgroundFetcherAdvanceState();
		void UpdateLowTileDataFetchState();
		void UpdatePixelPushState();

		uint16_t GetCurrentSpriteTileAddress() const;
		bool ShouldRenderSprite(const Sprite& sprite) const;
	};
}
