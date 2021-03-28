#pragma once
#include <vector>
#include <map>
#include "AppConfig.hpp"
#include "Logger.hpp"

namespace SHG
{
	class CommandLineParser
	{
	public:
		/// <summary>
		/// Parses the given command line arguments, and stores relevant information in the config object.
		/// Returns false if no ROM file is provided.
		/// </summary>
		/// <param name="argCount"></param>
		/// <param name="args"></param>
		/// <param name="config"></param>
		/// <returns></returns>
		static bool ParseArguments(int argCount, char* args[], AppConfig* config);

	private: 
		static bool ParseArgumentValue(std::string argKey, std::string argValue, AppConfig* config);
		static bool ParseLogLevel(std::string argValue, LogLevel* logLevel);
		static std::string ConvertStringToLowercase(std::string s);
	};
}