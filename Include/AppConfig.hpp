#pragma once
#include <string>
#include "Logger.hpp"

namespace SHG
{
	struct AppConfig
	{
		std::string romFilePath;
		bool isSystemStatusLoggingEnabled = false;
		bool isBackgroundTileMapDebuggingEnabled = false;
		bool isWindowTileMapDebuggingEnabled = false;
		bool isSpriteDebuggingEnabled = false;
		bool isGenericTileDebuggingEnabled = false;
	};
}