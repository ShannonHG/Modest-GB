#pragma once
#include <map>
#include <iostream>
#include <array>

namespace SHG
{
	class Logger
	{
	public:
		static bool IsSystemEventLoggingEnabled;
		static void WriteInfo(const std::string& message, const std::string& header = "");
		static void WriteWarning(const std::string& message, const std::string& header = "");
		static void WriteError(const std::string& message, const std::string& header = "");
		static void WriteSystemEvent(const std::string& message, const std::string& header = "");

	private:
		static void InitLogFile();
		static void WriteMessage(std::string heading, const std::string& message, bool writeToConsole, const std::string& customHeader);
	};
}