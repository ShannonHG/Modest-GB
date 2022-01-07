#pragma once
#include <cstdint>

namespace SHG
{
	class AudioTimer
	{
	public:
		bool Tick();
		void SetPeriod(uint16_t duration);
		void Restart(uint16_t period);
		bool IsStopped();
		void Disable();
		void Enable();

	private:
		uint16_t period = 0;
		uint16_t counter = 0;
		bool isEnabled = false;
	};
}