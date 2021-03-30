#include "Logger.hpp"
#include "AppConfig.hpp"
#include "CommandLineParser.hpp"
#include "Cartridge.hpp"

using namespace SHG;

int main(int argc, char* argv[]) 
{
	AppConfig config;

	if (ParseCommandLineArguments(argc, argv, &config))
	{
		Logger::Write("Setting ROM file path to '" + config.romFilePath + "' ");
	}
	else
	{
		return 0;
	}

	Logger::Write("Setting log level to '" + LOG_LEVEL_ENUMS_TO_STRINGS.at(config.logLevel) + "'");

	Cartridge cartridge;
	cartridge.LoadFromFile(config.romFilePath);

	return 0;
}