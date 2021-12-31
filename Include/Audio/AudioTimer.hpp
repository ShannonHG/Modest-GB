#pragma once
#include <cstdint>

namespace SHG
{
	class AudioTimer
	{
	public:
		bool Tick();
		void SetPeriod(uint32_t duration);
		void Restart();
		bool IsStopped();
		void Disable();
		void Enable();

	private:
		uint32_t period = 0;
		uint32_t counter = 0;
		bool isEnabled = false;
	};
}