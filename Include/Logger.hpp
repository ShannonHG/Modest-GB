#pragma once
#include <map>
#include <iostream>

namespace SHG
{
	enum class LogLevel
	{
		Verbose,
		Log,
		Error,
		Warning
	};

	const std::map<std::string, LogLevel> LOG_LEVEL_STRINGS_TO_ENUMS =
	{
		{"verbose", LogLevel::Verbose},
		{"log", LogLevel::Log},
		{"error", LogLevel::Error},
		{"warning", LogLevel::Warning}
	};

	const std::map<LogLevel, std::string> LOG_LEVEL_ENUMS_TO_STRINGS =
	{
		{LogLevel::Verbose, "verbose"},
		{LogLevel::Log, "log"},
		{LogLevel::Error, "error"},
		{LogLevel::Warning, "warning"}
	};

	class Logger
	{
	public:
		static LogLevel CurrentLogLevel;
		static void Write(std::string message);
		static void WriteWarning(std::string message);
		static void WriteError(std::string message);

	private:
		static bool IsLogLevelEnabled(LogLevel logLevel);
	};
}