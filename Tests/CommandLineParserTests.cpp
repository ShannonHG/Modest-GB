#include "gtest/gtest.h"
#include "CommandLineParser.hpp"

namespace SHG
{
	char* TEST_EXE = "test.exe";
	char* TEST_ROM_FILE_PATH = "romFile.gb";
	char* LOG_LEVEL_OPTION = "-LogLevel";

	TEST(CommandLineParser, ReturnsFalseOnMissingROMFileArgument) {
		char* testArgs[] = { TEST_EXE };

		AppConfig config;
		bool success = false;

		// Verify that no errors are thrown due to no ROM file being provided
		ASSERT_NO_THROW(success = TryParseCommandLineArguments(1, testArgs, &config));

		EXPECT_EQ(success, false);
	}

	TEST(CommandLineParser, ReturnsTrueOnROMFileArgumentProvided)
	{
		char* testArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH };

		AppConfig config;
		bool success = false;

		success = TryParseCommandLineArguments(2, testArgs, &config);
		EXPECT_EQ(success, true);
	}

	TEST(CommandLineParser, IgnoresInvalidOptions)
	{
		char* testArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, "-a", "value", "-1", "value" };

		AppConfig config;
		bool success = false;

		success = TryParseCommandLineArguments(6, testArgs, &config);

		EXPECT_EQ(success, true);

		// Confirm that the log level is set to the default, 
		// and has not been affect by the invalid arguments
		EXPECT_EQ(config.logLevel, DEFAULT_LOG_LEVEL);

		// Confirm that the ROM file path was set properly
		EXPECT_EQ(config.romFilePath, TEST_ROM_FILE_PATH);
	}

	TEST(CommandLineParser, IgnoresOptionWithNoValue)
	{
		char* testArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, "-a" };

		AppConfig config;
		bool success = false;

		success = TryParseCommandLineArguments(2, testArgs, &config);

		EXPECT_EQ(success, true);

		// Confirm that the log level is set to the default, 
		// and has not been affect by the invalid arguments
		EXPECT_EQ(config.logLevel, DEFAULT_LOG_LEVEL);

		// Confirm that the ROM file path was set properly
		EXPECT_EQ(config.romFilePath, TEST_ROM_FILE_PATH);
	}

	TEST(CommandLineParser, SetsLogLevelToCorrectValue)
	{
		char* verboseTestArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "Verbose" };
		char* logTestArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "Log" };
		char* errorTestArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "Error" };
		char* warningTestArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "Warning" };
		char* silentTestArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "Silent" };

		int argCount = 4;

		AppConfig config;
		bool success = false;

		// Set the LogLevel to 'Verbose'
		success = TryParseCommandLineArguments(argCount, verboseTestArgs, &config);
		EXPECT_EQ(config.logLevel, LogLevel::Verbose);
		EXPECT_EQ(success, true);
		success = false;

		// Set the LogLevel to 'Log'
		success = TryParseCommandLineArguments(argCount, logTestArgs, &config);
		EXPECT_EQ(config.logLevel, LogLevel::Info);
		EXPECT_EQ(success, true);
		success = false;

		// Set the LogLevel to 'Error'
		success = TryParseCommandLineArguments(argCount, errorTestArgs, &config);
		EXPECT_EQ(config.logLevel, LogLevel::Error);
		EXPECT_EQ(success, true);
		success = false;

		// Set the LogLevel to 'Warning'
		success = TryParseCommandLineArguments(argCount, warningTestArgs, &config);
		EXPECT_EQ(config.logLevel, LogLevel::Warning);
		EXPECT_EQ(success, true);
		success = false;

		// Set the LogLevel to 'Silent'
		success = TryParseCommandLineArguments(argCount, silentTestArgs, &config);
		EXPECT_EQ(config.logLevel, LogLevel::Silent);
		EXPECT_EQ(success, true);
	}

	TEST(CommandLineParser, IgnoresInvalidLogLevelValue)
	{
		char* testArgs[] = { TEST_EXE, TEST_ROM_FILE_PATH, LOG_LEVEL_OPTION, "test" };

		AppConfig config;
		bool success = false;

		success = TryParseCommandLineArguments(4, testArgs, &config);
		EXPECT_EQ(success, true);

		// Confirm that the log level is set to the default, 
		// and has not been affect by the invalid arguments
		EXPECT_EQ(config.logLevel, DEFAULT_LOG_LEVEL);

		// Confirm that the ROM file path was set properly
		EXPECT_EQ(config.romFilePath, TEST_ROM_FILE_PATH);
	}
}