#include <string>
#include "Audio/SoundChannel.hpp"
#include "Utils/GBSpecs.hpp"
#include "Logger.hpp"

namespace SHG
{
	const float MAX_UNSCALED_VOLUME = 15.0f;
	const uint8_t ENVELOPE_TIMER_PERIOD = 8;
	const uint8_t LENGTH_TIMER_PERIOD = 2;

	SoundChannel::SoundChannel()
	{
		
	}

	void SoundChannel::TickFrequencyTimer()
	{
		if (frequencyTimer.Tick())
			OnFrequencyTimerReachedZero();
	}

	void SoundChannel::TickVolumeEnvelopeTimer()
	{
		if (envelopeTimer.Tick())
		{
			// Decrement or increment the volume depending on the envelope direction.
			int16_t newVolume = volume + static_cast<int8_t>(GetVolumeEnvelopeDirection());

			// If the new volume does not overflow, then update the volume.
			if (newVolume >= 0 && newVolume <= MAX_UNSCALED_VOLUME)
				volume = newVolume;
		}
	}

	void SoundChannel::TickLengthControlTimer()
	{
		// If the length timer reaches 0, and the respective bit in NRX4 is set, 
		// the this channel should be disabled.
		if (lengthTimer.Tick() && IsConstrainedByLength())
			isEnabled = false;
	}

	bool SoundChannel::IsEnabled()
	{
		return isEnabled;
	}

	float SoundChannel::GetSample() const
	{
		// Outputs an amplitude between -1 and 1.
		return isEnabled ? ((GenerateSample() /  MAX_UNSCALED_VOLUME) * 2) - 1 : 0;
	}

	void SoundChannel::Reset()
	{
		nrx0.Write(0);
		nrx1.Write(0);
		nrx2.Write(0);
		nrx3.Write(0);
		nrx4.Write(0);
	}

	void SoundChannel::WriteToNRX0(uint8_t value)
	{
		nrx0.Write(value);
	}

	void SoundChannel::WriteToNRX1(uint8_t value)
	{
		nrx1.Write(value);
	}

	void SoundChannel::WriteToNRX2(uint8_t value)
	{
		nrx2.Write(value);
	}

	void SoundChannel::WriteToNRX3(uint8_t value)
	{
		nrx3.Write(value);
	}

	void SoundChannel::WriteToNRX4(uint8_t value)
	{
		nrx4.Write(value);

		if ((value >> 7) == 1)
			OnTrigger();
	}

	uint8_t SoundChannel::ReadNRX0() const
	{
		return nrx0.Read();
	}

	uint8_t SoundChannel::ReadNRX1() const
	{
		return nrx1.Read();
	}

	uint8_t SoundChannel::ReadNRX2() const
	{
		return nrx2.Read();
	}

	uint8_t SoundChannel::ReadNRX3() const
	{
		return nrx3.Read();
	}

	uint8_t SoundChannel::ReadNRX4() const
	{
		return nrx4.Read();
	}

	void SoundChannel::OnTrigger()
	{
		isEnabled = true;

		if (lengthTimer.IsStopped())
			ReloadLengthTimer();

		frequencyTimer.SetPeriod(GetFrequencyTimerPeriod());
		frequencyTimer.Restart();

		envelopeTimer.SetPeriod(GetVolumeEnvelopeTimerPeriod());
		envelopeTimer.Restart();

		volume = GetInitialEnvelopeVolume();
	}

	void SoundChannel::DisableVolumeEnvelope()
	{
		envelopeTimer.Disable();
	}

	void SoundChannel::ReloadLengthTimer()
	{
		lengthTimer.SetPeriod(GetLengthTimerPeriod());
		lengthTimer.Restart();
	}

	void SoundChannel::OnFrequencyTimerReachedZero()
	{

	}
}