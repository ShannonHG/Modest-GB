#pragma once
#include <string>
#include "Logger.hpp"

namespace SHG
{
	const LogLevel DEFAULT_LOG_LEVEL = LogLevel::Verbose;

	struct AppConfig
	{
		std::string romFilePath;
		LogLevel logLevel = DEFAULT_LOG_LEVEL;
	};
}