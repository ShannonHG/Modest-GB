#include "Logger.hpp"
#include "AppConfig.hpp"
#include "CommandLineParser.hpp"
#include "Emulator.hpp"

int main(int argc, char* argv[])
{
	return SHG::Emulator().Run();
}