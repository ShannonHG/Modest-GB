#pragma once

namespace SHG
{
	enum class LCDStatusFlags
	{
		ModeFlag0 = 0,
		ModeFlag1 = 1,
		LYCLYFlag = 2,
		HBlankStatInterrupt = 3,
		VBlankStatInterrupt = 4,
		OAMStatInterrupt = 5,
		LYCLYStatInterrupt = 6
	};
}