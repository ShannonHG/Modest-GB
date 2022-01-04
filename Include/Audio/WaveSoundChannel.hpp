#pragma once
#include <cstdint>
#include <vector>
#include "Audio/SoundChannel.hpp"

namespace SHG
{
	class WaveSoundChannel : public SoundChannel
	{
	public:
		void Reset() override;
		WaveSoundChannel();
		void WriteToWavePatternRAM(uint16_t address, uint8_t value);

	protected:
		uint32_t GetLengthTimerPeriod() const override;
		uint32_t GetFrequencyTimerPeriod() const override;
		float GenerateSample() const override;
		bool IsConstrainedByLength() const override;
		void OnFrequencyTimerReachedZero() override;
		void OnTrigger() override;
		ModifierDirection GetVolumeEnvelopeDirection() const;
		uint8_t GetInitialEnvelopeVolume() const;
		uint32_t GetVolumeEnvelopeTimerPeriod() const;

	private:
		uint8_t sampleIndex = 0;
		std::vector<uint8_t> samples;

		uint8_t GetVolumeControlShift() const;
	};
}