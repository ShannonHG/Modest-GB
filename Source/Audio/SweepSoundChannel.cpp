#include <string>
#include "Audio/SweepSoundChannel.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"

namespace ModestGB
{
	const uint16_t MAX_FREQUENCY = 2047;

	SweepSoundChannel::SweepSoundChannel() : ToneSoundChannel()
	{

	}

	void SweepSoundChannel::Reset()
	{
		ToneSoundChannel::Reset();

		sweepTimer.Disable();
		shadowFrequency = 0;
		isSweepEnabled = false;
	}

	void SweepSoundChannel::TickSweepTimer()
	{
		if (!isSoundControllerEnabled)
			return;

		if (sweepTimer.Tick())
		{
			ReloadSweepTimer();

			if (isSweepEnabled && GetSweepPeriod() != 0)
			{
				int32_t newShadowFrequency = CalculateShadowFrequency();

				if (GetSweepShift() != 0 && PerformOverflowCheck(newShadowFrequency))
				{
					// Update the frequency bits stored in NR13 and NR14.
					UpdateFrequency(newShadowFrequency);

					// The frequency calculation and overflow check are performed again, 
					// but the new frequency is not used.
					PerformOverflowCheck(CalculateShadowFrequency());
				}
			}
		}
	}

	uint8_t SweepSoundChannel::ReadNRX0() const
	{
		return SoundChannel::ReadNRX0() | 0x80;
	}

	void SweepSoundChannel::OnTrigger()
	{
		ToneSoundChannel::OnTrigger();

		// Copy the frequency to the shadow register
		shadowFrequency = GetFrequency();
		ReloadSweepTimer();

		uint8_t sweepPeriod = GetSweepPeriod();
		uint8_t sweepShift = GetSweepShift();

		// Enable the sweep functionality if either the sweep period or sweep shift are non-zero, otherwise disable it.
		isSweepEnabled = sweepPeriod != 0 || sweepShift != 0;

		// If the sweep shift is non-zero, perform the frequency calculation and overflow check.
		if (sweepShift != 0)
		{
			uint32_t newFrequency = CalculateShadowFrequency();
			PerformOverflowCheck(newFrequency);
		}
	}

	int32_t SweepSoundChannel::CalculateShadowFrequency() const
	{
		return shadowFrequency + ((shadowFrequency >> GetSweepShift()) * static_cast<int8_t>(GetSweepDirection()));
	}

	bool SweepSoundChannel::PerformOverflowCheck(int32_t newFrequency)
	{
		// The frequency of this channel is 11 bits wide, so if the new 
		// frequency is larger than 2047 (max value that can be represented with 11 bits), 
		// then the new frequency should not be used, and this channel should be disabled.
		if (newFrequency < 0 || newFrequency > MAX_FREQUENCY)
		{
			isEnabled = false;
			return false;
		}

		return true;
	}

	void SweepSoundChannel::UpdateFrequency(uint16_t newFrequency)
	{
		shadowFrequency = newFrequency;

		// Lower 8 bits of 11 bit frequency
		nrx3.Write(shadowFrequency & 255);

		// Upper 3 bits of 11 bit frequency
		nrx4.Write(nrx4.Read(3, 7) | (shadowFrequency >> 8) & 0b111);
	}

	uint8_t SweepSoundChannel::GetSweepPeriod() const
	{
		return nrx0.Read(4, 6);
	}

	SoundChannel::ModifierDirection SweepSoundChannel::GetSweepDirection() const
	{
		return nrx0.Read(3) ? ModifierDirection::Decrease : ModifierDirection::Increase;
	}

	uint8_t SweepSoundChannel::GetSweepShift() const
	{
		return nrx0.Read(0, 2);
	}

	void SweepSoundChannel::ReloadSweepTimer()
	{
		uint16_t period = GetSweepPeriod();

		// The sweep timer treats a period of 0 as 8.
		sweepTimer.Restart(period == 0 ? 8 : period);
	}
}