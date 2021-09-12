#pragma once
#include <map>
#include <iostream>

namespace SHG
{
	enum class LogLevel
	{
		Verbose,
		Info,
		Error,
		Warning,
		Silent
	};

	const std::map<std::string, LogLevel> LOG_LEVEL_STRINGS_TO_ENUMS =
	{
		{"verbose", LogLevel::Verbose},
		{"log", LogLevel::Info},
		{"error", LogLevel::Error},
		{"warning", LogLevel::Warning},
		{"silent", LogLevel::Silent}
	};

	const std::map<LogLevel, std::string> LOG_LEVEL_ENUMS_TO_STRINGS =
	{
		{LogLevel::Verbose, "verbose"},
		{LogLevel::Info, "log"},
		{LogLevel::Error, "error"},
		{LogLevel::Warning, "warning"},
		{LogLevel::Silent, "silent"}
	};

	/// <summary>
	/// Writes messages to the console and/or log file.
	/// </summary>
	class Logger
	{
	public:
		/// <summary>
		/// The logger's current log level.
		/// </summary>
		static LogLevel CurrentLogLevel;

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Log"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void Write(const std::string& message, bool includeHeading = true);

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Warning"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void WriteWarning(const std::string& message, bool includeHeading = true);

		/// <summary>
		/// Writes a message to the console if <see cref="CurrentLogLevel"/> is 
		/// set to <see cref="LogLevel::Error"/> or <see cref="LogLevel::Verbose"/>
		/// </summary>
		/// <param name="message"></param>
		static void WriteError(const std::string& message, bool includeHeading = true);

		static void WriteDivider();

	private:
		static std::ofstream LogFileStream;

		static void InitLogFile();
		static bool IsLogLevelEnabled(LogLevel logLevel);
		static void WriteMessage(std::string heading, const std::string& message, bool includeHeading, bool writeToConsole);
	};
}