#pragma once
#include <vector>
#include <map>
#include "AppConfig.hpp"
#include "Logger.hpp"

namespace SHG
{
		/// <summary>
		/// Parses the given command line arguments, and stores relevant information in the config object.
		/// Returns false if no ROM file is provided.
		/// </summary>
		/// <param name="argCount"></param>
		/// <param name="args"></param>
		/// <param name="config"></param>
		/// <returns></returns>
		bool ParseCommandLineArguments(int argCount, char* args[], AppConfig* config);
}