#pragma once
#include <cstdint>

namespace ModestGB
{
	class AudioTimer
	{
	public:
		bool Tick();
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