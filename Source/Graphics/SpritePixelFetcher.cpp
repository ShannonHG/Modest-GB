#include <string>
#include "Graphics/SpritePixelFetcher.hpp"
#include "Utils/GraphicsUtils.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/MemoryUtils.hpp"
#include "Logger.hpp"

namespace SHG
{
	// TODO: Improve timing accuracy.
	SpritePixelFetcher::SpritePixelFetcher(Memory* memoryMap, Register8* lcdc, BackgroundPixelFetcher* backgroundPixelFetcher)
		: memoryMap(memoryMap), lcdc(lcdc), backgroundPixelFetcher(backgroundPixelFetcher)
	{

	}

	void SpritePixelFetcher::SetSpritesOnScanline(const std::vector<Sprite>& sprites)
	{
		spritesOnCurrentScanline = sprites;
	}

	SpritePixelFetcherState SpritePixelFetcher::GetState()
	{
		return currentState;
	}

	void SpritePixelFetcher::Reset()
	{
		PixelFetcher::Reset();
		currentState = SpritePixelFetcherState::Idle;
		spritesOnCurrentScanline.clear();
		queuedPixels = std::queue<Pixel>();
	}

	void SpritePixelFetcher::Tick()
	{
		switch (currentState)
		{
		case SpritePixelFetcherState::Idle:
			UpdateIdleState();
			break;
		case SpritePixelFetcherState::AdvancingBackgroundPixelFetcher:
			UpdateBackgroundFetcherAdvanceState();
			break;
		case SpritePixelFetcherState::FetchingLowTileData:
			UpdateLowTileDataFetchState();
			break;
		case SpritePixelFetcherState::PushingPixelsToQueue:
			UpdatePixelPushState();
			break;
		}
	}

	void SpritePixelFetcher::UpdateIdleState()
	{
		for (currentSpriteIndex = 0; currentSpriteIndex < spritesOnCurrentScanline.size(); currentSpriteIndex++)
		{
			currentSprite = spritesOnCurrentScanline[currentSpriteIndex];
			if (ShouldRenderSprite(currentSprite))
			{
				// Advance the background pixel fetcher until at least 8 pixels are in its queue.
				if (backgroundPixelFetcher->GetPixelQueueSize() < TILE_WIDTH_IN_PIXELS)
				{
					currentState = SpritePixelFetcherState::AdvancingBackgroundPixelFetcher;
					// Begin executing the update cycle for the new state.
					Tick();
					return;
				}

				currentState = SpritePixelFetcherState::FetchingLowTileData;
				// Begin executing the update cycle for the new state.
				Tick();
				return;
			}
		}
	}

	void SpritePixelFetcher::UpdateBackgroundFetcherAdvanceState()
	{
		if (backgroundPixelFetcher->GetPixelQueueSize() < TILE_WIDTH_IN_PIXELS)
		{
			backgroundPixelFetcher->Tick();

			if (backgroundPixelFetcher->GetPixelQueueSize() >= TILE_WIDTH_IN_PIXELS)
				currentState = SpritePixelFetcherState::FetchingLowTileData;
		}
		else
		{
			currentState = SpritePixelFetcherState::FetchingLowTileData;
			// If the background pixel fetcher already contained 8 or more pixels in its queue, 
			// then instead of wasting this cycle, the sprite pixel fetcher can begin executing the next step.
			Tick();
		}
	}

	void SpritePixelFetcher::UpdateLowTileDataFetchState()
	{
		currentLowTileData = memoryMap->Read(GetCurrentSpriteTileAddress());
		currentState = SpritePixelFetcherState::PushingPixelsToQueue;
	}

	void SpritePixelFetcher::UpdatePixelPushState()
	{
		uint8_t highTileData = memoryMap->Read(GetCurrentSpriteTileAddress() + 1);

		// Add transparent pixels with the lowest priority to the queue. 
		while (queuedPixels.size() < TILE_WIDTH_IN_PIXELS)
			queuedPixels.push(Pixel());

		for (int8_t px = TILE_WIDTH_IN_PIXELS - 1; px >= 0; px--)
		{
			// Ignore any pixels that are off-screen.
			if (spritesOnCurrentScanline[currentSpriteIndex].x + (7 - px) < 0)
			{
				PopPixel();
				continue;
			}

			Pixel newPixel =
			{
				.colorIndex = GetColorIndexFromTileData(currentSprite.xFlip ? 7 - px : px, currentLowTileData, highTileData),
				.paletteAddress = currentSprite.palette == 0 ? GB_SPRITE_PALETTE_0_ADDRESS : GB_SPRITE_PALETTE_1_ADDRESS,
				.backgroundOverSprite = currentSprite.backgroundOverSprite,
				.spriteX = currentSprite.x,
				.spriteOAMIndex = currentSprite.oamIndex
			};

			Pixel existingPixel = PopPixel();

			// If the new pixel is transparent, then the existing pixel has priority. Otherwise, the sprite with the smaller X position has priority, 
			// but if the sprites have the same X position, then the sprite that appears first in OAM has priority.
			if (newPixel.colorIndex != 0 && 
				(newPixel.spriteX < existingPixel.spriteX ||
				(newPixel.spriteX == existingPixel.spriteX && (newPixel.spriteOAMIndex < existingPixel.spriteOAMIndex))))
			{
				queuedPixels.push(newPixel);
			}
			else
			{
				queuedPixels.push(existingPixel);
			}
		}

		// Remove the current sprite to ensure that it's not processed again.
		spritesOnCurrentScanline.erase(spritesOnCurrentScanline.begin() + currentSpriteIndex);
		currentState = SpritePixelFetcherState::Idle;
	}

	uint16_t SpritePixelFetcher::GetCurrentSpriteTileAddress() const
	{
		uint8_t tileIndex = currentSprite.tileIndex;

		// Get the tile-space scanline (a value between 0 and 7 for 8x8 sprites, or 0 and 15 for 8x16 sprites) based on the fetcher's Y position. 
		uint8_t scanline = y - currentSprite.y;

		// Determine the sprite's height (8 or 16).
		uint8_t spriteHeight = lcdc->Read(LCDC_OBJ_SIZE_BIT_INDEX) ? MAX_SPRITE_HEIGHT_IN_PIXELS : MIN_SPRITE_HEIGHT_IN_PIXELS;

		if (currentSprite.yFlip)
			scanline = (spriteHeight - 1) - scanline;

		// If the scanline belongs to the sprite's bottom tile, then increase the tile index so that it points to the bottom tile.
		if (spriteHeight == MAX_SPRITE_HEIGHT_IN_PIXELS && scanline >= MIN_SPRITE_HEIGHT_IN_PIXELS)
			tileIndex++;

		return GetTileAddress(tileIndex, scanline % TILE_HEIGHT_IN_PIXELS, true);
	}

	bool SpritePixelFetcher::ShouldRenderSprite(const Sprite& sprite) const
	{
		return 
			// A sprite should only be drawn if the fetcher's X position is less than 160 and equal to the sprite's X position,
			// or the fetcher's x position is 0 and the sprite's X position is less than 0 (some off-screen pixels), 
			// but greater than -8 (not completely hidden).
			(x < GB_SCREEN_WIDTH && x >= 0 && sprite.x == x) ||
			(x == 0 && (sprite.x < 0 && sprite.x > -TILE_WIDTH_IN_PIXELS));
	}
}