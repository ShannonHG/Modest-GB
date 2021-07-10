#pragma once

namespace SHG
{
	enum class LCDStatusModes
	{
		HBlank = 0,
		VBlank = 1,
		SearchingOAM = 2,
		LCDDataTransfer = 3
	};
}