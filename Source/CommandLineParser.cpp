#include <string>
#include <locale>
#include "CommandLineParser.hpp"

namespace SHG
{
	const uint8_t ROM_FILE_PATH_INDEX = 1;
	const char OPTION_PREFIX = '-';
	const std::string LOG_LEVEL_OPTION = "loglevel";
	const std::string SYSTEM_STATUS_DEBUG_OPTION = "logsystemevents";
	const std::string BACKGROUND_MAP_OPTION = "debugbackgroundmap";
	const std::string WINDOW_MAP_OPTION = "debugwindowmap";
	const std::string SPRITES_OPTION = "debugsprites";
	const std::string TILES_OPTION = "debugtiles";

	const std::vector<std::string> OPTIONS =
	{
		SYSTEM_STATUS_DEBUG_OPTION,
		BACKGROUND_MAP_OPTION,
		WINDOW_MAP_OPTION,
		SPRITES_OPTION,
		TILES_OPTION
	};

	void ConvertStringToLowercase(std::string& s);

	bool ParseROMFilePath(int argCount, char* args[], std::string& outFilePath)
	{
		// args[0] is the program execution command
		// args[1] is expected to be the ROM file. If no ROM is provided, then stop.
		if (argCount < 2)
			return false;

		outFilePath = args[ROM_FILE_PATH_INDEX];
		return true;
	}

	void ParseCommandLineOptions(int argCount, char* args[], AppConfig& outConfig)
	{
		outConfig.romFilePath = args[1];

		std::vector<std::string> parsedOptions;

		for (int i = ROM_FILE_PATH_INDEX + 1; i < argCount; i++)
		{
			std::string currentArg = args[i];

			size_t substrIndex = currentArg.find(OPTION_PREFIX);

			// If the argument does not begin with a hypen, then it is not a valid option.
			if (substrIndex == std::string::npos) continue;

			// Grab the substring that represents the option's name
			std::string option = currentArg.substr(substrIndex + 1, currentArg.size());

			// Ignore case
			ConvertStringToLowercase(option);

			// Validate the option
			if (std::find(OPTIONS.begin(), OPTIONS.end(), option) == OPTIONS.end())
			{
				Logger::WriteWarning("Ignoring invalid option '" + option + "'");
				continue;
			}

			// Ignore the option if it has already been found
			if (std::find(parsedOptions.begin(), parsedOptions.end(), option) != parsedOptions.end())
			{
				Logger::WriteWarning("Ignoring duplicate option '" + option + "'");
				continue;
			}

			if (option == SYSTEM_STATUS_DEBUG_OPTION)
			{
				outConfig.isSystemStatusLoggingEnabled = true;
				Logger::WriteInfo("System status logging enabled");
			}
			else if (option == BACKGROUND_MAP_OPTION)
			{
				outConfig.isBackgroundTileMapDebuggingEnabled = true;
				Logger::WriteInfo("Background tile map debugging enabled");
			}
			else if (option == WINDOW_MAP_OPTION)
			{
				outConfig.isWindowTileMapDebuggingEnabled = true;
				Logger::WriteInfo("Window tile map debugging enabled");
			}
			else if (option == SPRITES_OPTION)
			{
				outConfig.isSpriteDebuggingEnabled = true;
				Logger::WriteInfo("Sprite debugging enabled");
			}
			else if (option == TILES_OPTION)
			{
				outConfig.isGenericTileDebuggingEnabled = true;
				Logger::WriteInfo("Tile debugging enabled");
			}

			parsedOptions.push_back(option);
		}
	}

	void ConvertStringToLowercase(std::string& s)
	{
		for (char& c : s) c = std::tolower(c);
	}
}