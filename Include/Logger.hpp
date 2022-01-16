#pragma once
#include <map>
#include <iostream>
#include <array>
#include <functional>

namespace SHG
{
	enum class LogMessageType
	{
		Info,
		Warning,
		Error,
		SystemEvent
	};

	using LogEntryEvent = std::function<void(std::string message, LogMessageType messageType)>;

	class Logger
	{
	public:
		static bool IsSystemEventLoggingEnabled;
		static void WriteInfo(const std::string& message, const std::string& header = "");
		static void WriteWarning(const std::string& message, const std::string& header = "");
		static void WriteError(const std::string& message, const std::string& header = "");
		static void WriteSystemEvent(const std::string& message, const std::string& header = "");
		static void RegisterLogEntryAddedCallback(LogEntryEvent callback);

	private:
		static void WriteMessage(std::string heading, const std::string& message, bool writeToConsole, const std::string& customHeader, LogMessageType messageType);
	};
}