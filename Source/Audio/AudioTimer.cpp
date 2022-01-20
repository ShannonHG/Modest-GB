#include "Audio/AudioTimer.hpp"

namespace ModestGB
{
	bool AudioTimer::Tick()
	{
		if (!isEnabled)
			return false;

		if (counter > 0)
		{
			counter--;
			if (counter == 0)
				return true;
		}

		return false;
	}

	void AudioTimer::Disable()
	{
		isEnabled = false;
		counter = 0;
	}

	void AudioTimer::Enable()
	{
		isEnabled = true;
	}

	void AudioTimer::Restart(uint16_t period)
	{
		counter = period;
		Enable();
	}

	bool AudioTimer::IsStopped()
	{
		return counter == 0;
	}
}