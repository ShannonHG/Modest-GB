#include <filesystem>
#include <iostream>
#include "Logger.hpp"
#include <fstream>

namespace SHG
{
	LogLevel Logger::CurrentLogLevel = LogLevel::Verbose;
	std::ofstream Logger::LogFileStream;

	void Logger::Write(std::string message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Info)) WriteMessage("[INFO]", message, includeHeading);
	}

	void Logger::WriteWarning(std::string message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Warning)) WriteMessage("[WARNING]", message, includeHeading);
	}

	void Logger::WriteError(std::string message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Error)) WriteMessage("[ERROR]", message, includeHeading);
	}

	bool Logger::IsLogLevelEnabled(LogLevel logLevel)
	{
		return CurrentLogLevel == LogLevel::Verbose || CurrentLogLevel == logLevel;
	}

	void Logger::InitLogFile()
	{
		LogFileStream = std::ofstream(std::filesystem::current_path().string() + "/GBEmu.log", std::ios::out);
	}

	void Logger::WriteMessage(std::string heading, std::string message, bool includeHeading)
	{
		//if (!LogFileStream.is_open()) InitLogFile();
		if (includeHeading)
		{
			std::cout << heading << " " << message << std::endl;
			//LogFileStream << heading << " " << message << "\n";
		}
		else
		{
			std::cout << message << std::endl;
			//LogFileStream << message << "\n";
		}
	}

	void Logger::WriteDivider()
	{
		if (IsLogLevelEnabled(LogLevel::Info)) std::cout << "---------------------------------------------------------------------------------" << std::endl;
	}
}