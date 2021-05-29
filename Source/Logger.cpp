#include <filesystem>
#include <iostream>
#include "Logger.hpp"
#include <fstream>

namespace SHG
{
	LogLevel Logger::CurrentLogLevel = LogLevel::Verbose;
	std::ofstream Logger::LogFileStream;

	void Logger::Write(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Info)) WriteMessage("[INFO] " + message);
	}

	void Logger::WriteWarning(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Warning)) WriteMessage("[WARNING] " + message);
	}

	void Logger::WriteError(std::string message)
	{
		if (IsLogLevelEnabled(LogLevel::Error)) WriteMessage("[ERROR] " + message);
	}

	bool Logger::IsLogLevelEnabled(LogLevel logLevel)
	{
		return CurrentLogLevel == LogLevel::Verbose || CurrentLogLevel == logLevel;
	}

	void Logger::InitLogFile()
	{
		LogFileStream = std::ofstream(std::filesystem::current_path().string() + "/GBEmu.log", std::ios::out);
	}

	void Logger::WriteMessage(std::string message)
	{
		std::cout << message << std::endl;

	/*	if (!LogFileStream.is_open()) InitLogFile();
		LogFileStream << message << std::endl;*/
	}
}