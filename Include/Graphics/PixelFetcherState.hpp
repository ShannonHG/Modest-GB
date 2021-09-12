#pragma once

namespace SHG
{
	enum class PixelFetcherState
	{
		Idle,
		FetchingTile,
		FetchingLowTileData,
		FetchingHighTileData,
		Sleeping,
		PushingData
	};
}