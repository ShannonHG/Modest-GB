#include "Logger.hpp"
#include "AppConfig.hpp"
#include "CommandLineParser.hpp"
#include "Emulator.hpp"

int main(int argc, char* argv[])
{
	/*SHG::AppConfig config;

	if (SHG::ParseROMFilePath(argc, argv, config.romFilePath))
	{
		SHG::Logger::WriteInfo("ROM file path: '" + config.romFilePath + "' ");
	}
	else
	{
		SHG::Logger::WriteError("No ROM file provided. Exiting...");
		return 0;
	}

	SHG::ParseCommandLineOptions(argc, argv, config);
	SHG::Logger::IsSystemEventLoggingEnabled = config.isSystemStatusLoggingEnabled;*/

	SHG::Emulator().Start();

	return 0;
}