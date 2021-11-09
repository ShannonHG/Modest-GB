#include <filesystem>
#include <ctime>
#include <locale>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "Logger.hpp"

namespace SHG
{
	const std::string LOG_FILE_NAME = "GBEmu.log";
	const std::string INFO_MESSAGE_HEADER = "[INFO]";
	const std::string WARNING_MESSAGE_HEADER = "[WARNING]";
	const std::string ERROR_MESSAGE_HEADER = "[ERROR]";
	const std::string SYSTEM_STATUS_MESSAGE_HEADER = "[SYSTEM]";

	std::ofstream logFileStream;
	std::array<char, 100> logDateTimeBuffer;
	std::vector<LogEntryEvent> logEntryCallbacks;

	bool Logger::IsSystemEventLoggingEnabled = false;

	void Logger::WriteInfo(const std::string& message, const std::string& header)
	{
		WriteMessage(INFO_MESSAGE_HEADER, message, true, header, LogMessageType::Info);
	}

	void Logger::WriteWarning(const std::string& message, const std::string& header)
	{
		WriteMessage(WARNING_MESSAGE_HEADER, message, true, header, LogMessageType::Warning);
	}

	void Logger::WriteError(const std::string& message, const std::string& header)
	{
		WriteMessage(ERROR_MESSAGE_HEADER, message, true, header, LogMessageType::Error);
	}

	void Logger::WriteSystemEvent(const std::string& message, const std::string& header)
	{
		if (IsSystemEventLoggingEnabled) WriteMessage(SYSTEM_STATUS_MESSAGE_HEADER, message, false, header, LogMessageType::SystemEvent);
	}

	void Logger::RegisterLogEntryAddedCallback(LogEntryEvent callback)
	{
		logEntryCallbacks.push_back(callback);
	}

	void Logger::InitLogFile()
	{
		logFileStream = std::ofstream(std::filesystem::current_path().string() + "/" + LOG_FILE_NAME, std::ios::out);
	}

	// TODO: Currently this only fires the log entry callback so that the log can be displayed on screen.
	// At some point, writing to an actual log file may be useful again.
	void Logger::WriteMessage(std::string heading, const std::string& message, bool writeToConsole, const std::string& customHeader, LogMessageType messageType)
	{
		if (!logFileStream.is_open()) InitLogFile();

		std::time_t rawTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm dateTime{};

		// Some compilers complain about localtime() being unsafe due to thread-safety issues, 
		// so depending on the platform, use the respective safe alternative.
#if defined(__unix__)
		localtime_r(&rawTime, &dateTime)
#elif defined(_MSC_VER)
		localtime_s(&dateTime, &rawTime);
#else
		dateTime = *std::localtime(&rawTime);
#endif

		// %F %T = Y-m-d H:M
		std::strftime(&logDateTimeBuffer[0], logDateTimeBuffer.size(), "%F %T", &dateTime);
		std::string formattedMessage = "[" + std::string(logDateTimeBuffer.data()) + "] " + heading + (customHeader.empty() ? customHeader : " " + customHeader) + ": " + message;

		if (writeToConsole) 
			std::cout << formattedMessage << std::endl;

		// TODO: std::endl will force the stream to be flushed and immediately written to file, consider removing this.
		logFileStream << formattedMessage << std::endl;

		for (LogEntryEvent callback : logEntryCallbacks)
			callback(formattedMessage, messageType);
	}
}