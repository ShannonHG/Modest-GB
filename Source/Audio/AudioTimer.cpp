#include "Audio/AudioTimer.hpp"

namespace SHG
{
	bool AudioTimer::Tick()
	{
		if (!isEnabled)
			return false;

		if (counter > 0)
		{
			counter--;
			if (counter == 0)
			{
				Restart();
				return true;
			}
		}

		return false;
	}

	void AudioTimer::Disable()
	{
		isEnabled = false;
		counter = 0;
		period = 0;
	}

	void AudioTimer::Enable()
	{
		isEnabled = true;
	}

	void AudioTimer::SetPeriod(uint32_t duration)
	{
		period = duration;
	}

	void AudioTimer::Restart()
	{
		counter = period;
		Enable();
	}

	bool AudioTimer::IsStopped()
	{
		return counter == 0;
	}
}