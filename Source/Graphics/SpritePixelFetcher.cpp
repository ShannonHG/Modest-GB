#include <string>
#include "Graphics/SpritePixelFetcher.hpp"
#include "Graphics/Graphics.hpp"
#include "Utils/GBSpecs.hpp"
#include "Utils/GBMemoryMapAddresses.hpp"
#include "Logger.hpp"

namespace SHG
{
	SpritePixelFetcher::SpritePixelFetcher(MemoryMap* memoryMap, Register8* lcdc, BackgroundPixelFetcher* backgroundPixelFetcher)
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

	void SpritePixelFetcher::Step()
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
		for (Sprite& sprite : spritesOnCurrentScanline)
		{
			if (sprite.x == x)
			{
				currentSprite = sprite;
				// Advance the background pixel fetcher until at least 8 pixels are in its queue.
				if (backgroundPixelFetcher->GetPixelQueueSize() < TILE_WIDTH_IN_PIXELS)
				{
					currentState = SpritePixelFetcherState::AdvancingBackgroundPixelFetcher;
					// Begin executing the update cycle for the new state.
					Step();
					return;
				}

				currentState = SpritePixelFetcherState::FetchingLowTileData;
				// Begin executing the update cycle for the new state.
				Step();
				return;
			}
		}
	}

	void SpritePixelFetcher::UpdateBackgroundFetcherAdvanceState()
	{
		if (backgroundPixelFetcher->GetPixelQueueSize() < TILE_WIDTH_IN_PIXELS)
		{
			backgroundPixelFetcher->Step();

			if (backgroundPixelFetcher->GetPixelQueueSize() >= TILE_WIDTH_IN_PIXELS)
				currentState = SpritePixelFetcherState::FetchingLowTileData;
		}
		else
		{
			currentState = SpritePixelFetcherState::FetchingLowTileData;
			// If the background pixel fetcher already contained 8 or more pixels in its queue, 
			// then instead of wasting this cycle, the sprite pixel fetcher can begin executing the next step.
			Step();
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
			Pixel newPixel =
			{
				.colorIndex = GetColorIndexFromTileData(currentSprite.xFlip ? 7 - px : px, currentLowTileData, highTileData),
				.paletteAddress = currentSprite.palette == 0 ? GB_SPRITE_PALETTE_0_ADDRESS : GB_SPRITE_PALETTE_1_ADDRESS,
				.backgroundOverSprite = currentSprite.backgroundOverSprite,
				.spriteX = currentSprite.x,
				.spriteOAMIndex = currentSprite.oamIndex
			};

			Pixel existingPixel = PopPixel();

			// If the new pixel is transparent, then add the existing pixel back to the queue instead.
			if (newPixel.colorIndex == 0)
			{
				queuedPixels.push(existingPixel);
				continue;
			}
			
			// The sprite with the smaller X position has priority, but if the sprites have the same X position, 
			// then the sprite that appears first in OAM has priority.
			if (newPixel.spriteX < existingPixel.spriteX || 
				(newPixel.spriteX == existingPixel.spriteX && newPixel.spriteOAMIndex < existingPixel.spriteOAMIndex))
			{
				queuedPixels.push(newPixel);
			}
			else
			{
				queuedPixels.push(existingPixel);
			}
		}

		currentState = SpritePixelFetcherState::Idle;
	}

	uint16_t SpritePixelFetcher::GetCurrentSpriteTileAddress()
	{
		uint8_t tileIndex = currentSprite.tileIndex;

		// Get the tile-space scanline (a value between 0 and 7 for 8x8 sprites, or 0 and 15 for 8x16 sprites) based on the fetcher's Y position. 
		uint8_t scanline = y - currentSprite.y;

		// Determine the sprite's height (8 or 16).
		uint8_t spriteHeight = lcdc->GetBit(LCDC_OBJ_SIZE_BIT_INDEX) ? MAX_SPRITE_HEIGHT_IN_PIXELS : MIN_SPRITE_HEIGHT_IN_PIXELS;

		if (currentSprite.yFlip)
			scanline = (spriteHeight - 1) - scanline;

		// If the scanline belongs to the sprite's bottom tile, then increase the tile index so that it points to the bottom tile.
		if (spriteHeight == MAX_SPRITE_HEIGHT_IN_PIXELS && scanline >= MIN_SPRITE_HEIGHT_IN_PIXELS)
			tileIndex++;

		return GetTileAddress(tileIndex, scanline % TILE_HEIGHT_IN_PIXELS, true);
	}
}