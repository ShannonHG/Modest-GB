#include "Logger.hpp"

namespace SHG
{
	LogLevel Logger::CurrentLogLevel = LogLevel::Verbose;

	void Logger::Write(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Log)) std::cout << "[LOG] " << message << std::endl;
	}

	void Logger::WriteWarning(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Warning))std::cout << "[WARNING] " << message << std::endl;
	}

	void Logger::WriteError(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Error)) std::cout << "[ERROR] " << message << std::endl;
	}

	bool Logger::IsLogLevelEnabled(LogLevel logLevel)
	{
		return CurrentLogLevel == LogLevel::Verbose || CurrentLogLevel == logLevel;
	}
}