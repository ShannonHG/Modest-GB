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
		return (((nrx4 & 0b111) << 8) | nrx3);
	}

	uint8_t ToneSoundChannel::GetWaveformIndex() const
	{
		// Bits 6 - 7 of NRX1 will determine which waveform to use.
		return (nrx1 >> 6) & 0b11;
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
		return ((nrx2 >> 3) & 1) == 0 ? ModifierDirection::Decrease : ModifierDirection::Increase;
	}

	uint8_t ToneSoundChannel::GetInitialEnvelopeVolume() const
	{
		return (nrx2 >> 4) & 0b1111;
	}

	bool ToneSoundChannel::IsConstrainedByLength() const
	{
		return (nrx4 >> 6) & 1;
	}

	uint32_t ToneSoundChannel::GetFrequencyTimerPeriod() const
	{
		return ((2048 - GetFrequency()) * 4);
	}

	uint32_t ToneSoundChannel::GetVolumeEnvelopeTimerPeriod() const
	{
		// Bits 0 - 2 of NRX2 determine the volume envelope timer's period.
		return nrx2 & 0b111;
	}

	uint32_t ToneSoundChannel::GetLengthTimerPeriod() const
	{
		// Bits 0 - 5 of NRX1 determine the length timer's period.
		return nrx1 & 0b111111;
	}
}