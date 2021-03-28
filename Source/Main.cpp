#include "Logger.hpp"
#include "AppConfig.hpp"
#include "CommandLineParser.hpp"

int main(int argc, char* argv[]) 
{
	SHG::AppConfig config;

	if (SHG::CommandLineParser::ParseArguments(argc, argv, &config))
	{
		SHG::Logger::Write("Setting ROM file path to '" + config.romFilePath + "' ");
	}
	else
	{
		return 0;
	}

	SHG::Logger::Write("Setting log level to '" + SHG::LOG_LEVEL_ENUMS_TO_STRINGS.at(config.logLevel) + "'");

	return 0;
}