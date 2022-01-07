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

		uint8_t ReadNRX0() const override;
		uint8_t ReadNRX1() const override;
		uint8_t ReadNRX2() const override;
		uint8_t ReadNRX3() const override;
		uint8_t ReadNRX4() const override;

	protected:
		uint16_t GetFrequency() const;
		void OnFrequencyTimerReachedZero() override;
		float GenerateSample() const override;
		ModifierDirection GetVolumeEnvelopeDirection() const override;
		uint8_t GetInitialEnvelopeVolume() const override;
		bool IsConstrainedByLength() const override;
		virtual void OnTrigger() override;

	private:
		uint8_t waveformPosition = 0;

		uint16_t GetFrequencyTimerPeriod() const override;
		uint16_t GetVolumeEnvelopeTimerPeriod() const override;
		uint16_t GetLengthTimerPeriod() const override;
		uint8_t GetWaveformIndex() const;
	};
}