#pragma once
#include "Audio/SoundChannel.hpp"

namespace SHG
{
	class ToneSoundChannel : public SoundChannel
	{
	public:
		ToneSoundChannel();

		void Reset() override;
		virtual void WriteToNRX1(uint8_t value) override;
		virtual void WriteToNRX2(uint8_t value) override;

	protected:
		uint32_t GetFrequency() const;

		void OnFrequencyTimerReachedZero() override;
		float GenerateSample() const override;
		ModifierDirection GetVolumeEnvelopeDirection() const override;
		uint8_t GetInitialEnvelopeVolume() const override;
		bool IsConstrainedByLength() const override;
		virtual void OnTrigger() override;

	private:
		uint8_t waveformPosition = 0;

		uint32_t GetFrequencyTimerPeriod() const override;
		uint32_t GetVolumeEnvelopeTimerPeriod() const override;
		uint32_t GetLengthTimerPeriod() const override;
		uint8_t GetWaveformIndex() const;
	};
}