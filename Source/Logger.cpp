#include <filesystem>
#include <iostream>
#include "Logger.hpp"
#include <fstream>

namespace SHG
{
	const std::string LOG_FILE_NAME = "GBEmu.log";
	const std::string INFO_MESSAGE_HEADER = "[INFO]";
	const std::string WARNING_MESSAGE_HEADER = "[WARNING]";
	const std::string ERROR_MESSAGE_HEADER = "[ERROR]";

	LogLevel Logger::CurrentLogLevel = LogLevel::Verbose;
	std::ofstream Logger::LogFileStream;

	void Logger::Write(const std::string& message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Info)) WriteMessage(INFO_MESSAGE_HEADER, message, includeHeading, false);
	}

	void Logger::WriteWarning(const std::string& message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Warning)) WriteMessage(WARNING_MESSAGE_HEADER, message, includeHeading, true);
	}

	void Logger::WriteError(const std::string& message, bool includeHeading)
	{
		if (IsLogLevelEnabled(LogLevel::Error)) WriteMessage(ERROR_MESSAGE_HEADER, message, includeHeading, true);
	}

	bool Logger::IsLogLevelEnabled(LogLevel logLevel)
	{
		return (CurrentLogLevel == LogLevel::Verbose && logLevel != LogLevel::Silent) || CurrentLogLevel == logLevel;
	}

	void Logger::InitLogFile()
	{
		LogFileStream = std::ofstream(std::filesystem::current_path().string() + "/" + LOG_FILE_NAME, std::ios::out);
	}

	void Logger::WriteMessage(std::string heading, const std::string& message, bool includeHeading, bool writeToConsole)
	{
		if (!LogFileStream.is_open()) InitLogFile();
		if (includeHeading)
		{
			if (writeToConsole) std::cout << heading << " " << message << std::endl;
			LogFileStream << heading << " " << message << "\n";
		}
		else
		{
			if (writeToConsole) std::cout << " " << message << std::endl;
			LogFileStream << message << "\n";
		}
	}

	void Logger::WriteDivider()
	{
		if (!IsLogLevelEnabled(LogLevel::Silent)) LogFileStream << "---------------------------------------------------------------------------------" << "\n";
	}
}