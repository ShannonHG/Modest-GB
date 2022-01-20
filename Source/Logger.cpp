#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "Logger.hpp"
#include "Utils/SystemTime.hpp"

namespace ModestGB
{
	const std::string INFO_MESSAGE_HEADER = "[INFO]";
	const std::string WARNING_MESSAGE_HEADER = "[WARNING]";
	const std::string ERROR_MESSAGE_HEADER = "[ERROR]";
	const std::string SYSTEM_STATUS_MESSAGE_HEADER = "[SYSTEM]";

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

	void Logger::WriteMessage(std::string heading, const std::string& message, bool writeToConsole, const std::string& customHeader, LogMessageType messageType)
	{
		SystemTime::DateTime currentTime = SystemTime::GetCurrentTime();

		std::stringstream formattedTime;

		// Y-m-d H:M
		formattedTime << currentTime.year << "-" << currentTime.month << "-" << currentTime.day << " " << currentTime.hour << ":" << std::setw(2) << std::setfill('0') << currentTime.minutes;

		std::string formattedMessage = "[" + formattedTime.str() + "] " + heading + (customHeader.empty() ? customHeader : " " + customHeader) + ": " + message;

		if (writeToConsole) 
			std::cout << formattedMessage << std::endl;

		for (LogEntryEvent callback : logEntryCallbacks)
			callback(formattedMessage, messageType);
	}
}