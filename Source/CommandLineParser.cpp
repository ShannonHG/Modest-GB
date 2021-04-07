#include <string>
#include <locale>
#include "CommandLineParser.hpp"

namespace SHG
{
	const char OPTION_PREFIX = '-';
	const std::string LOG_LEVEL_OPTION = "loglevel";

	const std::vector<std::string> OPTIONS = 
	{
		LOG_LEVEL_OPTION,
	};

	bool ParseArgumentValue(std::string argKey, std::string argValue, AppConfig* config);
	bool ParseLogLevel(std::string argValue, LogLevel* logLevel);
	std::string ConvertStringToLowercase(std::string s);

	bool TryParseCommandLineArguments(int argCount, char* args[], AppConfig* config)
	{
		// args[0] is the program execution command
		// args[1] is expected to be the ROM file. If no ROM is provided, then stop.
		if (argCount < 2)
		{
			Logger::WriteError("No ROM file provided");
			return false;
		}

		config->romFilePath = args[1];

		std::vector<std::string> parsedOptions;

		for (int i = 2; i < argCount; i += 2)
		{
			std::string currentArg = args[i];

			// Grab the substring that represents the option's name
			std::string option = currentArg.substr(currentArg.find(OPTION_PREFIX) + 1, currentArg.size());

			// Ignore case
			option = ConvertStringToLowercase(option);

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

			// Check if the option has a value associated with it
			if (i == argCount - 1)
			{
				Logger::WriteWarning("Ignoring option '" + option + "' since no value was provided");
				break;
			}

			// Read the option's value, ignoring its case
			std::string optionValue = ConvertStringToLowercase(args[i + 1]);

			// Try to load the argument value into the appropriate config field
			if (!ParseArgumentValue(option, optionValue, config))
			{
				Logger::WriteWarning("Invalid value given for option '" + option + "'");
			}
			else
			{
				parsedOptions.push_back(option);
			}
		}

		return true;
	}

	bool ParseArgumentValue(std::string option, std::string optionValue, AppConfig* result)
	{
		if (option == LOG_LEVEL_OPTION)
		{
			return ParseLogLevel(optionValue, &result->logLevel);
		}

		return false;
	}

	bool ParseLogLevel(std::string optionValue, LogLevel* logLevel)
	{
		// Confirm that the option's value exists in the LOG_LEVEL_STRINGS map
		if (LOG_LEVEL_STRINGS_TO_ENUMS.find(optionValue) == LOG_LEVEL_STRINGS_TO_ENUMS.end())
		{
			return false;
		}

		*logLevel = LOG_LEVEL_STRINGS_TO_ENUMS.at(optionValue);
		return true;
	}

	std::string ConvertStringToLowercase(std::string s)
	{
		std::string result = s;

		for (char& c : result) c = std::tolower(c);

		return result;
	}
}