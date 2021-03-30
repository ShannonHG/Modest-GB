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
		Warning,
		Silent
	};

	const std::map<std::string, LogLevel> LOG_LEVEL_STRINGS_TO_ENUMS =
	{
		{"verbose", LogLevel::Verbose},
		{"log", LogLevel::Log},
		{"error", LogLevel::Error},
		{"warning", LogLevel::Warning},
		{"silent", LogLevel::Silent}
	};

	const std::map<LogLevel, std::string> LOG_LEVEL_ENUMS_TO_STRINGS =
	{
		{LogLevel::Verbose, "verbose"},
		{LogLevel::Log, "log"},
		{LogLevel::Error, "error"},
		{LogLevel::Warning, "warning"},
		{LogLevel::Silent, "silent"}
	};

	class Logger
	{
	public:
		static LogLevel CurrentLogLevel;

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Log"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void Write(std::string message);

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Warning"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void WriteWarning(std::string message);

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Error"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void WriteError(std::string message);

	private:
		static bool IsLogLevelEnabled(LogLevel logLevel);
	};
}