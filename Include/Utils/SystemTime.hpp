#pragma once

namespace ModestGB::SystemTime
{
	struct DateTime
	{
		int seconds;
		int minutes;
		int hour;
		int day;
		int month;
		int year;
	};

	DateTime GetCurrentTime();
}