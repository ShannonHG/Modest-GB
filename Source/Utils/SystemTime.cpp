#include <ctime>
#include <locale>
#include <chrono>
#include "Utils/SystemTime.hpp"

namespace SHG::SystemTime
{
	DateTime SHG::SystemTime::GetCurrentTime()
	{
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

		return DateTime
		{
			.seconds = std::min(dateTime.tm_sec, 59),
			.minutes = dateTime.tm_min,
			.hour = dateTime.tm_hour,
			.day = dateTime.tm_mday,
			.month = dateTime.tm_mon + 1,
			.year = dateTime.tm_year + 1900
		};
	}
}