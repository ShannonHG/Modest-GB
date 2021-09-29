#pragma once
#include <vector>
#include <map>
#include "AppConfig.hpp"
#include "Logger.hpp"

namespace SHG
{
		void ParseCommandLineOptions(int argCount, char* args[], AppConfig& config);
		bool ParseROMFilePath(int argCount, char* args[], std::string& outFilePath);
}