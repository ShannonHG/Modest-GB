#include <map>
#include <string>
#include "Audio/ToneSoundChannel.hpp"
#include "Logger.hpp"

namespace SHG
{
	const std::map<uint8_t, std::vector<bool>> WAVEFORMS =
	{
		{ 0,  {0, 0, 0, 0, 0, 0, 0, 1} },
		{ 1,  {1, 0, 0, 0, 0, 0, 0, 1} },
		{ 2,  {1, 0, 0, 0, 0, 1, 1, 1} },
		{ 3,  {0, 1, 1, 1, 1, 1, 1, 0} }
	};

	ToneSoundChannel::ToneSoundChannel() : SoundChannel()
	{

	}

	void ToneSoundChannel::Reset()
	{
		SoundChannel::Reset();

		waveformPosition = 0;
	}

	void ToneSoundChannel::WriteToNRX1(uint8_t value)
	{
		SoundChannel::WriteToNRX1(value);

		ReloadLengthTimer();
	}

	void ToneSoundChannel::WriteToNRX2(uint8_t value)
	{
		SoundChannel::WriteToNRX2(value);

		// If the period is 0, then stop the volume envelope.
		if (GetVolumeEnvelopeTimerPeriod() == 0)
			DisableVolumeEnvelope();
	}

	float ToneSoundChannel::GenerateSample() const
	{
		return WAVEFORMS.at(GetWaveformIndex())[waveformPosition] * volume;
	}

	uint32_t ToneSoundChannel::GetFrequency() const
	{
		// Bits 0 - 2 of NRX4 define the upper 3 bits of the 11 bit frequency, 
		// and NRX3 defines the lower 8 bits.
		return (nrx4.Read(0, 2) << 8) | nrx3.Read();
	}

	uint8_t ToneSoundChannel::GetWaveformIndex() const
	{
		return nrx1.Read(6, 7);
	}

	void ToneSoundChannel::OnFrequencyTimerReachedZero()
	{
		waveformPosition = (waveformPosition + 1) % 8;
	}

	void ToneSoundChannel::OnTrigger() 
	{
		// TODO: The low 2 bits of the frequency timer should not be modified.
		SoundChannel::OnTrigger();

		waveformPosition = 0;
	}

	SoundChannel::ModifierDirection ToneSoundChannel::GetVolumeEnvelopeDirection() const
	{
		return nrx2.Read(3) ? ModifierDirection::Increase : ModifierDirection::Decrease;
	}

	uint8_t ToneSoundChannel::GetInitialEnvelopeVolume() const
	{
		return nrx2.Read(4, 7);
	}

	bool ToneSoundChannel::IsConstrainedByLength() const
	{
		return nrx4.Read(6);
	}

	uint32_t ToneSoundChannel::GetFrequencyTimerPeriod() const
	{
		return ((2048 - GetFrequency()) * 4);
	}

	uint32_t ToneSoundChannel::GetVolumeEnvelopeTimerPeriod() const
	{
		return nrx2.Read(0, 2);
	}

	uint32_t ToneSoundChannel::GetLengthTimerPeriod() const
	{
		return nrx1.Read(0, 5);
	}
}